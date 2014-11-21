#include "BumpmapToNormalmap.h"

void BumpmapToNormalmap::Convert(const Array2D<float> & heightmap, float heightScale, bool normalizeRange, Array2D<Vector3f> & normals)
{
    //TODO: Fix.

    Vector3f sum, tempSum, base, first, second;
    for (Vector2u loc(0, 1); loc.y < heightmap.GetHeight(); ++loc.y)
    {
        base.y = (float)loc.y;

        for (loc.x = 1; loc.x < heightmap.GetWidth(); ++loc.x)
        {
            sum = Vector3f();

            base.x = (float)loc.x;
            base.z = heightScale * heightmap[loc];

            first = Vector3f(base.x - 1.0f, base.y, heightScale * heightmap[heightmap.Wrap(loc.LessX())]);
            second = Vector3f(base.x, base.y - 1.0f, heightScale * heightmap[heightmap.Wrap(loc.LessY())]);

            tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
            if (tempSum.z < 0.0f)
                tempSum = -tempSum;

            sum += tempSum;


            first = Vector3f(base.x + 1.0f, base.y, heightScale * heightmap[heightmap.Wrap(loc.MoreX())]);
            second = Vector3f(base.x, base.y + 1.0f, heightScale * heightmap[heightmap.Wrap(loc.MoreY())]);

            tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
            if (tempSum.z < 0.0f)
                tempSum = -tempSum;

            sum += tempSum;


            sum = sum.Normalized();

            if (normalizeRange) sum = (sum * 0.5f) + Vector3f(0.5f, 0.5f, 0.5f);
            normals[loc] = sum;
        }
    }
}