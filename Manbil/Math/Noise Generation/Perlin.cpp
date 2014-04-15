#include "Perlin.h"

#include <iostream>
#include <assert.h>


void Perlin2D::Generate(Fake2DArray<float> & outValues) const
{
	FastRand fr(RandSeed);

	int width = outValues.GetWidth(), height = outValues.GetHeight();
	
	Vector2i loc;


	//First compute the gradient at every grid.

	//Compute the gradient by hashing the grid coordinate
	//  and using the hash to look up a select few pre-made gradients.
	const int numGradients = 8;
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

	int gradientWidth = BasicMath::RoundToInt(width / Scale),
		gradientHeight = BasicMath::RoundToInt(height / Scale);
    if (gradientWidth == 0 || gradientHeight == 0)
    {
        outValues.Fill(0.0f);
        return;
    }
	Fake2DArray<Vector2f> gradients(gradientWidth, gradientHeight);

    for (loc.y = 0; loc.y < gradients.GetHeight(); ++loc.y)
    {
        for (loc.x = 0; loc.x < gradients.GetWidth(); ++loc.x)
		{
			gradients[loc] = gradientTable[BasicMath::Abs(fr.GetRandInt()) % numGradients];
		}
	}


	//Now compute the noise for every point.

	float (*smoothStepper)(float inF);
	switch (SmoothAmount)
	{
		case Smoothness::Linear:
			smoothStepper = [](float inF) { return inF; };
			break;
		case Smoothness::Cubic:
			smoothStepper = &BasicMath::Smooth;
			break;
		case Smoothness::Quintic:
			smoothStepper = &BasicMath::Supersmooth;
			break;

		default: assert(false);
	}


	Vector2f lerpGrid, relGrid;
	Vector2i tlGrid;
	float invScale = 1.0f / Scale;
	float tlDot, trDot, blDot, brDot;

    for (loc.y = 0; loc.y < height; ++loc.y)
    {
        lerpGrid.y = loc.y * invScale;
        tlGrid.y = (int)lerpGrid.y;
        relGrid.y = lerpGrid.y - tlGrid.y;

        for (loc.x = 0; loc.x < width; ++loc.x)
        {
            lerpGrid.x = loc.x * invScale;
            tlGrid.x = (int)lerpGrid.x;
            relGrid.x = lerpGrid.x - tlGrid.x;

            //TODO: This bit caused artifacts in the 3D version. See if this can be removed.
            //The noise value at every grid point is 0.
            if (relGrid.x == 0.0f && relGrid.y == 0.0f)
            {
                outValues[loc] = 0.0f;
                continue;
            }

            //Get the dot of each grid corner's gradient and the vector from the coordinate to that grid corner.
            Vector2f tl = gradients[gradients.Wrap(tlGrid)],
                     tr = gradients[gradients.Wrap(tlGrid.MoreX())],
                     bl = gradients[gradients.Wrap(tlGrid.MoreY())],
                     br = gradients[gradients.Wrap(tlGrid.MoreXY())];
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
            float val = BasicMath::Lerp(BasicMath::Lerp(tlDot, trDot, smoothedX),
                                        BasicMath::Lerp(blDot, brDot, smoothedX),
                                        smoothStepper(relGrid.y));
            outValues[loc] = val;
        }
    }


	NoiseAnalysis2D::MinMax mm = NoiseAnalysis2D::GetMinAndMax(outValues);

    int size = outValues.GetWidth() * outValues.GetHeight();
    for (int i = 0; i < size; ++i)
        outValues.GetArray()[i] = BasicMath::Remap(mm.Min, mm.Max, 0.0f, 1.0, outValues.GetArray()[i]);
}


void Perlin3D::Generate(Fake3DArray<float> & outNoise) const
{
    FastRand fr(RandSeed);

    Vector3i dimensions(outNoise.GetWidth(), outNoise.GetHeight(), outNoise.GetDepth());


    //First compute the gradient at every grid.

    //Compute the gradient by hashing the grid coordinate
    //  and using the hash to look up a select few pre-made gradients.
    const int numGradients = 16;
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

    Vector3i gradientDims(BasicMath::RoundToInt(dimensions.x / Scale),
                          BasicMath::RoundToInt(dimensions.y / Scale),
                          BasicMath::RoundToInt(dimensions.z / Scale));
    if (gradientDims.x == 0 || gradientDims.y == 0 || gradientDims.z == 0)
    {
        outNoise.Fill(0.0f);
        return;
    }

    Fake3DArray<Vector3f> gradients(gradientDims.x, gradientDims.y, gradientDims.z);

    Vector3i loc, offLoc;
    for (loc.z = 0; loc.z < gradients.GetDepth(); ++loc.z)
    {
        offLoc.z = loc.z + Offset.z;

        for (loc.y = 0; loc.y < gradients.GetHeight(); ++loc.y)
        {
            offLoc.y = loc.y + Offset.y;

            for (loc.x = 0; loc.x < gradients.GetWidth(); ++loc.x)
            {
                offLoc.x = loc.x + Offset.x;

                gradients[loc] = gradientTable[BasicMath::Abs(FastRand(offLoc.GetHashCode()).GetRandInt()) % numGradients];
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
        smoothStepper = &BasicMath::Smooth;
        break;
    case Smoothness::Quintic:
        smoothStepper = &BasicMath::Supersmooth;
        break;

    default: assert(false);
    }


    Vector3f lerpGrid, relGrid, relGridLess;
    Vector3i minGrid;
    float invScale = 1.0f / Scale;
    float minXYZ_dot, minXY_maxZ_dot, minX_maxY_minZ_dot, minX_maxYZ_dot,
          maxX_minYZ_dot, maxX_minY_maxZ_dot, maxXY_minZ_dot, maxXYZ_dot;

    for (loc.z = 0; loc.z < dimensions.z; ++loc.z)
    {
        offLoc.z = loc.z + Offset.z;

        lerpGrid.z = offLoc.z * invScale;
        minGrid.z = (int)lerpGrid.z;
        relGrid.z = lerpGrid.z - minGrid.z;
        relGridLess.z = relGrid.z - 1.0f;

        for (loc.y = 0; loc.y < dimensions.y; ++loc.y)
        {
            offLoc.y = loc.y + Offset.y;

            lerpGrid.y = offLoc.y * invScale;
            minGrid.y = (int)lerpGrid.y;
            relGrid.y = lerpGrid.y - minGrid.y;
            relGridLess.y = relGrid.y - 1.0f;

            for (loc.x = 0; loc.x < dimensions.x; ++loc.x)
            {
                offLoc.x = loc.x + Offset.x;

                lerpGrid.x = offLoc.x * invScale;
                minGrid.x = (int)lerpGrid.x;
                relGrid.x = lerpGrid.x - minGrid.x;
                relGridLess.x = relGrid.x - 1.0f;

                //Get the dot of each grid corner's gradient and the vector from the coordinate to that grid corner.

                minXYZ_dot = gradients[gradients.Wrap(minGrid)].Dot(relGrid);
                minXY_maxZ_dot = gradients[gradients.Wrap(minGrid.MoreZ())].Dot(Vector3f(relGrid.x, relGrid.y, relGridLess.z));
                minX_maxY_minZ_dot = gradients[gradients.Wrap(minGrid.MoreY())].Dot(Vector3f(relGrid.x, relGridLess.y, relGrid.z));
                minX_maxYZ_dot = gradients[gradients.Wrap(minGrid.MoreY().MoreZ())].Dot(Vector3f(relGrid.x, relGridLess.y, relGridLess.z));
                
                maxX_minYZ_dot = gradients[gradients.Wrap(minGrid.MoreX())].Dot(Vector3f(relGridLess.x, relGrid.y, relGrid.z));
                maxX_minY_maxZ_dot = gradients[gradients.Wrap(minGrid.MoreX().MoreZ())].Dot(Vector3f(relGridLess.x, relGrid.y, relGridLess.z));
                maxXY_minZ_dot = gradients[gradients.Wrap(minGrid.MoreX().MoreY())].Dot(Vector3f(relGridLess.x, relGridLess.y, relGrid.z));
                maxXYZ_dot = gradients[gradients.Wrap(minGrid.MoreX().MoreY().MoreZ())].Dot(relGridLess);

                //Interpolate the values one axis at a time.
                Vector3f smoothed(smoothStepper(relGrid.x), smoothStepper(relGrid.y), smoothStepper(relGrid.z));
                float val = BasicMath::Lerp(BasicMath::Lerp(BasicMath::Lerp(minXYZ_dot, maxX_minYZ_dot, smoothed.x),
                                                            BasicMath::Lerp(minX_maxY_minZ_dot, maxXY_minZ_dot, smoothed.x),
                                                            smoothed.y),
                                            BasicMath::Lerp(BasicMath::Lerp(minXY_maxZ_dot, maxX_minY_maxZ_dot, smoothed.x),
                                                            BasicMath::Lerp(minX_maxYZ_dot, maxXYZ_dot, smoothed.x),
                                                            smoothed.y),
                                            smoothed.z);
                outNoise[loc] = val;
            }
        }
    }

    //Remap to the range [0, 1].

    NoiseAnalysis3D::MinMax mm = NoiseAnalysis3D::GetMinAndMax(outNoise);

    if (mm.Min == mm.Max)
    {
        outNoise.Fill(0.5f);
        return;
    }

    int size = outNoise.GetWidth() * outNoise.GetHeight() * outNoise.GetDepth();
    for (int i = 0; i < size; ++i)
        outNoise.GetArray()[i] = BasicMath::Remap(mm.Min, mm.Max, 0.0f, 1.0, outNoise.GetArray()[i]);
}