#include "Perlin.h"

#include <iostream>
#include <assert.h>


void Perlin::Generate(Fake2DArray<float> & outValues) const
{
	FastRand fr(RandSeed);

	int width = outValues.GetWidth(), height = outValues.GetHeight();
	
	int x, y;
	Vector2i loc;


	//First compute the gradient at every grid.

	//Compute the gradient by hashing the grid coordinate
	//  and using the hash to look up a select few pre-made gradients.
	const int numGradients = 8;
	const Vector2f gradientTable[numGradients] = 
	{
		Vector2f(1.0f, 1.0f).Normalized(),
		Vector2f(-1.0f, 1.0f).Normalized(),
		Vector2f(1.0f, -1.0f).Normalized(),
		Vector2f(-1.0f, -1.0f).Normalized(),
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

	for (x = 0; x < gradients.GetWidth(); ++x)
	{
		loc.x = x;

		for (y = 0; y < gradients.GetHeight(); ++y)
		{
			loc.y = y;

			int hash = fr.GetRandInt();
			gradients[loc] = gradientTable[BasicMath::Abs(fr.GetRandInt()) % numGradients];
		}
	}


	//Now compute the noise for every point.

	Vector2f lerpGrid, relGrid;
	Vector2i tlGrid;
	float invScale = 1.0f / Scale;
	float tlDot, trDot, blDot, brDot;


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

	for (x = 0; x < width; ++x)
	{
		loc.x = x;

		lerpGrid.x = x * invScale;
        tlGrid.x = (int)lerpGrid.x;
		relGrid.x = lerpGrid.x - tlGrid.x;

		for (y = 0; y < height; ++y)
		{
			loc.y = y;

			lerpGrid.y = y * invScale;
            tlGrid.y = (int)lerpGrid.y;
			relGrid.y = lerpGrid.y - tlGrid.y;

			//The noise value at every grid point is 0.
			if (relGrid.x == 0.0f && relGrid.y == 0.0f)
			{
				outValues[loc] = 0.0f;
				continue;
			}

			//Get the dot of each grid corner's gradient and the vector from the coordinate to that grid corner.
			Vector2f tl = gradients[gradients.Wrap(tlGrid)],
					 tr = gradients[gradients.Wrap(tlGrid.Right())],
					 bl = gradients[gradients.Wrap(tlGrid.Below())],
                     br = gradients[gradients.Wrap(tlGrid.BelowRight())];
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

	for (x = 0; x < outValues.GetWidth(); ++x)
	{
		for (y = 0; y < outValues.GetHeight(); ++y)
		{
			if (mm.Min == mm.Max)
			{
				outValues[Vector2i(x, y)] = 0.5f;
			}
			else
			{
				outValues[Vector2i(x, y)] = BasicMath::Remap(mm.Min, mm.Max,
															 0.0f, 1.0f,
															 outValues[Vector2i(x, y)]);
			}
		}
	}
}