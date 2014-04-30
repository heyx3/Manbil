#pragma once

#include "../../Math/LowerMath.hpp"

//Calculates a normal map for the given bumpmap.
class BumpmapToNormalmap
{
public:
    
    static void Convert(const Array2D<float> & heightmap, float heightScale, Array2D<Vector3f> & outNormals);
};