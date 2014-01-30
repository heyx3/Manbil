#pragma once

#include "OpenGLIncludes.h"
#include <string>

//Allows drawing into a texture.
class RenderTarget
{
public:

	unsigned int TextureSlot;

	RenderTarget(int width, int height);
	RenderTarget(void); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.
	~RenderTarget(void);

	void operator=(const RenderTarget & other); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.

	//Gets the most recent error this render target created, or the empty string if there is no error.
	const std::string & GetErrorMessage(void) const { return errorMsg; }
	//Removes the current error message.
	void RemoveErrorMessage(void) { errorMsg.clear(); }

	//Resizes this render target.
	void ChangeSize(int newWidth, int newHeight);

	//Gets whether this render target is ready to be used.
	bool IsValid(void) const;

	//Sets up this render target so that any rendering will go into this render target.
	void EnableDrawingInto(void);

	//Sets up this render target so that any rendering will go into the default frame buffer (a.k.a. the window).
	void DisableDrawingInto(void);

	//Draws this render target to the current frame buffer.
	void Draw(void);

private:

	unsigned int width, height;

	BufferObjHandle shaderProg, vShader, fShader,
					frameBuffer, colorTex,
					vbo, ibo;
	UniformLocation colorTexLoc;
	GLuint depthBuff;

	mutable std::string errorMsg;
};