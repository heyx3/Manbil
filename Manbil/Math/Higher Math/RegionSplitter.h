#pragma once

#include "../LowerMath.hpp"

#include <memory>
#include <vector>
#include <assert.h>


template<typename Data>
//Takes data on some kind of 2D grid and splits it up into discrete space-filling rectangular regions.
class RegionSplitter
{
	//The output -- the data, split into rectangular regions inside the grid.
	typedef Array2D<std::vector<Data*>> SplitRegions;
	//Takes a data object and outputs its position along the grid.
	typedef Vector2f (*DatToGridPos)(const Data * const dat);

	//Splits up the data. The grid width/height must both be multiples of the region size!
	static std::shared_ptr<SplitRegions> Generate(std::vector<Data> locations, DatToGridPos toGridPos, Vector2i gridSize, int regionSize)
	{
		const int rWidth = gridSize.x / regionSize,
				  rHeight = gridSize.y / regionSize;
		assert((rWidth * regionSize == gridSize.x) && (rHeight * regionSize == gridSize.y));


		std::shared_ptr<SplitRegions> splitted(new SplitRegions(rWidth, rHeight));


		//Go through every Data instance.

		Vector2f loc;
		Vector2i locI;
		std::vector<Data> * tempRegion;
		Data * tempDat;
		for (unsigned int i = 0; i < locations.size(); ++i)
		{
			tempDat = &locations[i];

			//Convert the Data's position into the region coordinate.
			loc = toGridPos(tempDat);
			locI.x = (int)loc.x;
			locI.y = (int)loc.y;
			locI /= regionSize;

			//Insert the Data instance into the correct region.
			tempRegion = &(*splitted)[locI];
			tempRegion->insert(tempRegion->end(), tempDat);
		}


		return splitted;
	}
};