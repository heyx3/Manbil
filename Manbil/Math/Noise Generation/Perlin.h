#pragma once

#include "BasicGenerators.h"
#include "../Array3D.h"


//2D Perlin noise generator.
//Generates gradient values for each point on a grid, and interpolates between these gradients for each pixel of noise.
class Perlin2D : public Generator2D
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
    //The scale of the noise (i.e. the number of noise pixels inside a single grid element along each axis).
    Vector2f Scale;
    //The offset of the noise (used to make different adjacent pieces of noise fit together without forcing the noise to be tileable).
    Vector2i Offset;
    //The generated noise will be tileable every "GradientWrapInterval.x" grid points along the X, and every "GradientWrapInterval.y" grid points along the Y.
    Vector2i GradientWrapInterval;
    //This generator will often generate dark values. If this flag is true, those values will be normalized.
    bool RemapValues;
    
    Perlin2D(Vector2f scale, Smoothness amount = Smoothness::Linear, Vector2i offset = Vector2i(), int seed = 12345, bool remapValues = true,
             Vector2i gradientWrapInterval = Vector2i(std::numeric_limits<int>().max(), std::numeric_limits<int>().max()))
        : Scale(scale), SmoothAmount(amount), Offset(offset), RandSeed(seed), GradientWrapInterval(gradientWrapInterval), RemapValues(remapValues) { }
    Perlin2D(float scale = 1.0f, Smoothness amount = Smoothness::Linear, Vector2i offset = Vector2i(), int seed = 12345, bool remapValues = true,
             Vector2i gradientWrapInterval = Vector2i(std::numeric_limits<int>().max(), std::numeric_limits<int>().max()))
        : Scale(scale, scale), SmoothAmount(amount), Offset(offset), RandSeed(seed), GradientWrapInterval(gradientWrapInterval), RemapValues(remapValues) { }

	virtual void Generate(Array2D<float> & outValues) const override;
};

//3D Perlin noise generator.
//Generates gradient values for each point on a grid, and interpolates between these gradients for each pixel of noise.
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
    //The scale of the noise (i.e. the number of noise pixels inside a single grid element along each axis).
    Vector3f Scale;
    //The offset of the noise (used to make different adjacent pieces of noise fit together without forcing the noise to be tileable).
    Vector3i Offset;
    //The generated noise will be tileable every "GradientWrapInterval.x" grid points along the X,
    //   every "GradientWrapInterval.y" grid points along the Y, and every "GradientWrapInterval.z" grid points along the Z.
    Vector3i GradientWrapInterval;
    //This generator will often generate dark values. If this flag is true, those values will be normalized.
    bool RemapValues;

    Perlin3D(float scale = 1.0f, Smoothness amount = Smoothness::Linear, Vector3i offset = Vector3i(), int seed = 12345, bool remapValues = true,
             Vector3i gradientWrapInterval = Vector3i(std::numeric_limits<int>().max(), std::numeric_limits<int>().max(), std::numeric_limits<int>().max()))
        : Scale(scale, scale, scale), SmoothAmount(amount), Offset(offset), RandSeed(seed), GradientWrapInterval(gradientWrapInterval), RemapValues(remapValues) { }
    Perlin3D(Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f), Smoothness amount = Smoothness::Linear, Vector3i offset = Vector3i(), int seed = 12345, bool remapValues = true,
             Vector3i gradientWrapInterval = Vector3i(std::numeric_limits<int>().max(), std::numeric_limits<int>().max(), std::numeric_limits<int>().max()))
        : Scale(scale), SmoothAmount(amount), Offset(offset), RandSeed(seed), GradientWrapInterval(gradientWrapInterval), RemapValues(remapValues) { }

    void Generate(Array3D<float> & outValues) const;
};