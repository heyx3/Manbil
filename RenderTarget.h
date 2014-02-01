#pragma once

#include "OpenGLIncludes.h"
#include "Math/Vectors.h"
#include <string>

//Allows drawing into a texture.
class RenderTarget
{
public:

    //Takes in the size of the target.
    RenderTarget(unsigned int width, unsigned int height, bool useColor, bool useDepth);
	RenderTarget(void); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.
	~RenderTarget(void);

	void operator=(const RenderTarget & other); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.


    bool HasError(void) const { return !errorMsg.empty(); }
	const std::string & GetErrorMessage(void) const { return errorMsg; }
	void ClearErrorMessage(void) { errorMsg.clear(); }


    BufferObjHandle GetColorTexture(void) const { return colorTex; }
    BufferObjHandle GetDepthTexture(void) const { return depthTex; }

    BufferObjHandle GetFramebuffer(void) const { return frameBuffer; }

    Vector2i GetSize(void) const { return Vector2i(width, height); }

	//Resizes this render target.
	void ChangeSize(int newWidth, int newHeight);

	//Gets whether this render target is ready to be used.
	bool IsValid(void) const;

	//Sets up this render target so that any rendering will go into this render target.
	void EnableDrawingInto(void) const;

	//Sets up this render target so that any rendering will go into the default frame buffer (a.k.a. the window).
    //Takes in the size of the back buffer to use.
	void DisableDrawingInto(unsigned int width, unsigned int height) const;

private:

	unsigned int width, height;

    bool usesCol, usesDepth;
	BufferObjHandle frameBuffer, colorTex, depthTex;

	mutable std::string errorMsg;
};