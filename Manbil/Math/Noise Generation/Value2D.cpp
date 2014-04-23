#include "Value2D.h"


void Value2D::Generate(Fake2DArray<float> & outValues) const
{
	int width = outValues.GetWidth();
	int height = outValues.GetHeight();
	FastRand fr;

	Vector2i loc;
	for (loc.x = 0; loc.x < width; ++loc.x)
	{
		for (loc.y = 0; loc.y < height; ++loc.y)
		{
			fr.Seed = Vector3i(loc.x, loc.y, RandSeed).GetHashCode();
			outValues[loc] = fr.GetZeroToOne();
		}
	}
}