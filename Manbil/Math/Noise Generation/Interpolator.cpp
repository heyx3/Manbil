#include "Interpolator.h"

void Interpolator::ComputeTempSmoothedNoise(Noise2D & tempSmoothedNoise) const
{
	Fake2DArray<float> nti = Fake2DArray<float>(InterpolateWidth, InterpolateHeight);
	NoiseToInterpolate->Generate(nti);

	float val;
    Vector2i loc;
	int w = tempSmoothedNoise.GetWidth(), h = tempSmoothedNoise.GetHeight();
	int x1, x_1, y1, y_1;

    for (loc.y = 0; loc.y < h; ++loc.y)
    {
        y1 = loc.y + 1;
        if (y1 >= h) y1 -= h;
        y_1 = loc.y - 1;
        if (y_1 < 0) y_1 += h;

        for (loc.x = 0; loc.x < w; ++loc.x)
        {
            x1 = loc.x + 1;
            if (x1 >= w) x1 -= w;
            x_1 = loc.x - 1;
            if (x_1 < 0) x_1 += w;

			val = ((nti[Vector2i(x_1, y_1)] +
					nti[Vector2i(x1, y_1)] +
					nti[Vector2i(x_1, y1)] +
					nti[Vector2i(x1, y1)]) * 0.0625f) +
				  ((nti[Vector2i(x_1, loc.y)] +
					nti[Vector2i(x1, loc.y)] +
                    nti[Vector2i(loc.x, y_1)] +
                    nti[Vector2i(loc.x, y1)]) * 0.125f) +
				  (nti[loc] * 0.25f);

            tempSmoothedNoise[loc] = val;
		}
	}
}

float Interpolator::GetInterpolatedNoise(Vector2f scale, Noise2D & tempSmoothedNoise) const
{
	int ix = (int)scale.x,
		iy = (int)scale.y;

	float xFrac = scale.x - ix,
		  yFrac = scale.y - iy;

	int x1 = ix + 1, y1 = iy + 1;
	if (x1 >= tempSmoothedNoise.GetWidth()) x1 -= tempSmoothedNoise.GetWidth();
	if (y1 >= tempSmoothedNoise.GetHeight()) y1 -= tempSmoothedNoise.GetHeight();

	float c1 = tempSmoothedNoise[tempSmoothedNoise.Wrap(Vector2i(ix, iy))],
          c2 = tempSmoothedNoise[tempSmoothedNoise.Wrap(Vector2i(x1, iy))],
          c3 = tempSmoothedNoise[tempSmoothedNoise.Wrap(Vector2i(ix, y1))],
          c4 = tempSmoothedNoise[tempSmoothedNoise.Wrap(Vector2i(x1, y1))];

	float int1 = BasicMath::Lerp(c1, c2, xFrac),
		  int2 = BasicMath::Lerp(c3, c4, xFrac);

	return BasicMath::Lerp(int1, int2, yFrac);
}

void Interpolator::Generate(Fake2DArray<float> & outN) const
{
	Noise2D tempSmoothedNoise(InterpolateWidth, InterpolateHeight);
	ComputeTempSmoothedNoise(tempSmoothedNoise);

	int w = outN.GetWidth(), h = outN.GetHeight();
	float invScale = 1.0f / InterpolateScale;
	Vector2i loc;
	Vector2f scaleLoc;

    for (loc.y = 0; loc.y < h; ++loc.y)
    {
        scaleLoc.y = (float)loc.y * invScale;

        for (loc.x = 0; loc.x < w; ++loc.x)
        {
            scaleLoc.x = (float)loc.x * invScale;

            outN[loc] = GetInterpolatedNoise(scaleLoc, tempSmoothedNoise);
        }
    }
}