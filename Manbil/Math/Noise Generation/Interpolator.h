#pragma once

#include "BasicGenerators.h"

//Scales up a noise grid and interpolates between the grid values to be smooth.
class Interpolator2D : public Generator2D
{
public:

    //TODO: Test that this works. Figure out what "InterpolateWidth" and "InterpolateHeight" are for. Change to use linear/cubic/quintic interpolation.
	const Generator2D * NoiseToInterpolate;
	float InterpolateScale;
	int InterpolateWidth, InterpolateHeight;

	Interpolator2D(const Generator2D * toInterpolate, int width, int height, float interpScale = 1.0f)
		: NoiseToInterpolate(toInterpolate), InterpolateScale(interpScale), InterpolateWidth(width), InterpolateHeight(height) { }

	virtual void Generate(Noise2D & outNoise) const override;

private:

	void ComputeTempSmoothedNoise(Noise2D & tempSmoothedNoise) const;
};


//Scales up a noise grid and interpolates between the grid values to be smooth.
class Interpolator3D : public Generator3D
{
public:

    //TODO: Finish after completing the above TODO.

    const Generator3D * NoiseToInterpolate;
    float InterpolateScale;
    Vector3i InterpolateSize;

    Interpolator3D(const Generator3D * toInterpolate, Vector3i interpSize, float interpScale = 1.0f)
        : NoiseToInterpolate(toInterpolate), InterpolateScale(interpScale), InterpolateSize(interpSize) { }

    virtual void Generate(Noise3D & outNoise) const override;

private:

    void ComputeTempSmoothedNoise(Noise3D & tempSmoothedNoise) const;
};