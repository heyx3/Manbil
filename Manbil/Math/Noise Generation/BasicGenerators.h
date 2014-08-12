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


//Generates noise using a noise array.
struct CopyNoise2D : public Generator2D
{
public:
	Noise2D * ToCopy;
	Vector2i CopyOffset;
	CopyNoise2D(Noise2D * toCopy = 0, Vector2i copyOffset = Vector2i(0, 0)) : ToCopy(toCopy), CopyOffset(copyOffset) { }
	virtual void Generate(Noise2D & outNoise) const override { outNoise.Fill(*ToCopy, 0.0f, CopyOffset); }
};



//Generates random noise using a fast PRNG.
struct WhiteNoise2D : public Generator2D
{
public:
	int Seed;
	WhiteNoise2D(int seed = 12345) : Seed(seed) { }
	virtual void Generate(Noise2D & outNoise) const override
    {
        int s = Seed;
        outNoise.FillFunc([s](Vector2u loc, float * fOut)
        {
           *fOut = FastRand(Vector3u(loc, s).GetHashCode()).GetZeroToOne();
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


//Generates noise using a noise array.
struct CopyNoise3D : public Generator3D
{
public:
    Noise3D * ToCopy;
    Vector3i CopyOffset;
    CopyNoise3D(Noise3D * toCopy = 0, Vector3i copyOffset = Vector3i(0, 0, 0))
        : ToCopy(toCopy), CopyOffset(copyOffset) { }
    virtual void Generate(Noise3D & outNoise) const override { outNoise.Fill(*ToCopy, 0.0f, CopyOffset); }
};



//Generates random noise using a fast PRNG.
struct WhiteNoise3D : public Generator3D
{
public:
    int Seed;
    WhiteNoise3D(int seed = 12345) : Seed(seed) { }
    virtual void Generate(Noise3D & outNoise) const override
    {
        FastRand fr;
        int s = Seed;
        outNoise.FillFunc([&fr, s](Vector3u loc, float * fOut)
        {
            fr.Seed = Vector4u(loc, s).GetHashCode();
            *fOut = fr.GetZeroToOne();
        }); 
    }
};


#pragma endregion