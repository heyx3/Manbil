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
				Exception_FloatNotInRange,
				Exception_NodesNotOrdered;

	std::vector<ColorNode> OrderedNodes;

	ColorGradient() { }
	~ColorGradient(void) {  }

	void GetColors(Fake2DArray<Vector4f> & outColors, const Fake2DArray<float> & noiseValues) const;
	Vector4f GetColor(float value) const { CheckErrors(); return GetColorWOErrorChecking(value); }

private:

	void CheckErrors(void) const;
	Vector4f GetColorWOErrorChecking(float value) const;
};