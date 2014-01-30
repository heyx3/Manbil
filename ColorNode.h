#pragma once

#include "Vectors.h"

//Represents a spot on the noise scale.
struct ColorNode
{
public:

	float Position;
	Vector4f LeftColor, RightColor;

	ColorNode(float pos, Vector4f leftCol, Vector4f rightCol) : Position(pos), LeftColor(leftCol), RightColor(rightCol) { }
	ColorNode(float pos, Vector4f col) : Position(pos), LeftColor(col), RightColor(col) { }
	~ColorNode(void) { }
};