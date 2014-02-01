#pragma once

#include "OpenGLIncludes.h"

//Information about OpenGL rendering.
class RenderingState
{
public:

	enum Cullables
	{
		C_FRONT,
		C_BACK,
		C_FRONT_AND_BACK,
		C_NONE,
	};
	Cullables ToCull;

    enum BlendingExpressions
    {
        Zero,
        One,

        SourceColor,
        DestColor,

        OneMinusSourceColor,
        OneMinusDestColor,

        SourceAlpha,
        DestAlpha,

        OneMinusSourceAlpha,
        OneMinusDestAlpha,

        SourceAlphaSaturate,
    };
    //These values will only be applicable if "UseBlending" is true. They represent how the source is blended with the current framebuffer data.
    BlendingExpressions SourceBlend, DestBlend;

	bool UseDepthTesting, UseBlending, UseTextures;


	RenderingState(bool useDepthTesting = true, bool useBlending = false, bool useTextures = true,
                   Cullables toCull = Cullables::C_NONE,
                   BlendingExpressions source = BlendingExpressions::SourceAlpha,
                   BlendingExpressions dest = BlendingExpressions::OneMinusSourceAlpha)
        : UseDepthTesting(useDepthTesting), UseBlending(useBlending), UseTextures(useTextures),
          ToCull(toCull), SourceBlend(source), DestBlend(dest)
	{

	}
    RenderingState(const RenderingState & cpy)
        : UseDepthTesting(cpy.UseDepthTesting), UseBlending(cpy.UseBlending), UseTextures(cpy.UseTextures),
        ToCull(cpy.ToCull), SourceBlend(cpy.SourceBlend), DestBlend(cpy.DestBlend)
    {

    }


	//Sets the rendering API to use this state.
	void EnableState(void) const;

private:

    static GLenum ToEnum(BlendingExpressions expression)
    {
        switch (expression)
        {
            case BlendingExpressions::Zero: return GL_ZERO;
            case BlendingExpressions::One: return GL_ONE;
               
            case BlendingExpressions::SourceColor: return GL_SRC_COLOR;
            case BlendingExpressions::DestColor: return GL_DST_COLOR;

            case BlendingExpressions::OneMinusSourceColor: return GL_ONE_MINUS_SRC_COLOR;
            case BlendingExpressions::OneMinusDestColor: return GL_ONE_MINUS_DST_COLOR;

            case BlendingExpressions::SourceAlpha: return GL_SRC_ALPHA;
            case BlendingExpressions::DestAlpha: return GL_DST_ALPHA;

            case BlendingExpressions::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case BlendingExpressions::OneMinusDestAlpha: return GL_ONE_MINUS_SRC_COLOR;

            case BlendingExpressions::SourceAlphaSaturate: return GL_SRC_ALPHA_SATURATE;
        }
    }
};