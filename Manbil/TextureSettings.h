#pragma once

#include "OpenGLIncludes.h"
#include "RenderDataHandler.h"


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


    //Uses the currently-bound texture.
    static void SetWrappingData(TextureWrapping horizontal, TextureWrapping vertical);
    //Uses the currently-bound texture.
    static void SetWrappingData(TextureWrapping wrap) { SetWrappingData(wrap, wrap); }
    //Uses the currently-bound texture.
    static void SetFilteringData(TextureFiltering minFilter, TextureFiltering magFilter, bool usesMipmaps);
    //Uses the currently-bound texture.
    static void SetFilteringData(TextureFiltering filter, bool usesMipmaps) { SetFilteringData(filter, filter, usesMipmaps); }
    //Uses the currently-bound texture.
    static void GenMipmaps(void);

    //Uses the currently-bound texture.
    void SetData(void) const;
    //Uses the currently-bound texture.
    void SetWrappingData(void) const { SetWrappingData(HorWrap, VertWrap); }
    //Uses the currently-bound texture.
    void SetFilteringData(void) const { SetFilteringData(MinFilter, MagFilter, GenerateMipmaps); }
    //Uses the currently-bound texture.
    void SetMipmaps(void) const { if (GenerateMipmaps) GenMipmaps(); }

    void SetData(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetData(); }
    void SetWrappingData(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetWrappingData(); }
    void SetFilteringData(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetFilteringData(); }
    void SetMipmaps(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetMipmaps(); }


private:

	static GLint ToGLInt(TextureFiltering tf, bool minFilter, bool genMips) { return (tf == TextureFiltering::TF_LINEAR) ? ((minFilter && genMips) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : GL_NEAREST; }
	static GLint ToGLInt(TextureWrapping twa) { return (twa == TextureWrapping::TW_CLAMP) ? GL_CLAMP_TO_EDGE : GL_REPEAT; }
};