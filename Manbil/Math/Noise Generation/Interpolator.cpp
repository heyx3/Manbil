#include "Interpolator.h"

#include <assert.h>



void Interpolator2D::Generate(Array2D<float> & outN) const
{
    assert(InterpolateScale > 0.0f);

    Vector2i toInterpSize((int)ceilf(outN.GetWidth() / InterpolateScale) + 1,
                          (int)ceilf(outN.GetHeight() / InterpolateScale) + 1);
	Noise2D toInterp((unsigned int)toInterpSize.x, (unsigned int)toInterpSize.y);
    NoiseToInterpolate->Generate(toInterp);

    float(*smoothStepper)(float inVal);
    switch (SmoothAmount)
    {
        case Smoothness::I2S_LINEAR:
            smoothStepper = [](float inVal) { return inVal; };
            break;
        case Smoothness::I2S_CUBIC:
            smoothStepper = BasicMath::Smooth;
            break;
        case Smoothness::I2S_QUINTIC:
            smoothStepper = BasicMath::Supersmooth;
            break;

        default: assert(false);
    }


	int w = outN.GetWidth(), h = outN.GetHeight();
	float invScale = 1.0f / InterpolateScale;
	Vector2i loc;
    Vector2i srcLocMin, srcLocMax;
    Vector2f lerpVal;

    for (loc.y = 0; loc.y < h; ++loc.y)
    {
        float srcY = (float)loc.y / InterpolateScale;

        srcLocMin.y = (int)srcY;
        srcLocMax.y = srcLocMin.y + 1;
        lerpVal.y = smoothStepper(srcY - (float)srcLocMin.y);

        for (loc.x = 0; loc.x < w; ++loc.x)
        {
            float srcX = (float)loc.x / InterpolateScale;

            srcLocMin.x = (int)srcX;
            srcLocMax.x = srcLocMin.x + 1;
            lerpVal.x = smoothStepper(srcX - (float)srcLocMin.x);

            outN[loc] = BasicMath::Lerp(BasicMath::Lerp(toInterp[srcLocMin],
                                                        toInterp[Vector2i(srcLocMax.x, srcLocMin.y)],
                                                        lerpVal.x),
                                        BasicMath::Lerp(toInterp[Vector2i(srcLocMin.x, srcLocMax.y)],
                                                        toInterp[srcLocMax],
                                                        lerpVal.x),
                                        lerpVal.y);
        }
    }
}