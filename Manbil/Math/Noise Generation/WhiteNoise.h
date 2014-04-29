#pragma once

#include "BasicGenerators.h"

//Generates pseudo-random white noise.
class WhiteNoise2D : public Generator2D
{
public:

    WhiteNoise2D(int seed = 12345) : RandSeed(seed) { }

    int RandSeed;

    virtual void Generate(Fake2DArray<float> & outValues) const override;
};
//Generates pseudo-random white noise.
class WhiteNoise3D : public Generator3D
{
public:

    WhiteNoise3D(int seed = 12345) : RandSeed(seed) { }

    int RandSeed;

    virtual void Generate(Fake3DArray<float> & outValues) const override;
};