#pragma once

#include "../Math/Noise Generation/ColorGradient.h"
#include "../Math/Noise Generation/BasicGenerators.h"

//Converts a 2D noise field to a texture.
class NoiseToTexture
{
public:

	const ColorGradient* GradientToUse;
	const Noise2D* NoiseToUse;

	NoiseToTexture(ColorGradient* gradient = 0, Noise2D* noise = 0)
        : GradientToUse(gradient), NoiseToUse(noise) { }

	void GetImage(Array2D<Vector4b>& outImage) const;
    void GetImage(Array2D<Vector4f>& outImage) const;
};