#include "WhiteNoise.h"


void WhiteNoise2D::Generate(Fake2DArray<float> & outValues) const
{
    int width = outValues.GetWidth();
    int height = outValues.GetHeight();
    FastRand fr;

    Vector2i loc;
    for (loc.y = 0; loc.y < height; ++loc.y)
    {
        for (loc.x = 0; loc.x < width; ++loc.x)
        {
            fr.Seed = Vector3i(loc, RandSeed).GetHashCode();
            outValues[loc] = fr.GetZeroToOne();
        }
    }
}

void WhiteNoise3D::Generate(Fake3DArray<float> & outValues) const
{
    int width = outValues.GetWidth();
    int height = outValues.GetHeight();
    int depth = outValues.GetDepth();
    FastRand fr;

    Vector3i loc;
    for (loc.z = 0; loc.z < depth; ++loc.z)
    {
        for (loc.y = 0; loc.y < height; ++loc.y)
        {
            for (loc.x = 0; loc.x < width; ++loc.x)
            {
                fr.Seed = Vector4i(loc, RandSeed).GetHashCode();
                outValues[loc] = fr.GetZeroToOne();
            }
        }
    }
}