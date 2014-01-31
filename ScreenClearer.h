#pragma once

#include "Math/Vectors.h"

//Clears the screen.
class ScreenClearer
{
public:

	bool ClearColor, ClearDepth, ClearStencil;

	Vector4f ClearedColor;
	float ClearedDepth;
	int ClearedStencil;

	ScreenClearer(bool clearColor = true, bool clearDepth = true, bool clearStencil = false,
				  Vector4f clearedColor = Vector4f(0.0f, 0.0f, 0.0f, 0.0f), float clearedDepth = 1.0f, int clearedStencil = 0)
				  : ClearColor(clearColor), ClearDepth(clearDepth), ClearStencil(clearStencil),
				    ClearedColor(clearedColor), ClearedDepth(clearedDepth), ClearedStencil(clearedStencil)
	{

	}

	void ClearScreen(void) const;
};