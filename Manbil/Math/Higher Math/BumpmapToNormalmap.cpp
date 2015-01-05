#include "BumpmapToNormalmap.h"

void BumpmapToNormalmap::Convert(const Array2D<float>& heightmap, float heightScale,
                                 bool normalizeRange, Array2D<Vector3f>& normals)
{
    //Go through every point on the bumpmap and find its normal.
    Vector3f base, first, second;
    for (Vector2u loc(0, 0); loc.y < heightmap.GetHeight(); ++loc.y)
    {
        base.y = (float)loc.y;

        for (loc.x = 0; loc.x < heightmap.GetWidth(); ++loc.x)
        {
            base.x = (float)loc.x;

            base.z = heightScale * heightmap[loc];


            //Get the two normals that define this point's normal and average them together.

            Vector3f sum;
            Vector2i locI = ToV2i(loc);

            first = Vector3f(base.x - 1.0f, base.y,
                             heightScale * heightmap[ToV2u(heightmap.Wrap(locI.LessX()))]);
            second = Vector3f(base.x, base.y - 1.0f,
                              heightScale * heightmap[ToV2u(heightmap.Wrap(locI.LessY()))]);

            Vector3f tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
            if (tempSum.z < 0.0f)
                tempSum = -tempSum;

            sum += tempSum;


            first = Vector3f(base.x + 1.0f, base.y,
                             heightScale * heightmap[ToV2u(heightmap.Wrap(locI.MoreX()))]);
            second = Vector3f(base.x, base.y + 1.0f,
                              heightScale * heightmap[ToV2u(heightmap.Wrap(locI.MoreY()))]);

            tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
            if (tempSum.z < 0.0f)
                tempSum = -tempSum;

            sum += tempSum;

            sum.Normalize();

            //Pack the normal into the range [0, 1] if necessary.
            if (normalizeRange)
            {
                sum = (sum * 0.5f) + Vector3f(0.5f, 0.5f, 0.5f);
            }

            normals[loc] = sum;
        }
    }
}