#pragma once

#include "BasicGenerators.h"

//Generates pseudo-random white noise.
class Value2D : public Generator
{
public:

	Value2D(int seed = 12345) : RandSeed(seed) { }

	int RandSeed;

	virtual void Generate(Fake2DArray<float> & outValues) const override;
};