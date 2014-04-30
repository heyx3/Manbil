#include "Worley.h"

#include <vector>
#include <queue>
#include "NoiseFilterer.h"

#include <memory>
#define PTR(X) (std::unique_ptr<X>)


const float BadWADDistance = -1.0f;
//Stores a generated point in a cell and the distance to that point.
struct WorleyAlgorithmData2D
{
    Vector2f Pos;
    float Distance;
    WorleyAlgorithmData2D(Vector2f pos = Vector2f(), float distance = BadWADDistance) : Pos(pos), Distance(distance) { }
};
typedef WorleyAlgorithmData2D WAD2;
bool IsGoodData(WAD2 & dat) { return dat.Distance != BadWADDistance; }


void Insert(WorleyAlgorithmData2D* toInsertInto, WorleyAlgorithmData2D & toInsert)
{
	//Only need to use the closest 5 elements.
	const int maxSize = Worley2D::NUMB_DISTANCE_VALUES;

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

void Worley2D::Generate(Array2D<float> & noise) const
{
    unsigned int pointsPerCellRange = MaxPointsPerCell - MinPointsPerCell;


	//Get the size of a cell.
	int cSize = BasicMath::Min<int>(CellSize, noise.GetWidth(), noise.GetHeight());

	//Get the number of cells (use one extra row/column of cells behind the noise array).
	Vector2i cells = Vector2i((noise.GetWidth() / cSize),
							  (noise.GetHeight() / cSize));

	//Initialize cell contents.
	int i;
    Vector2i loc;
	struct Cell { std::vector<Vector2f> pointsInCell; };
	Array2D<Cell> cellContents(cells.x, cells.y);

	//Generate cell contents.
	FastRand fr(Seed);
	Interval tempIntX, tempIntY;
	std::vector<Vector2f> * cellTemp;
	int pointsInCell;

    for (loc.y = 0; loc.y < cells.y; ++loc.y)
    {
        tempIntY = Interval(loc.y * cSize, (loc.y * cSize) + cSize, 0.001f, true, true);

        for (loc.x = 0; loc.x < cells.x; ++loc.x)
        {
            tempIntX = Interval(loc.x * cSize, (loc.x * cSize) + cSize, 0.001f, true, true);

            cellTemp = &cellContents[loc].pointsInCell;

            fr.Seed = Vector3i(Seed, loc.x, loc.y).GetHashCode();

            //Generate some randomized number of points in this cell.
            pointsInCell = (fr.GetRandInt() % pointsPerCellRange) + MinPointsPerCell;
            //pointsInCell = BasicMath::RoundToInt(PointsPerCell.RandomInsideRange(fr));
            for (int i = 0; i < pointsInCell; ++i)
            {
                cellTemp->insert(cellTemp->end(), Vector2f(tempIntX.RandomInsideRange(fr), tempIntY.RandomInsideRange(fr)));
            }
        }
    }


	//Get n-th-closest point for every noise element.

	NoiseAnalysis2D::MinMax mm(999999.0f, -999999.0f);
	DistanceValues vals;
	WAD2 valsData[NUMB_DISTANCE_VALUES];
	WAD2 tempWAD;
	int x2, y2;
	Vector2f tempPos;
    Vector2i tempCellLoc;
	float tempNoiseVal;
	//Go through every noise element.
    for (loc.y = 0; loc.y < noise.GetHeight(); ++loc.y)
    {
        tempPos.y = (float)loc.y;
        tempCellLoc.y = loc.y / CellSize;

        for (loc.x = 0; loc.x < noise.GetWidth(); ++loc.x)
        {
            tempPos.x = (float)loc.x;
            tempCellLoc.x = loc.x / CellSize;

            for (i = 0; i < NUMB_DISTANCE_VALUES; ++i)
            {
                valsData[i] = WAD2();
            }

            //Loop through every surrounding cell.
            for (y2 = -1; y2 <= 1; ++y2)
            {
                for (x2 = -1; x2 <= 1; ++x2)
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
            noise[loc] = tempNoiseVal;
        }
    }

	//Remap values to 0-1.
	NoiseFilterer2D nf;
    RectangularFilterRegion mfr(Vector2i(), Vector2i(noise.GetWidth(), noise.GetHeight()));
	nf.FillRegion = &mfr;
	nf.RemapValues_OldVals = Interval(mm.Min, mm.Max, 0.001f, true, true);
    nf.RemapValues(&noise);
}








//Stores a generated point in a cell and the distance to that point.
struct WorleyAlgorithmData3D
{
    Vector3f Pos;
    float Distance;
    WorleyAlgorithmData3D(Vector3f pos = Vector3f(), float distance = BadWADDistance) : Pos(pos), Distance(distance) { }
};
typedef WorleyAlgorithmData3D WAD3;
bool IsGoodData(WAD3 & dat) { return dat.Distance != BadWADDistance; }


void Insert(WorleyAlgorithmData3D* toInsertInto, WorleyAlgorithmData3D & toInsert)
{
    //Only need to use the closest 5 elements.
    const int maxSize = Worley3D::NUMB_DISTANCE_VALUES;

    int i;
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
        toInsertInto[i].Distance = toInsert.Distance;
    }
}

void Worley3D::Generate(Array3D<float> & noise) const
{
    unsigned int pointsPerCellRange = MaxPointsPerCell - MinPointsPerCell;

    //Get the size of a cell.
    int cSize = BasicMath::Min<int>(CellSize, noise.GetWidth(), BasicMath::Min(noise.GetHeight(), noise.GetDepth()));

    //Get the number of cells (use one extra row/column of cells behind the noise array).
    Vector3i cells = Vector3i((noise.GetWidth() / cSize),
                              (noise.GetHeight() / cSize),
                              (noise.GetDepth() / cSize));

    //Initialize cell contents.
    int i;
    Vector3i loc;
    struct Cell { std::vector<Vector3f> pointsInCell; };
    Array3D<Cell> cellContents(cells.x, cells.y, cells.z);

    //Generate cell contents.
    FastRand fr(Seed);
    Interval tempIntX, tempIntY, tempIntZ;
    std::vector<Vector3f> * cellTemp;
    int pointsInCell;

    for (loc.z = 0; loc.z < cells.z; ++loc.z)
    {
        tempIntZ = Interval(loc.z * cSize, (loc.z * cSize) + cSize, 0.001f, true, true);

        for (loc.y = 0; loc.y < cells.y; ++loc.y)
        {
            tempIntY = Interval(loc.y * cSize, (loc.y * cSize) + cSize, 0.001f, true, true);

            for (loc.x = 0; loc.x < cells.x; ++loc.x)
            {
                tempIntX = Interval(loc.x * cSize, (loc.x * cSize) + cSize, 0.001f, true, true);

                cellTemp = &cellContents[loc].pointsInCell;

                fr.Seed = Vector4i(loc.z, Seed, loc.x, loc.y).GetHashCode();

                //Generate some randomized number of points in this cell.
                pointsInCell = (fr.GetRandInt() % pointsPerCellRange) + MinPointsPerCell;
                //pointsInCell = BasicMath::RoundToInt(PointsPerCell.RandomInsideRange(fr));
                for (int i = 0; i < pointsInCell; ++i)
                {
                    cellTemp->insert(cellTemp->end(), Vector3f(tempIntX.RandomInsideRange(fr), tempIntY.RandomInsideRange(fr), tempIntZ.RandomInsideRange(fr)));
                }
            }
        }
    }


    //Get n-th-closest point for every noise element.

    NoiseAnalysis3D::MinMax mm(999999.0f, -999999.0f);
    DistanceValues vals;
    WAD3 valsData[NUMB_DISTANCE_VALUES];
    WAD3 tempWAD;
    int x2, y2, z2;
    Vector3f tempPos;
    Vector3i tempCellLoc;
    float tempNoiseVal;
    //Go through every noise element.
    for (loc.z = 0; loc.z < noise.GetDepth(); ++loc.z)
    {
        tempPos.z = (float)loc.z;
        tempCellLoc.z = loc.z / CellSize;

        for (loc.y = 0; loc.y < noise.GetHeight(); ++loc.y)
        {
            tempPos.y = (float)loc.y;
            tempCellLoc.y = loc.y / CellSize;

            for (loc.x = 0; loc.x < noise.GetWidth(); ++loc.x)
            {
                tempPos.x = (float)loc.x;
                tempCellLoc.x = loc.x / CellSize;

                for (i = 0; i < NUMB_DISTANCE_VALUES; ++i)
                {
                    valsData[i] = WAD3();
                }

                //Loop through every surrounding cell.
                for (z2 = -1; z2 <= 1; ++z2)
                {
                    for (y2 = -1; y2 <= 1; ++y2)
                    {
                        for (x2 = -1; x2 <= 1; ++x2)
                        {
                            //If the cell area is out of bounds, either wrap it or stop this iteration.
                            Vector3i tempLoci(tempCellLoc.x + x2, tempCellLoc.y + y2, tempCellLoc.z + z2);
                            Vector3i newTempLoci = cellContents.Wrap(tempLoci);
                            bool wrappedLeftX = (tempLoci.x < newTempLoci.x),
                                 wrappedLeftY = (tempLoci.y < newTempLoci.y),
                                 wrappedLeftZ = (tempLoci.z < newTempLoci.z),
                                 wrappedRightX = (tempLoci.x > newTempLoci.x),
                                 wrappedRightY = (tempLoci.y > newTempLoci.y),
                                 wrappedRightZ = (tempLoci.z > newTempLoci.z);

                            //Go through every point in the cell.
                            cellTemp = &cellContents[newTempLoci].pointsInCell;

                            for (i = 0; i < (int)cellTemp->size(); ++i)
                            {
                                //Put it into the ordered list of distances.
                                tempWAD.Pos = (*cellTemp)[i];
                                if (wrappedLeftX) tempWAD.Pos.x -= noise.GetWidth();
                                if (wrappedLeftY) tempWAD.Pos.y -= noise.GetHeight();
                                if (wrappedLeftZ) tempWAD.Pos.z -= noise.GetDepth();
                                if (wrappedRightX) tempWAD.Pos.x += noise.GetWidth();
                                if (wrappedRightY) tempWAD.Pos.y += noise.GetHeight();
                                if (wrappedRightZ) tempWAD.Pos.z += noise.GetDepth();

                                tempWAD.Distance = DistFunc(tempPos, tempWAD.Pos);

                                if (!IsGoodData(valsData[NUMB_DISTANCE_VALUES - 1]) ||
                                    tempWAD.Distance < valsData[NUMB_DISTANCE_VALUES - 1].Distance)
                                {
                                    Insert(valsData, tempWAD);
                                }
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
                noise[loc] = tempNoiseVal;
            }
        }
    }

    //Remap values to 0-1.
    NoiseFilterer3D nf;
    MaxFilterVolume mfr;
    nf.FillVolume = &mfr;
    nf.RemapValues_OldVals = Interval(mm.Min, mm.Max, 0.001f, true, true);
    nf.RemapValues(&noise);
}