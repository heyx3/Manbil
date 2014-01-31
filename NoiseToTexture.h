#pragma once

#include "Math/Noise Generation/ColorGradient.h"
#include "SFML\Graphics\Image.hpp"
#include "Math/Noise Generation/BasicGenerators.h"

//Converts a 2D noise field to an SFML Image.
class NoiseToTexture
{
public:

	const ColorGradient * GradientToUse;
	const Noise2D * NoiseToUse;

	NoiseToTexture(ColorGradient * gradient = 0, Noise2D * noise = 0) : GradientToUse(gradient), NoiseToUse(noise) { }

	//Returns whether or not it was successful.
	bool GetImage(sf::Image & outImage) const;
};