#include "Interpolator.h"

#include <assert.h>


void Interpolator2D::Generate(Array2D<float>& outN) const
{
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
    float wF = (float)w,
          hF = (float)h;
	float invScale = 1.0f / InterpolateScale;
	Vector2u loc;
    Vector2u srcLocMin, srcLocMax;
    Vector2f lerpVal;

    for (loc.y = 0; loc.y < h; ++loc.y)
    {
        float srcY = (float)loc.y / InterpolateScale;
        srcY += GridOffset.y;

        //Wrap srcY to be inside the grid.
        while (srcY < 0.0f)
            srcY += hF;
        srcY = fmodf(srcY, hF);

        srcLocMin.y = (unsigned int)srcY;

        srcLocMax.y = srcLocMin.y + 1;
        lerpVal.y = smoothStepper(srcY - (float)srcLocMin.y);

        for (loc.x = 0; loc.x < w; ++loc.x)
        {
            float srcX = (float)loc.x / InterpolateScale;
            srcX += GridOffset.x;

            //Wrap srcX to be inside the grid.
            while (srcX < 0.0f)
                srcX += wF;
            srcX = fmodf(srcX, wF);

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
void Interpolator3D::Generate(Array3D<float>& outN) const
{
    Vector3u toInterpSize((unsigned int)ceilf(outN.GetWidth() / InterpolateScale) + 1,
                          (unsigned int)ceilf(outN.GetHeight() / InterpolateScale) + 1,
                          (unsigned int)ceilf(outN.GetDepth() / InterpolateScale) + 1);
	Noise3D toInterp(toInterpSize.x, toInterpSize.y, toInterpSize.z);
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
                 h = outN.GetHeight(),
                 d = outN.GetDepth();
    float wF = (float)w,
          hF = (float)h,
          dF = (float)d;
	float invScale = 1.0f / InterpolateScale;
	Vector3u loc;
    Vector3u min, max;
    Vector3f lerpVal;

    for (loc.z = 0; loc.z < d; ++loc.z)
    {
        float srcZ = (float)loc.z / InterpolateScale;
        srcZ += GridOffset.z;

        //Wrap srcZ to be inside the grid.
        while (srcZ < 0.0f)
            srcZ += dF;
        srcZ = fmodf(srcZ, dF);

        min.z = (unsigned int)srcZ;
        max.z = min.z + 1;
        lerpVal.z = smoothStepper(srcZ - (float)min.z);

        for (loc.y = 0; loc.y < h; ++loc.y)
        {
            float srcY = (float)loc.y / InterpolateScale;
            srcY += GridOffset.y;

            //Wrap srcY to be inside the grid.
            while (srcY < 0.0f)
                srcY += hF;
            srcY = fmodf(srcY, hF);

            min.y = (unsigned int)srcY;
            max.y = min.y + 1;
            lerpVal.y = smoothStepper(srcY - (float)min.y);

            for (loc.x = 0; loc.x < w; ++loc.x)
            {
                float srcX = (float)loc.x / InterpolateScale;
                srcX += GridOffset.x;

                //Wrap srcX to be inside the grid.
                while (srcX < 0.0f)
                    srcX += wF;
                srcX = fmodf(srcX, wF);

                min.x = (unsigned int)srcX;
                max.x = min.x + 1;
                lerpVal.x = smoothStepper(srcX - (float)min.x);

                //Interpolate along the X, then along the Y, then along the Z.
                float x1, x2, x3, x4;
                x1 = Mathf::Lerp(toInterp[Vector3u(min.x, min.y, min.z)],
                                 toInterp[Vector3u(max.x, min.y, min.z)],
                                 lerpVal.x);
                x2 = Mathf::Lerp(toInterp[Vector3u(min.x, max.y, min.z)],
                                 toInterp[Vector3u(max.x, max.y, min.z)],
                                 lerpVal.x);
                x3 = Mathf::Lerp(toInterp[Vector3u(min.x, min.y, max.z)],
                                 toInterp[Vector3u(max.x, min.y, max.z)],
                                 lerpVal.x);
                x4 = Mathf::Lerp(toInterp[Vector3u(min.x, max.y, max.z)],
                                 toInterp[Vector3u(max.x, max.y, max.z)],
                                 lerpVal.x);
                float y1, y2;
                y1 = Mathf::Lerp(x1, x2, lerpVal.y);
                y2 = Mathf::Lerp(x3, x4, lerpVal.y);
                outN[loc] = Mathf::Lerp(y1, y2, lerpVal.z);
            }
        }
    }
}