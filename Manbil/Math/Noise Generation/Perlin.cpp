#include "Perlin.h"

#include <iostream>
#include <assert.h>
#include "NoiseFilterer.h"


void Perlin2D::Generate(Array2D<float> & outValues) const
{
	FastRand fr(RandSeed);

	Vector2u noiseDim = outValues.GetDimensions();


    //First compute the gradient at each grid point.

    //Calculate the number of gradients that will be needed.
	unsigned int gradientWidth = Mathf::RoundToInt(noiseDim.x / Scale.x),
		         gradientHeight = Mathf::RoundToInt(noiseDim.y / Scale.y);
    if (gradientWidth == 0 || gradientHeight == 0)
    {
        outValues.Fill(0.0f);
        return;
    }
	Array2D<Vector2f> gradients(gradientWidth + 2, gradientHeight + 2);

    //Generate the gradients. by hashing the grid coordinate and using that hash in a look-up table.
	const unsigned int numGradients = 8;
	const Vector2f gradientTable[numGradients] = 
	{
		Vector2f(1.0f, 1.0f),
		Vector2f(-1.0f, 1.0f),
		Vector2f(1.0f, -1.0f),
		Vector2f(-1.0f, -1.0f),
		Vector2f(1.0f, 0.0f),
		Vector2f(0.0f, 1.0f),
		Vector2f(-1.0f, 0.0f),
		Vector2f(0.0f, -1.0f),
	};

	Vector2u loc;
    Vector2i offLoc;
    Vector2i scaledOffset((int)(Offset.x / Scale.x), (int)(Offset.y / Scale.y));
    for (loc.y = 0; loc.y < gradients.GetHeight(); ++loc.y)
    {
        offLoc.y = loc.y + scaledOffset.y;
        offLoc.y %= GradientWrapInterval.y;

        for (loc.x = 0; loc.x < gradients.GetWidth(); ++loc.x)
        {
            offLoc.x = loc.x + scaledOffset.x;
            offLoc.x %= GradientWrapInterval.x;

            fr.Seed = offLoc.GetHashCode() + RandSeed;
			gradients[loc] = gradientTable[Mathf::Abs(fr.GetRandInt()) % numGradients];
		}
	}


	//Now compute the noise for every point.

	float (*smoothStepper)(float inF) = 0;
	switch (SmoothAmount)
	{
		case Smoothness::Linear:
			smoothStepper = [](float inF) { return inF; };
			break;
		case Smoothness::Cubic:
			smoothStepper = &Mathf::Smooth;
			break;
		case Smoothness::Quintic:
			smoothStepper = &Mathf::Supersmooth;
			break;

		default: assert(false);
	}

    //Keep track of the min/max in case the noise should be normalized.
    float min = std::numeric_limits<float>().max(),
          max = std::numeric_limits<float>().min();
	Vector2f lerpGrid, relGrid;
	Vector2u tlGrid;
	Vector2f invScale(1.0f / Scale.x, 1.0f / Scale.y);
	float tlDot, trDot, blDot, brDot;
    Vector2f withinGridOffset(fmodf(Offset.x, Scale.x), fmodf(Offset.y, Scale.y));

    for (loc.y = 0; loc.y < noiseDim.y; ++loc.y)
    {
        lerpGrid.y = ((float)loc.y + withinGridOffset.y) * invScale.y;

        tlGrid.y = (unsigned int)lerpGrid.y;
        relGrid.y = lerpGrid.y - tlGrid.y;

        for (loc.x = 0; loc.x < noiseDim.x; ++loc.x)
        {
            lerpGrid.x = ((float)loc.x + withinGridOffset.x) * invScale.x;

            tlGrid.x = (unsigned int)lerpGrid.x;
            relGrid.x = lerpGrid.x - tlGrid.x;

            //TODO: This bit caused artifacts in the 3D version. See if this can be removed.
            //The noise value at every grid point is 0.
            if (false && relGrid.x == 0.0f && relGrid.y == 0.0f)
            {
                outValues[loc] = 0.0f;
                continue;
            }

            //Get the dot of each grid corner's gradient and the vector from the coordinate to that grid corner.
            Vector2f tl = gradients[gradients.Clamp(tlGrid)],
                     tr = gradients[gradients.Clamp(tlGrid.MoreX())],
                     bl = gradients[gradients.Clamp(tlGrid.MoreY())],
                     br = gradients[gradients.Clamp(tlGrid + Vector2u(1, 1))];
            Vector2f tTL = relGrid,
                     tTR = relGrid - Vector2f(1.0f, 0.0f),
                     tBL = relGrid - Vector2f(0.0f, 1.0f),
                     tBR = relGrid - Vector2f(1.0f, 1.0f);
            tlDot = tl.Dot(tTL);
            trDot = tr.Dot(tTR);
            blDot = bl.Dot(tBL);
            brDot = br.Dot(tBR);

            //Interpolate the values.
            float smoothedX = smoothStepper(relGrid.x);
            float val = Mathf::Lerp(Mathf::Lerp(tlDot, trDot, smoothedX),
                                        Mathf::Lerp(blDot, brDot, smoothedX),
                                        smoothStepper(relGrid.y));
            outValues[loc] = val;

            min = Mathf::Min(val, min);
            max = Mathf::Max(val, max);
        }
    }

    if (RemapValues)
    {
        NoiseFilterer2D nf;
        MaxFilterRegion mfr;
        nf.FillRegion = &mfr;
        nf.RemapValues_OldVals = Interval(min, max, 0.00001f);
        nf.RemapValues(&outValues);
    }
}


void Perlin3D::Generate(Array3D<float> & outNoise) const
{
    FastRand fr(RandSeed);

    Vector3u dimensions = outNoise.GetDimensions();


    //First compute the gradient at every grid.

    //Calculate the number of gradient points that will be needed.
    Vector3u gradientDims((unsigned int)Mathf::RoundToInt(dimensions.x / Scale.x) + 2,
                          (unsigned int)Mathf::RoundToInt(dimensions.y / Scale.y) + 2,
                          (unsigned int)Mathf::RoundToInt(dimensions.z / Scale.z) + 2);

    Array3D<Vector3f> gradients(gradientDims.x, gradientDims.y, gradientDims.z);

    //Compute the gradient by hashing the grid coordinate
    //  and using the hash to look up a select few pre-made gradients.
    const unsigned int numGradients = 16;
    const Vector3f gradientTable[numGradients] =
    {
        Vector3f(1, 1, 0),
        Vector3f(-1, 1, 0),
        Vector3f(1, -1, 0),
        Vector3f(-1, -1, 0),

        Vector3f(1, 0, 1),
        Vector3f(-1, 0, 1),
        Vector3f(1, 0, -1),
        Vector3f(-1, 0, -1),

        Vector3f(0, 1, 1),
        Vector3f(0, -1, 1),
        Vector3f(0, 1, -1),
        Vector3f(0, -1, -1),

        Vector3f(1, 1, 0),
        Vector3f(0, -1, 1),
        Vector3f(-1, 1, 0),
        Vector3f(0, -1, -1),
    };
    Vector3u loc;
    Vector3i offLoc;
    Vector3i scaledOffset((int)(Offset.x / Scale.x), (int)(Offset.y / Scale.y), (int)(Offset.z / Scale.z));
    for (loc.z = 0; loc.z < gradientDims.z; ++loc.z)
    {
        offLoc.z = loc.z + scaledOffset.z;
        offLoc.z %= GradientWrapInterval.z;

        for (loc.y = 0; loc.y < gradientDims.y; ++loc.y)
        {
            offLoc.y = loc.y + scaledOffset.y;
            offLoc.y %= GradientWrapInterval.y;

            for (loc.x = 0; loc.x < gradientDims.x; ++loc.x)
            {
                offLoc.x = loc.x + scaledOffset.x;
                offLoc.x %= GradientWrapInterval.x;

                fr.Seed = offLoc.GetHashCode() + RandSeed;
                gradients[loc] = gradientTable[Mathf::Abs(fr.GetRandInt()) % numGradients];
            }
        }
    }


    //Now compute the noise for every point.

    float(*smoothStepper)(float inF);
    switch (SmoothAmount)
    {
        case Smoothness::Linear:
            smoothStepper = [](float inF) { return inF; };
            break;
        case Smoothness::Cubic:
            smoothStepper = &Mathf::Smooth;
            break;
        case Smoothness::Quintic:
            smoothStepper = &Mathf::Supersmooth;
            break;

        default: assert(false);
    }


    //Keep track of the min/max in case the noise should be normalized.
    float min = std::numeric_limits<float>().max(),
          max = std::numeric_limits<float>().min();
    Vector3f lerpGrid, relGrid, relGridLess;
    Vector3u minGrid;
    Vector3f invScale(1.0f / Scale.x, 1.0f / Scale.y, 1.0f / Scale.z);
    float minXYZ_dot, minXY_maxZ_dot, minX_maxY_minZ_dot, minX_maxYZ_dot,
          maxX_minYZ_dot, maxX_minY_maxZ_dot, maxXY_minZ_dot, maxXYZ_dot;
    Vector3f withinGridOffset(fmodf(Offset.x, Scale.x), fmodf(Offset.y, Scale.y), fmodf(Offset.z, Scale.z));

    for (loc.z = 0; loc.z < dimensions.z; ++loc.z)
    {
        lerpGrid.z = ((float)loc.z + withinGridOffset.z) * invScale.z;

        minGrid.z = (unsigned int)lerpGrid.z;
        relGrid.z = lerpGrid.z - (float)minGrid.z;
        relGridLess.z = relGrid.z - 1.0f;

        for (loc.y = 0; loc.y < dimensions.y; ++loc.y)
        {
            lerpGrid.y = ((float)loc.y + withinGridOffset.y) * invScale.y;

            minGrid.y = (unsigned int)lerpGrid.y;
            relGrid.y = lerpGrid.y - (float)minGrid.y;
            relGridLess.y = relGrid.y - 1.0f;

            for (loc.x = 0; loc.x < dimensions.x; ++loc.x)
            {
                lerpGrid.x = ((float)loc.x + withinGridOffset.x) * invScale.x;

                minGrid.x = (unsigned int)lerpGrid.x;
                relGrid.x = lerpGrid.x - (float)minGrid.x;
                relGridLess.x = relGrid.x - 1.0f;

                //Get the dot of each grid corner's gradient and the vector from the coordinate to that grid corner.

                minXYZ_dot = gradients[gradients.Clamp(minGrid)].Dot(relGrid);
                minXY_maxZ_dot = gradients[gradients.Clamp(minGrid.MoreZ())].Dot(Vector3f(relGrid.x, relGrid.y, relGridLess.z));
                minX_maxY_minZ_dot = gradients[gradients.Clamp(minGrid.MoreY())].Dot(Vector3f(relGrid.x, relGridLess.y, relGrid.z));
                minX_maxYZ_dot = gradients[gradients.Clamp(minGrid.MoreY().MoreZ())].Dot(Vector3f(relGrid.x, relGridLess.y, relGridLess.z));
                
                maxX_minYZ_dot = gradients[gradients.Clamp(minGrid.MoreX())].Dot(Vector3f(relGridLess.x, relGrid.y, relGrid.z));
                maxX_minY_maxZ_dot = gradients[gradients.Clamp(minGrid.MoreX().MoreZ())].Dot(Vector3f(relGridLess.x, relGrid.y, relGridLess.z));
                maxXY_minZ_dot = gradients[gradients.Clamp(minGrid.MoreX().MoreY())].Dot(Vector3f(relGridLess.x, relGridLess.y, relGrid.z));
                maxXYZ_dot = gradients[gradients.Clamp(minGrid.MoreX().MoreY().MoreZ())].Dot(relGridLess);

                //Interpolate the values one axis at a time.
                Vector3f smoothed(smoothStepper(relGrid.x), smoothStepper(relGrid.y), smoothStepper(relGrid.z));
                float val = Mathf::Lerp(Mathf::Lerp(Mathf::Lerp(minXYZ_dot, maxX_minYZ_dot, smoothed.x),
                                                            Mathf::Lerp(minX_maxY_minZ_dot, maxXY_minZ_dot, smoothed.x),
                                                            smoothed.y),
                                            Mathf::Lerp(Mathf::Lerp(minXY_maxZ_dot, maxX_minY_maxZ_dot, smoothed.x),
                                                            Mathf::Lerp(minX_maxYZ_dot, maxXYZ_dot, smoothed.x),
                                                            smoothed.y),
                                            smoothed.z);
                outNoise[loc] = val;

                min = Mathf::Min(val, min);
                max = Mathf::Max(val, max);
            }
        }
    }

    if (RemapValues)
    {
        NoiseFilterer3D nf;
        MaxFilterVolume mfv;
        nf.FillVolume = &mfv;
        nf.RemapValues_OldVals = Interval(min, max, 0.00001f);
        nf.RemapValues(&outNoise);
    }
}