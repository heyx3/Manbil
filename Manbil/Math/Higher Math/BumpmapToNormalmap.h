#pragma once

#include "../../Math/LowerMath.hpp"

//Calculates a normal map for the given bumpmap.
class BumpmapToNormalmap
{
public:
    
    static void Convert(const Fake2DArray<float> & heightmap, float heightScale, Fake2DArray<Vector3f> & outNormals);
};