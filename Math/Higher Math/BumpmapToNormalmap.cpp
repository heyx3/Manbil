#include "BumpmapToNormalmap.h"

void BumpmapToNormalmap::Convert(const Fake2DArray<float> & heightmap, Fake2DArray<Vector3f> & normals)
{
    Vector3f sum, tempSum, base, first, second;
    for (int x = 0; x < heightmap.GetWidth(); ++x)
    {
        for (int y = 0; y < heightmap.GetHeight(); ++y)
        {
            sum = Vector3f();

            base = Vector3f(x, y, heightmap[Vector2i(x, y)]);

            if (x > 0 && y > 0)
            {
                first = Vector3f(x - 1.0f, y, heightmap[Vector2i(x - 1, y)]);
                second = Vector3f(x, y - 1.0f, heightmap[Vector2i(x, y - 1)]);

                tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
                if (tempSum.z < 0.0f)
                    tempSum = -tempSum;

                sum += tempSum;
            }
            if (x < heightmap.GetWidth() - 1 && y < heightmap.GetHeight() - 1)
            {
                first = Vector3f(x + 1.0f, y, heightmap[Vector2i(x + 1, y)]);
                second = Vector3f(x, y - 1.0f, heightmap[Vector2i(x, y + 1)]);

                tempSum = (first - base).Normalized().Cross((second - base).Normalized()).Normalized();
                if (tempSum.z < 0.0f)
                    tempSum = -tempSum;

                sum += tempSum;
            }

            normals[Vector2i(x, y)] = sum.Normalized();
        }
    }
}