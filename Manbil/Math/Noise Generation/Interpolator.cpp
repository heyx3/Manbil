#include "Interpolator.h"

void Interpolator::ComputeTempSmoothedNoise(Noise2D & tempSmoothedNoise) const
{
	Fake2DArray<float> nti = Fake2DArray<float>(InterpolateWidth, InterpolateHeight);
	NoiseToInterpolate->Generate(nti);

	float val;
	int x, y;
	int w = tempSmoothedNoise.GetWidth(), h = tempSmoothedNoise.GetHeight();
	int x1, x_1, y1, y_1;
	for (x = 0; x < w; ++x)
	{
		for (y = 0; y < h; ++y)
		{
			x1 = x + 1;
			x_1 = x - 1;
			y1 = y + 1;
			y_1 = y - 1;

			if (x_1 < 0) x_1 += w;
			if (x1 >= w) x1 -= w;
			if (y_1 < 0) y_1 += h;
			if (y1 >= h) y1 -= h;
			

			val = ((nti[Vector2i(x_1, y_1)] +
					nti[Vector2i(x1, y_1)] +
					nti[Vector2i(x_1, y1)] +
					nti[Vector2i(x1, y1)]) * 0.0625f) +
				  ((nti[Vector2i(x_1, y)] +
					nti[Vector2i(x1, y)] +
					nti[Vector2i(x, y_1)] +
					nti[Vector2i(x, y1)]) * 0.125f) +
				  (nti[Vector2i(x, y)] * 0.25f);

			tempSmoothedNoise[Vector2i(x, y)] = val;
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

	int x, y;
	int w = outN.GetWidth(), h = outN.GetHeight();
	float invScale = 1.0f / InterpolateScale;
	Vector2i loc;
	Vector2f scaleLoc;

	for (x = 0; x < w; ++x)
	{
		scaleLoc.x = x * invScale;
		loc.x = x;

		for (y = 0; y < h; ++y)
		{
			scaleLoc.y = y * invScale;
			loc.y = y;

			outN[loc] = GetInterpolatedNoise(scaleLoc, tempSmoothedNoise);
		}
	}
}