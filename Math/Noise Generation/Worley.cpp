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
	Vector2i cells = Vector2i((noise.GetWidth() / cSize) + 2,
							  (noise.GetHeight() / cSize) + 2);
	if ((noise.GetWidth() / cSize) < ((float)noise.GetWidth() / (float)cSize))
	{
		cells += Vector2i(1, 1);
	}

	//Initialize cell contents.
	int i, x, y;
	struct Cell { std::vector<Vector2f> pointsInCell; };
	Fake2DArray<Cell> cellContents(cells.x, cells.y);
	//std::vector<Vector2f> ** cellContents = new std::vector<Vector2f>*[cells.x+1];
	//std::vector<Vector2f>::iterator it;
	//for (i = 0; i < cells.x+1; ++i)
	//{
	//	cellContents[i] = new std::vector<Vector2f>[cells.y+1];
	//}

	//Generate cell contents.
	FastRand fr(Seed);
	Interval tempIntX, tempIntY;
	Vector2i tempCellLoc;
	std::vector<Vector2f> * cellTemp;
	int pointsInCell;
	for (x = -1; x < cells.x - 1; ++x)
	{
		tempIntX = Interval(x * cSize, (x * cSize) + cSize, 0.001f, true, true);
		tempCellLoc.x = x + 1;

		for (y = -1; y < cells.y - 1; ++y)
		{
			tempIntY = Interval(y * cSize, (y * cSize) + cSize, 0.001f, true, true);
			tempCellLoc.y = y + 1;

			cellTemp = &cellContents[tempCellLoc].pointsInCell;

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
	float tempNoiseVal;
	//Go through every noise element.
	for (x = 0; x < noise.GetWidth(); ++x)
	{
		tempPos.x = (float)x;
		tempPosi.x = x;
		tempCellLoc.x = x / CellSize;
		tempCellLoc.x += 1;

		for (y = 0; y < noise.GetHeight(); ++y)
		{
			tempPos.y = (float)y;
			tempPosi.y = y;
			tempCellLoc.y = y / CellSize;
			tempCellLoc.y += 1;

			for (i = 0; i < NUMB_DISTANCE_VALUES; ++i)
			{
				valsData[i] = WAD();
			}

			//Loop through every surrounding cell.
			for (x2 = -1; x2 <= 1; ++x2)
			{
				for (y2 = -1; y2 <= 1; ++y2)
				{
					//Go through every point in the cell.
					cellTemp = &cellContents[tempCellLoc + Vector2i(x2, y2)].pointsInCell;

					for (i = 0; i < (int)cellTemp->size(); ++i)
					{
						//Put it into the ordered list of distances.
						tempWAD.Pos = (*cellTemp)[i];
						tempWAD.Distance = DistFunc(tempPos, (*cellTemp)[i]);
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


	//Clean up.
	//for (i = 0; i < cells.x; ++i)
	//{
	//	delete [] cellContents[i];
	//}
	//delete[] cellContents;


	//Remap values to 0-1.
	NoiseFilterer nf;
	RectangularFilterRegion rfr(Vector2i(), Vector2i(noise.GetWidth(), noise.GetHeight()));

	nf.FillRegion = &rfr;
	nf.RemapValues_OldVals = Interval(mm.Min, mm.Max, 0.001f, true, true);
	nf.FilterFunc = &NoiseFilterer::RemapValues;
	nf.Generate(noise);
}