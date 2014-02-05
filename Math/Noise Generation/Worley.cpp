#include "Worley.h"

#include <vector>
#include <queue>
#include "NoiseFilterer.h"

#include <memory>
#define PTR(X) (std::unique_ptr<X>)


const float BadWADDistance = -1.0f;
//Stores a generated point in a cell and the distance to that point.
struct WorleyAlgorithmData { Vector2f Pos; float Distance; WorleyAlgorithmData(Vector2f pos = Vector2f(), float distance = BadWADDistance) : Pos(pos), Distance(distance) { } void operator=(WorleyAlgorithmData & copy) { Pos = copy.Pos; Distance = copy.Distance; } };
typedef WorleyAlgorithmData WAD;
bool IsGoodData(WAD & dat) { return dat.Distance != BadWADDistance; }

void Insert(WorleyAlgorithmData* toInsertInto, WorleyAlgorithmData & toInsert)
{
	//Only need to use the closest 5 elements.
	const int maxSize = Worley::NUMB_DISTANCE_VALUES;

	int i = 0;
	for (i = 0; i < maxSize && IsGoodData(toInsertInto[i]); ++i)
	{
		//If this is the right spot to insert the data...
		if (toInsertInto[i].Distance > toInsert.Distance)
		{
			//Move all the other data up.
			for (int j = maxSize - 1; j > i; --j)
			{
				toInsertInto[j].Pos = toInsertInto[j - 1].Pos;
				toInsertInto[j].Distance = toInsertInto[j - 1].Distance;
			}
			//Insert the element.
			toInsertInto[i].Pos = toInsert.Pos;
			toInsertInto[i].Distance = toInsert.Distance;
			return;
		}
	}

	//If there is still space at the end of the list, insert it.
	if (i < maxSize)
	{
		toInsertInto[i].Pos = toInsert.Pos;
	}
	if (i < maxSize)
	{
		toInsertInto[i].Distance = toInsert.Distance;
	}
}

void Worley::Generate(Fake2DArray<float> & noise) const
{
	//Get the size of a cell.
	int cSize = CellSize;
	if (cSize > noise.GetWidth())
	{
		cSize = noise.GetWidth();
	}
	if (cSize > noise.GetHeight())
	{
		cSize = noise.GetHeight();
	}

	//Get the number of cells (use one extra row/column of cells behind the noise array).
	Vector2i cells = Vector2i((noise.GetWidth() / cSize),
							  (noise.GetHeight() / cSize));

	//Initialize cell contents.
	int i, x, y;
	struct Cell { std::vector<Vector2f> pointsInCell; };
	Fake2DArray<Cell> cellContents(cells.x, cells.y);

	//Generate cell contents.
	FastRand fr(Seed);
	Interval tempIntX, tempIntY;
	std::vector<Vector2f> * cellTemp;
	int pointsInCell;
	for (x = 0; x < cells.x; ++x)
	{
		tempIntX = Interval(x * cSize, (x * cSize) + cSize, 0.001f, true, true);

		for (y = 0; y < cells.y; ++y)
		{
			tempIntY = Interval(y * cSize, (y * cSize) + cSize, 0.001f, true, true);

			cellTemp = &cellContents[Vector2i(x, y)].pointsInCell;

			//Generate some randomized number of points in this cell.
			pointsInCell = BasicMath::RoundToInt(PointsPerCell.RandomInsideRange(fr));
			for (int i = 0; i < pointsInCell; ++i)
			{
				cellTemp->insert(cellTemp->end(), Vector2f(tempIntX.RandomInsideRange(fr), tempIntY.RandomInsideRange(fr)));
			}
		}
	}


	//Get n-th-closest point for every noise element.

	NoiseAnalysis2D::MinMax mm(999999.0f, -999999.0f);
	DistanceValues vals;
	WAD valsData[NUMB_DISTANCE_VALUES];
	WAD tempWAD;
	int x2, y2;
	Vector2f tempPos;
	Vector2i tempPosi;
    Vector2i tempCellLoc;
	float tempNoiseVal;
	//Go through every noise element.
	for (x = 0; x < noise.GetWidth(); ++x)
	{
		tempPos.x = (float)x;
		tempPosi.x = x;
		tempCellLoc.x = x / CellSize;

		for (y = 0; y < noise.GetHeight(); ++y)
		{
			tempPos.y = (float)y;
			tempPosi.y = y;
			tempCellLoc.y = y / CellSize;

			for (i = 0; i < NUMB_DISTANCE_VALUES; ++i)
			{
				valsData[i] = WAD();
			}

			//Loop through every surrounding cell.
			for (x2 = -1; x2 <= 1; ++x2)
			{
				for (y2 = -1; y2 <= 1; ++y2)
				{
                    //If the cell area is out of bounds, either wrap it or stop this iteration.
                    Vector2i tempLoci(tempCellLoc.x + x2, tempCellLoc.y + y2);
                    Vector2i newTempLoci = cellContents.Wrap(tempLoci);
                    bool wrappedLeftX = (tempLoci.x < newTempLoci.x),
                         wrappedLeftY = (tempLoci.y < newTempLoci.y),
                         wrappedRightX = (tempLoci.x > newTempLoci.x),
                         wrappedRightY = (tempLoci.y > newTempLoci.y);

					//Go through every point in the cell.
					cellTemp = &cellContents[newTempLoci].pointsInCell;

					for (i = 0; i < (int)cellTemp->size(); ++i)
					{
						//Put it into the ordered list of distances.
						tempWAD.Pos = (*cellTemp)[i];
                        if (wrappedLeftX) tempWAD.Pos.x -= noise.GetWidth();
                        if (wrappedLeftY) tempWAD.Pos.y -= noise.GetHeight();
                        if (wrappedRightX) tempWAD.Pos.x += noise.GetWidth();
                        if (wrappedRightY) tempWAD.Pos.y += noise.GetHeight();

						tempWAD.Distance = DistFunc(tempPos, tempWAD.Pos);

						if (!IsGoodData(valsData[NUMB_DISTANCE_VALUES - 1]) ||
							tempWAD.Distance < valsData[NUMB_DISTANCE_VALUES - 1].Distance)
						{
							Insert(valsData, tempWAD);
						}
					}
				}
			}

			//Now that we have the points, turn it into a noise value.
			for (i = 0; i < NUMB_DISTANCE_VALUES; ++i)
			{
				if (IsGoodData(valsData[i]))
				{
					vals.Values[i] = valsData[i].Distance;
				}
				else
				{
					vals.Values[i] = 0.0f;
				}
			}
			tempNoiseVal = ValueGenerator(vals);

			if (tempNoiseVal < mm.Min) mm.Min = tempNoiseVal;
			if (tempNoiseVal > mm.Max) mm.Max = tempNoiseVal;
			noise[tempPosi] = tempNoiseVal;
		}
	}

	//Remap values to 0-1.
	NoiseFilterer nf;
    RectangularFilterRegion mfr(Vector2i(), Vector2i(noise.GetWidth(), noise.GetHeight()));
	nf.FillRegion = &mfr;
	nf.RemapValues_OldVals = Interval(mm.Min, mm.Max, 0.001f, true, true);
    nf.RemapValues(&noise);
}