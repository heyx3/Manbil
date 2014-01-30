#pragma once

#include "Fake2DArray.h"
#include "BasicGenerators.h"

//Scales up a noise grid and interpolates between the grid values to be smooth.
class Interpolator : public Generator
{
public:

	const Generator * NoiseToInterpolate;
	float InterpolateScale;
	int InterpolateWidth, InterpolateHeight;

	Interpolator(const Generator * toInterpolate, int width, int height, float interpScale = 1.0f)
		: NoiseToInterpolate(toInterpolate), InterpolateScale(interpScale), InterpolateWidth(width), InterpolateHeight(height) { }

	virtual void Generate(Noise2D & outNoise) const override;

private:

	void ComputeTempSmoothedNoise(Noise2D & tempSmoothedNoise) const;

	float GetInterpolatedNoise(Vector2f pos, Noise2D & tempSmoothedNoise) const;
};