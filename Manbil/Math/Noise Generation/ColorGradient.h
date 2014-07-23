#pragma once

#include <vector>
#include "ColorNode.h"
#include "../LowerMath.hpp"

//Maps an interval of floats to colors.
class ColorGradient
{
public:

	static char Exception_NoStartingNode,
				Exception_NoEndingNode,
				Exception_NodesNotOrdered;

	std::vector<ColorNode> OrderedNodes;

	ColorGradient() { }
	~ColorGradient(void) {  }

	void GetColors(Vector4f * outColors, const float * noiseValues, unsigned int numbElements) const;
	Vector4f GetColor(float value) const { CheckErrors(); return GetColorWOErrorChecking(value); }

private:

	void CheckErrors(void) const;
	Vector4f GetColorWOErrorChecking(float value) const;
};