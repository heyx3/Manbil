#include "Interpolator.h"

#include <assert.h>



void Interpolator2D::Generate(Array2D<float> & outN) const
{
    assert(InterpolateScale > 0.0f);

    Vector2u toInterpSize((unsigned int)ceilf(outN.GetWidth() / InterpolateScale) + 1,
                          (unsigned int)ceilf(outN.GetHeight() / InterpolateScale) + 1);
	Noise2D toInterp(toInterpSize.x, toInterpSize.y);
    NoiseToInterpolate->Generate(toInterp);

    float(*smoothStepper)(float inVal);
    switch (SmoothAmount)
    {
        case Smoothness::I2S_LINEAR:
            smoothStepper = [](float inVal) { return inVal; };
            break;
        case Smoothness::I2S_CUBIC:
            smoothStepper = Mathf::Smooth;
            break;
        case Smoothness::I2S_QUINTIC:
            smoothStepper = Mathf::Supersmooth;
            break;

        default: assert(false);
    }


	unsigned int w = outN.GetWidth(),
                 h = outN.GetHeight();
	float invScale = 1.0f / InterpolateScale;
	Vector2u loc;
    Vector2u srcLocMin, srcLocMax;
    Vector2f lerpVal;

    for (loc.y = 0; loc.y < h; ++loc.y)
    {
        float srcY = (float)loc.y / InterpolateScale;

        srcLocMin.y = (unsigned int)srcY;
        srcLocMax.y = srcLocMin.y + 1;
        lerpVal.y = smoothStepper(srcY - (float)srcLocMin.y);

        for (loc.x = 0; loc.x < w; ++loc.x)
        {
            float srcX = (float)loc.x / InterpolateScale;

            srcLocMin.x = (unsigned int)srcX;
            srcLocMax.x = srcLocMin.x + 1;
            lerpVal.x = smoothStepper(srcX - (float)srcLocMin.x);

            outN[loc] = Mathf::Lerp(Mathf::Lerp(toInterp[srcLocMin],
                                                        toInterp[Vector2u(srcLocMax.x, srcLocMin.y)],
                                                        lerpVal.x),
                                        Mathf::Lerp(toInterp[Vector2u(srcLocMin.x, srcLocMax.y)],
                                                        toInterp[srcLocMax],
                                                        lerpVal.x),
                                        lerpVal.y);
        }
    }
}