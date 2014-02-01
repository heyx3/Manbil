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

    bool GenerateMipmaps;


	TextureSettings(TextureFiltering filter = TextureFiltering::TF_NEAREST, TextureWrapping wrap = TextureWrapping::TW_WRAP, bool genMipmaps = true)
        : MagFilter(filter), MinFilter(filter), HorWrap(wrap), VertWrap(wrap), GenerateMipmaps(genMipmaps)
	{

	}
	TextureSettings(TextureFiltering mag, TextureFiltering min, TextureWrapping hor, TextureWrapping vert, bool genMipmaps = true)
		: MagFilter(mag), MinFilter(min), HorWrap(hor), VertWrap(vert), GenerateMipmaps(genMipmaps)
	{

	}


	void SetData(BufferObjHandle tex) const;


private:

	static GLint ToGLInt(TextureFiltering tf, bool minFilter) { return (tf == TextureFiltering::TF_LINEAR) ? (minFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : GL_NEAREST; }
	static GLint ToGLInt(TextureWrapping twa) { return (twa == TextureWrapping::TW_CLAMP) ? GL_CLAMP_TO_EDGE : GL_REPEAT; }
};