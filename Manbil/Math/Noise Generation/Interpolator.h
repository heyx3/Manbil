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
	const Generator2D* NoiseToInterpolate;

	float InterpolateScale;
    Vector2f GridOffset;


	Interpolator2D(const Generator2D* toInterpolate, Smoothness smoothAmount,
                   float interpScale = 1.0f, Vector2f gridOffset = Vector2f())
		: NoiseToInterpolate(toInterpolate), SmoothAmount(smoothAmount),
          InterpolateScale(interpScale), GridOffset(gridOffset) { }


	virtual void Generate(Noise2D& outNoise) const override;
};


//Scales up a noise grid and interpolates between the grid values to be smooth.
class Interpolator3D : public Generator3D
{
public:

    enum Smoothness
    {
        I2S_LINEAR,
        I2S_CUBIC,
        I2S_QUINTIC,
    };

    
    Smoothness SmoothAmount;
	const Generator3D* NoiseToInterpolate;
	
    float InterpolateScale;
    Vector3f GridOffset;

	
    Interpolator3D(const Generator3D* toInterpolate, Smoothness smoothAmount,
                   float interpScale = 1.0f, Vector3f gridOffset = Vector3f())
		: NoiseToInterpolate(toInterpolate), SmoothAmount(smoothAmount),
          InterpolateScale(interpScale), GridOffset(gridOffset) { }


	virtual void Generate(Noise3D& outNoise) const override;
};