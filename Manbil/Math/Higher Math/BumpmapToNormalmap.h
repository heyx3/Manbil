#pragma once

#include "../../Math/LowerMath.hpp"

//Calculates a normal map for the given bumpmap.
class BumpmapToNormalmap
{
public:
    
    //If "normalizeRange" is true, each normal's X, Y, and Z values will be remapped
    //    from [-1, 1] to [0, 1] for packing into a texture.
    //Resizes "outNormals" if they aren't the same size as "heightmap" already.
    static void Convert(const Array2D<float>& heightmap, float heightScale,
                        bool normalizeRange, Array2D<Vector3f>& outNormals);
};