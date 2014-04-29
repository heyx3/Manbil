#pragma once

#include <math.h>
#include "BasicGenerators.h"


//A noise Generator that overlays multiple Generators on top of each other with different weights.
class LayeredOctave2D : public Generator2D
{
public:

	LayeredOctave2D(int numbOctaves, const float octaveWeights[], const Generator2D *const*const octaves);
	~LayeredOctave2D(void);

	int GetNumbOctaves(void) const { return Octaves; }

	//Generates the layered noise and puts it into the given array.
	virtual void Generate(Noise2D & outNoiseArray) const override;

private:

	Generator2D ** noises;
	int Octaves;
	float * OctaveStrengths;
};


//A noise Generator that overlays multiple Generators on top of each other with different weights.
class LayeredOctave3D : public Generator3D
{
public:

    LayeredOctave3D(int numbOctaves, const float octaveWeights[], const Generator3D *const*const octaves);
    ~LayeredOctave3D(void);

    int GetNumbOctaves(void) const { return Octaves; }

    //Generates the layered noise and puts it into the given array.
    virtual void Generate(Noise3D & outNoiseArray) const override;

private:

    Generator3D ** noises;
    int Octaves;
    float * OctaveStrengths;
};