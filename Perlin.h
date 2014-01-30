#pragma once

#include "BasicGenerators.h"

//Perlin noise generator.
class Perlin : public Generator
{
public:
	
	//The different qualities of interpolation.
	//Better interpolations mean worse performance.
	enum Smoothness
	{
		Linear,
		Cubic,
		Quintic,
	};
	Smoothness SmoothAmount;

	int RandSeed;
	float Scale;

	Perlin(float scale = 1.0f, Smoothness amount = Smoothness::Linear, int seed = 12345) : Scale(scale), SmoothAmount(amount), RandSeed(seed) { }
	Perlin(const Perlin & cpy) : Scale(cpy.Scale), SmoothAmount(cpy.SmoothAmount), RandSeed(cpy.RandSeed) { }

	virtual void Generate(Fake2DArray<float> & outValues) const override;
};