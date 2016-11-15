#include "Worley.h"

#include "NoiseFilterer.h"



Worley2D::Worley2D(DistanceCalculatorFunc distFunc, GetValueFunc noiseOutput,
                   unsigned int cellSize, Vector2f variability, int seed, Vector2i cellOffset)
    : DistFunc(distFunc), ValueGenerator(noiseOutput),
      CellSize(cellSize), Variability(variability), Seed(seed), CellOffset(cellOffset)
{

}
Worley3D::Worley3D(DistanceCalculatorFunc distFunc, GetValueFunc noiseOutput,
                   unsigned int cellSize, Vector3f variability, int seed, Vector3i cellOffset)
    : DistFunc(distFunc), ValueGenerator(noiseOutput),
      CellSize(cellSize), Variability(variability), Seed(seed), CellOffset(cellOffset)
{

}


#pragma warning(disable : 4018)

namespace WORLEY_HELPERS
{
    //Given an array and its size, tries to insert the given element
    //    while keeping the array sorted in ascending order.
    void Insert(float toInsert, float* insertInto, unsigned int maxSize)
    {
	    unsigned int i;
	    for (i = 0; i < maxSize; ++i)
	    {
		    //If this is the right spot to insert the data...
		    if (toInsert < insertInto[i])
		    {
			    //Move all the other data up.
			    for (unsigned int j = maxSize - 1; j > i; --j)
			    {
				    insertInto[j] = insertInto[j - 1];
			    }
			    //Insert the element.
			    insertInto[i] = toInsert;
			    break;
		    }
	    }
    }
}
using namespace WORLEY_HELPERS;

void Worley2D::Generate(Array2D<float>& noise) const
{
	//Get the size of a cell.
    unsigned int cSize = Mathf::Min(CellSize, noise.GetWidth(), noise.GetHeight());
    float cSizeF = (float)cSize;

	//Get the number of cells.
	Vector2u cells = Vector2u((noise.GetWidth() / cSize),
							  (noise.GetHeight() / cSize));

	Array2D<Vector2f> cellCenters(cells.x, cells.y);

	//Generate cell positions.
	FastRand fr(Seed);
    Vector2u loc;
    for (loc.y = 0; loc.y < cells.y; ++loc.y)
    {
        //Get the range of Y values available for the center of this cell.
        float startY = (float)loc.y * cSizeF;
        Interval yBreadth = Interval(startY, startY + cSizeF, 0.001f, true, true);
        yBreadth = yBreadth.Inflate(Variability.y);

        for (loc.x = 0; loc.x < cells.x; ++loc.x)
        {
            //Get the range of X values available for the center of this cell.
            float startX = loc.x * cSizeF;
            Interval xBreadth = Interval(startX, startX + cSizeF, 0.001f, true, true);
            xBreadth = xBreadth.Inflate(Variability.x);

            //Generate the center position for this cell.
            fr.Seed = Vector3i(Seed, (int)loc.x + CellOffset.x, (int)loc.y + CellOffset.y).GetHashCode();
            fr.GetRandInt();
            fr.GetRandInt();
            cellCenters[loc] = Vector2f(xBreadth.RandomInsideRange(fr),
                                        yBreadth.RandomInsideRange(fr));

            Vector2f temp = cellCenters[loc];
            temp.x = temp.x;
        }
    }


	//Get n-th-closest point for every spot on the noise grid.

    float min = std::numeric_limits<float>::max(),
          max = -std::numeric_limits<float>::max();

	DistanceValues vals;

	Vector2f tempPos;
    Vector2u tempCellLoc;
    loc = Vector2u();

    for (loc.y = 0; loc.y < noise.GetHeight(); ++loc.y)
    {
        tempPos.y = (float)loc.y;
        tempCellLoc.y = loc.y / cSize;

        for (loc.x = 0; loc.x < noise.GetWidth(); ++loc.x)
        {
            tempPos.x = (float)loc.x;
            tempCellLoc.x = loc.x / cSize;

            for (unsigned int i = 0; i < NUMB_DISTANCE_VALUES; ++i)
                vals.Values[i] = std::numeric_limits<float>::max();

            //Loop through every surrounding cell.
            for (int y2 = -1; y2 <= 1; ++y2)
            {
                for (int x2 = -1; x2 <= 1; ++x2)
                {
                    //If this cell is out of the bounds of the cell grid, wrap around.
                    Vector2i tempCell = ToV2i(tempCellLoc) + Vector2i(x2, y2),
                             tempCellWrapped = tempCell;
                    Vector2f posOffset;
                    if (tempCell.x < 0)
                    {
                        tempCellWrapped.x += cells.x;
                        posOffset.x = -(float)noise.GetWidth();
                    }
                    else if (tempCell.x >= cells.x)
                    {
                        tempCellWrapped.x -= cells.x;
                        posOffset.x = (float)noise.GetWidth();
                    }
                    if (tempCell.y < 0)
                    {
                        tempCellWrapped.y += cells.y;
                        posOffset.y = -(float)noise.GetHeight();
                    }
                    else if (tempCell.y >= cells.y)
                    {
                        tempCellWrapped.y -= cells.y;
                        posOffset.y = (float)noise.GetHeight();
                    }
                    Vector2u tempCellWrappedU = ToV2u(tempCellWrapped);

                    //Get the distance to the cell's center and insert that distance into the sorted array.
                    float tempDist = DistFunc(tempPos, cellCenters[tempCellWrappedU] + posOffset);
                    Insert(tempDist, vals.Values, NUMB_DISTANCE_VALUES);
                }
            }

            float noiseVal = ValueGenerator(vals);
            min = Mathf::Min(min, noiseVal);
            max = Mathf::Max(max, noiseVal);
            noise[loc] = noiseVal;
        }
    }

	//Remap values to 0-1.
	NoiseFilterer2D nf;
    MaxFilterRegion mfr;
	nf.FillRegion = &mfr;
	nf.RemapValues_OldVals = Interval(min, max, 0.000001f, true, true);
    nf.RemapValues_NewVals = Interval(0.0f, 1.0f, 0.000001f, true, true);
    nf.RemapValues(&noise);
}
void Worley3D::Generate(Array3D<float>& noise) const
{
	//Get the size of a cell.
    unsigned int cSize = CellSize;//Mathf::Min(CellSize, noise.GetWidth(),
                                    //Mathf::Min(noise.GetHeight(), noise.GetDepth()));
    float cSizeF = (float)cSize;

	//Get the number of cells.
	Vector3u cells = Vector3u(Mathf::Max((unsigned int)1, noise.GetWidth() / cSize),
							  Mathf::Max((unsigned int)1, noise.GetHeight() / cSize),
                              Mathf::Max((unsigned int)1, noise.GetDepth() / cSize));

	Array3D<Vector3f> cellCenters(cells.x, cells.y, cells.z);

	//Generate cell positions.
	FastRand fr(Seed);
    Vector3u loc;
    for (loc.z = 0; loc.z < cells.z; ++loc.z)
    {
        //Get the range of Z values available for the center of this cell.
        float startZ = loc.z * cSizeF;
        Interval zBreadth = Interval(startZ, startZ + cSizeF, 0.001f, true, true);
        zBreadth.Inflate(Variability.z);

        for (loc.y = 0; loc.y < cells.y; ++loc.y)
        {
            //Get the range of Y values available for the center of this cell.
            float startY = loc.y * cSizeF;
            Interval yBreadth = Interval(startY, startY + cSizeF, 0.001f, true, true);
            yBreadth.Inflate(Variability.y);

            for (loc.x = 0; loc.x < cells.x; ++loc.x)
            {
                //Get the range of X values available for the center of this cell.
                float startX = loc.x * cSizeF;
                Interval xBreadth = Interval(startX, startX + cSizeF, 0.001f, true, true);
                xBreadth.Inflate(Variability.x);

                //Generate the center position for this cell.
                fr.Seed = Vector4i(Seed,
                                   (int)loc.x + CellOffset.x,
                                   (int)loc.y + CellOffset.y,
                                   (int)loc.z + CellOffset.z).GetHashCode();
                fr.GetRandInt();
                fr.GetRandInt();
                cellCenters[loc] = Vector3f(xBreadth.RandomInsideRange(fr),
                                            yBreadth.RandomInsideRange(fr),
                                            zBreadth.RandomInsideRange(fr));
            }
        }
    }


	//Get n-th-closest point for every spot on the noise grid.

    float min = std::numeric_limits<float>::max(),
          max = -std::numeric_limits<float>::max();

	DistanceValues vals;
	Vector3f tempPos;
    Vector3u tempCellLoc;
    loc = Vector3u();

    for (loc.z = 0; loc.z < noise.GetDepth(); ++loc.z)
    {
        tempPos.z = (float)loc.z;
        tempCellLoc.z = loc.z / cSize;

        for (loc.y = 0; loc.y < noise.GetHeight(); ++loc.y)
        {
            tempPos.y = (float)loc.y;
            tempCellLoc.y = loc.y / cSize;

            for (loc.x = 0; loc.x < noise.GetWidth(); ++loc.x)
            {
                tempPos.x = (float)loc.x;
                tempCellLoc.x = loc.x / cSize;

                for (unsigned int i = 0; i < NUMB_DISTANCE_VALUES; ++i)
                    vals.Values[i] = std::numeric_limits<float>::max();

                //Loop through every surrounding cell.
                for (int z2 = -1; z2 <= 1; ++z2)
                {
                    for (int y2 = -1; y2 <= 1; ++y2)
                    {
                        for (int x2 = -1; x2 <= 1; ++x2)
                        {
                            //If this cell is out of the bounds of the cell grid, wrap around.
                            Vector3i tempCell = ToV3i(tempCellLoc) + Vector3i(x2, y2, z2),
                                     tempCellWrapped = tempCell;
                            Vector3f posOffset;
                            if (tempCell.x < 0)
                            {
                                tempCellWrapped.x += cells.x;
                                posOffset.x = -(float)noise.GetWidth();
                            }
                            else if (tempCell.x >= cells.x)
                            {
                                tempCellWrapped.x -= cells.x;
                                posOffset.x = (float)noise.GetWidth();
                            }
                            if (tempCell.y < 0)
                            {
                                tempCellWrapped.y += cells.y;
                                posOffset.y = -(float)noise.GetHeight();
                            }
                            else if (tempCell.y >= cells.y)
                            {
                                tempCellWrapped.y -= cells.y;
                                posOffset.y = (float)noise.GetHeight();
                            }
                            if (tempCell.z < 0)
                            {
                                tempCellWrapped.z += cells.z;
                                posOffset.z = -(float)noise.GetDepth();
                            }
                            else if (tempCell.z >= cells.z)
                            {
                                tempCellWrapped.z -= cells.z;
                                posOffset.z = (float)noise.GetDepth();
                            }
                            Vector3u tempCellWrappedU = ToV3u(tempCellWrapped);

                            //Get the distance to the cell's center
                            //    and insert that distance into the sorted array.
                            float tempDist = DistFunc(tempPos, cellCenters[tempCellWrappedU] + posOffset);
                            Insert(tempDist, vals.Values, NUMB_DISTANCE_VALUES);
                        }
                    }
                }

                float noiseVal = ValueGenerator(vals);
                min = Mathf::Min(min, noiseVal);
                max = Mathf::Max(max, noiseVal);
                noise[loc] = noiseVal;
            }
        }
    }

	//Remap values to 0-1.
	NoiseFilterer3D nf;
    MaxFilterVolume mfv;
	nf.FillVolume = &mfv;
	nf.RemapValues_OldVals = Interval(min, max, 0.000001f, true, true);
    nf.RemapValues(&noise);
}


#pragma warning(default : 4018)

#undef PTR