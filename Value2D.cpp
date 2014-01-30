#include "Value2D.h"

#include "Vectors.h"
#include "FastRand.h"
#include "BasicMath.h"

void Value2D::Generate(Fake2DArray<float> & outValues) const
{
	int x, y;

	int width = outValues.GetWidth();
	int height = outValues.GetHeight();
	FastRand fr;

	Vector2i loc;
	for (x = 0; x < width; ++x)
	{
		loc.x = x;
		for (y = 0; y < height; ++y)
		{
			loc.y = y;
			fr.Seed = Vector3i(x, y, RandSeed).GetHashCode();
			outValues[loc] = fr.GetZeroToOne();
		}
	}
}