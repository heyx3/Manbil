#pragma once

#include "BasicGenerators.h"
#include "../Fake3DArray.h"


//2D Perlin noise generator.
class Perlin2D : public Generator
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
    Vector2f Scale;
    Vector2i Offset;
    
	Perlin2D(Vector2f scale = Vector2f(1.0f, 1.0f), Smoothness amount = Smoothness::Linear, Vector2i offset = Vector2i(), int seed = 12345)
        : Scale(scale), SmoothAmount(amount), Offset(offset), RandSeed(seed) { }
	Perlin2D(float scale = 1.0f, Smoothness amount = Smoothness::Linear, Vector2i offset = Vector2i(), int seed = 12345)
        : Scale(scale, scale), SmoothAmount(amount), Offset(offset), RandSeed(seed) { }

	virtual void Generate(Fake2DArray<float> & outValues) const override;
};

//3D Perlin noise generator.
class Perlin3D
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
    Vector3f Scale;
    Vector3i Offset;

    Perlin3D(float scale = 1.0f, Smoothness amount = Smoothness::Linear, Vector3i offset = Vector3i(), int seed = 12345)
        : Scale(scale, scale, scale), SmoothAmount(amount), Offset(offset), RandSeed(seed) { }
    Perlin3D(Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f), Smoothness amount = Smoothness::Linear, Vector3i offset = Vector3i(), int seed = 12345)
        : Scale(scale), SmoothAmount(amount), Offset(offset), RandSeed(seed) { }

    void Generate(Fake3DArray<float> & outValues) const;
};