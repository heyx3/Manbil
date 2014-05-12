#include "BumpmapToNormalmap.h"

void BumpmapToNormalmap::Convert(const Array2D<float> & heightmap, float heightScale, bool normalizeRange, Array2D<Vector3f> & normals)
{
    Vector3f sum, tempSum, base, first, second;
    for (Vector2i loc; loc.y < heightmap.GetHeight(); ++loc.y)
    {
        base.y = loc.y;
        for (loc.x = 0; loc.x < heightmap.GetWidth(); ++loc.x)
        {
            sum = Vector3f();

            base.x = loc.x;
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