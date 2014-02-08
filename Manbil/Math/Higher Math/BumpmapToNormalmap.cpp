#include "BumpmapToNormalmap.h"

void BumpmapToNormalmap::Convert(const Fake2DArray<float> & heightmap, float heightScale, Fake2DArray<Vector3f> & normals)
{
    Vector3f sum, tempSum, base, first, second;
    for (int x = 0; x < heightmap.GetWidth(); ++x)
    {
        for (int y = 0; y < heightmap.GetHeight(); ++y)
        {
            sum = Vector3f();

            base = Vector3f(x, y, heightScale * heightmap[Vector2i(x, y)]);


            first = Vector3f(x - 1.0f, y, heightScale * heightmap[heightmap.Wrap(Vector2i(x - 1, y))]);
            second = Vector3f(x, y - 1.0f, heightScale * heightmap[heightmap.Wrap(Vector2i(x, y - 1))]);

            tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
            if (tempSum.z < 0.0f)
                tempSum = -tempSum;

            sum += tempSum;


            first = Vector3f(x + 1.0f, y, heightScale * heightmap[heightmap.Wrap(Vector2i(x + 1, y))]);
            second = Vector3f(x, y + 1.0f, heightScale * heightmap[heightmap.Wrap(Vector2i(x, y + 1))]);

            tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
            if (tempSum.z < 0.0f)
                tempSum = -tempSum;

            sum += tempSum;


            sum = sum.Normalized();
            //Normal components are in the range {-1, 1}, but color components are in the range {0, 1}.
            sum.x = 0.5f + (0.5f * sum.x);
            sum.y = 0.5f + (0.5f * sum.y);

            normals[Vector2i(x, y)] = sum;
        }
    }
}