#include "Interpolator.h"

void Interpolator2D::ComputeTempSmoothedNoise(Noise2D & tempSmoothedNoise) const
{
	Array2D<float> nti = Array2D<float>(InterpolateWidth, InterpolateHeight);
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

float Interpolator2D::GetInterpolatedNoise(Vector2f scale, Noise2D & tempSmoothedNoise) const
{
	
}

void Interpolator2D::Generate(Array2D<float> & outN) const
{
	Noise2D tempSmoothedNoise(InterpolateWidth, InterpolateHeight);
	ComputeTempSmoothedNoise(tempSmoothedNoise);

	int w = outN.GetWidth(), h = outN.GetHeight();
	float invScale = 1.0f / InterpolateScale;
	Vector2i loc;
	Vector2f scaleLoc, fracLoc;
    Vector2i scaleLocI, scaleLocI1;

    for (loc.y = 0; loc.y < h; ++loc.y)
    {
        scaleLoc.y = (float)loc.y * invScale;
        scaleLocI.y = (int)scaleLoc.y;
        scaleLocI1.y = (scaleLocI.y + 1) % tempSmoothedNoise.GetWidth();
        fracLoc.y = scaleLoc.y - loc.y;

        for (loc.x = 0; loc.x < w; ++loc.x)
        {
            scaleLoc.x = (float)loc.x * invScale;
            scaleLocI.x = (int)scaleLoc.x;
            scaleLocI1.x = (scaleLocI.x + 1) % tempSmoothedNoise.GetHeight();
            fracLoc.x = scaleLoc.x - loc.x;

            float c1 = tempSmoothedNoise[tempSmoothedNoise.Wrap(scaleLocI)],
                  c2 = tempSmoothedNoise[tempSmoothedNoise.Wrap(Vector2i(scaleLocI1.x, scaleLocI.y))],
                  c3 = tempSmoothedNoise[tempSmoothedNoise.Wrap(Vector2i(scaleLocI.x, scaleLocI1.y))],
                  c4 = tempSmoothedNoise[tempSmoothedNoise.Wrap(Vector2i(scaleLocI1.x, scaleLocI1.y))];

            outN[loc] = BasicMath::Lerp(BasicMath::Lerp(c1, c2, fracLoc.x),
                                        BasicMath::Lerp(c3, c4, fracLoc.x),
                                        fracLoc.y);

            outN[loc] = GetInterpolatedNoise(scaleLoc, tempSmoothedNoise);
        }
    }
}