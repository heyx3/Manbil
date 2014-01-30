#pragma once
#include <math.h>
#include "Value2D.h"
#include "BasicGenerators.h"

//A 2D noise Generator, overlaying multiple Generators.
class LayeredOctave : public Generator
{
public:

	LayeredOctave(int numbOctaves, const float octaveWeights[], const Generator *const*const octaves);
	~LayeredOctave(void);

	int GetNumbOctaves(void) const { return Octaves; }

	//Generates the layered noise and puts it into the given array.
	virtual void Generate(Noise2D & outNoiseArray) const override;

private:

	Generator ** noises;
	int Octaves;
	float * OctaveStrengths;
};