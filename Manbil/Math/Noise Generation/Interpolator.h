#pragma once

#include "BasicGenerators.h"

//Scales up a noise grid and interpolates between the grid values to be smooth.
class Interpolator2D : public Generator2D
{
public:

    enum Smoothness
    {
        I2S_LINEAR,
        I2S_CUBIC,
        I2S_QUINTIC,
    };

    Smoothness SmoothAmount;
	const Generator2D * NoiseToInterpolate;
	float InterpolateScale;

	Interpolator2D(const Generator2D * toInterpolate, Smoothness smoothAmount, float interpScale = 1.0f)
		: NoiseToInterpolate(toInterpolate), SmoothAmount(smoothAmount), InterpolateScale(interpScale) { }

	virtual void Generate(Noise2D & outNoise) const override;
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