#pragma once

#include "../LowerMath.hpp"

typedef Array2D<float> Noise2D;
typedef Array3D<float> Noise3D;

//TODO: All generators have an "offset" and use their FastRand math based only on coordinate hashing/generator seed.

//******************************************************************************************
//All noise values in this generation system are assumed to generate values between 0 and 1.
//******************************************************************************************


#pragma region TwoD


//Analysis of 2D noise.
namespace NoiseAnalysis2D
{
    struct MinMax { float Min, Max; MinMax(float min, float max) : Min(min), Max(max) { } };
    MinMax GetMinAndMax(const Noise2D & noise);

    float GetAverage(const Noise2D & noise);
}


//Abstract class that can be told to generate some kind of noise.
struct Generator2D
{
public:

    virtual ~Generator2D(void) { }

	//The "generate noise" function. Given the coordinates of the noise, gives a noise value.
	virtual void Generate(Noise2D & outNoise) const = 0;
};


//Generates a constant noise value.
struct FlatNoise2D : public Generator2D
{
public:
	float FlatValue;
	FlatNoise2D(float flatValue = 0.0f) : FlatValue(flatValue) { }
	virtual void Generate(Noise2D & outNoise) const override { outNoise.Fill(FlatValue); }
};


//Generates random noise using a fast PRNG.
struct WhiteNoise2D : public Generator2D
{
public:
    Vector2i SeedOffset;
	int Seed;
	WhiteNoise2D(int seed = 12345, Vector2i seedOffset = Vector2i()) : Seed(seed), SeedOffset(seedOffset) { }
	virtual void Generate(Noise2D & outNoise) const override
    {
        int seed = Seed;
        Vector2i offset = SeedOffset;
        outNoise.FillFunc([seed, offset](Vector2u loc, float * fOut)
        {
            Vector3i seed(ToV2i(loc) + offset, seed);
           *fOut = FastRand(seed.GetHashCode()).GetZeroToOne();
        });
    }
};


#pragma endregion


#pragma region ThreeD


//Analysis of 3D noise.
namespace NoiseAnalysis3D
{
    struct MinMax { float Min, Max; MinMax(float min, float max) : Min(min), Max(max) { } };
    MinMax GetMinAndMax(Noise3D & noise);

    float GetAverage(Noise3D & noise);
}


//Abstract class that can be told to generate some kind of noise.
struct Generator3D
{
public:
    
    virtual ~Generator3D(void) { }

    //The "generate noise" function. Given the coordinates of the noise, gives a noise value.
    virtual void Generate(Noise3D & outNoise) const = 0;
};


//Generates a constant noise value.
struct FlatNoise3D : public Generator3D
{
public:
    float FlatValue;
    FlatNoise3D(float flatValue = 0.0f) : FlatValue(flatValue) { }
    virtual void Generate(Noise3D & outNoise) const override { outNoise.Fill(FlatValue); }
};


//Generates random noise using a fast PRNG.
struct WhiteNoise3D : public Generator3D
{
public:
    Vector3i SeedOffset;
	int Seed;
	WhiteNoise3D(int seed = 12345, Vector3i seedOffset = Vector3i()) : Seed(seed), SeedOffset(seedOffset) { }
	virtual void Generate(Noise3D & outNoise) const override
    {
        int seed = Seed;
        Vector3i offset = SeedOffset;
        outNoise.FillFunc([seed, offset](Vector3u loc, float * fOut)
        {
            Vector4i seed(ToV3i(loc) + offset, seed);
           *fOut = FastRand(seed.GetHashCode()).GetZeroToOne();
        });
    }
};


#pragma endregion