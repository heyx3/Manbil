#pragma once

#include "OpenGLIncludes.h"


//Handles manipulation of OpenGL textures.
class TextureSettings
{
public:

	enum TextureFiltering
	{
		TF_NEAREST,
		TF_LINEAR,
	};
	enum TextureWrapping
	{
		TW_WRAP,
		TW_CLAMP,
	};

	TextureFiltering MagFilter, MinFilter;
	TextureWrapping HorWrap, VertWrap;


	TextureSettings(TextureFiltering filter = TextureFiltering::TF_NEAREST, TextureWrapping wrap = TextureWrapping::TW_WRAP)
		: MagFilter(filter), MinFilter(filter), HorWrap(wrap), VertWrap(wrap)
	{

	}
	TextureSettings(TextureFiltering mag, TextureFiltering min, TextureWrapping hor, TextureWrapping vert)
					: MagFilter(mag), MinFilter(min), HorWrap(hor), VertWrap(vert)
	{

	}


	void SetData(BufferObjHandle tex) const;


private:

	static GLint ToGLInt(TextureFiltering tf) { return (tf == TextureFiltering::TF_LINEAR) ? GL_LINEAR : GL_NEAREST; }
	static GLint ToGLInt(TextureWrapping twa) { return (twa == TextureWrapping::TW_CLAMP) ? GL_CLAMP_TO_EDGE : GL_REPEAT; }
};