#pragma once

#include "OpenGLIncludes.h"

//Information about OpenGL rendering.
struct RenderingState
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

    //How to test whether a fragment should be ignored based on its alpha.
    enum AlphaTests
    {
        //Never use the fragment.
        AT_NEVER,
        //Always use the fragment.
        AT_ALWAYS,

        //Use the fragment if it's alpha is not equal to "AlphaTestValue".
        AT_EQUAL,
        //Use the fragment if it's alpha is equal to "AlphaTestValue".
        AT_NOT_EQUAL,

        //Use the fragment if it's alpha is less than "AlphaTestValue".
        AT_LESS,
        //Use the fragment if it's alpha is greater than "AlphaTestValue".
        AT_GREATER,
        //Use the fragment if it's alpha is less than or equal to "AlphaTestValue".
        AT_LESS_OR_EQUAL,
        //Use the fragment if it's alpha is greater than or equal to "AlphaTestValue".
        AT_GREATER_OR_EQUAL,
    };
    //The type of alpha testing to use.
    AlphaTests AlphaTest;
    //The value used for certain kinds of alpha tests.
    float AlphaTestValue;


	bool UseDepthTesting, UseBlending, UseTextures;


    RenderingState(Cullables toCull,
                   BlendingExpressions source = BlendingExpressions::SourceAlpha,
                   BlendingExpressions dest = BlendingExpressions::OneMinusSourceAlpha,
                   bool useDepthTesting = true,
                   AlphaTests test = AlphaTests::AT_ALWAYS, float testValue = 0.0f,
                   bool useTextures = true)
        : UseDepthTesting(useDepthTesting), UseBlending(true), UseTextures(useTextures),
          ToCull(toCull), SourceBlend(source), DestBlend(dest),
          AlphaTest(test), AlphaTestValue(testValue)
    {

    }
    RenderingState(bool useDepthTesting = true, Cullables toCull = Cullables::C_NONE, AlphaTests test = AlphaTests::AT_ALWAYS, float testValue = 0.0f, bool useTextures = true)
        : UseDepthTesting(useDepthTesting), UseBlending(false), UseTextures(useTextures),
          ToCull(toCull), SourceBlend(BlendingExpressions::SourceAlpha), DestBlend(BlendingExpressions::OneMinusSourceAlpha),
          AlphaTest(test), AlphaTestValue(testValue)
    {

    }


    //Gets whether or not this state uses alpha testing.
    bool UsesAlphaTesting(void) const { return AlphaTest != AlphaTests::AT_ALWAYS; }


	//Sets OpenGL to use this state.
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

            default: return GL_INVALID_ENUM;
        }
    }
    static GLenum ToEnum(AlphaTests test)
    {
        switch (test)
        {
            case AlphaTests::AT_ALWAYS: return GL_ALWAYS;
            case AlphaTests::AT_NEVER: return GL_NEVER;

            case AlphaTests::AT_EQUAL: return GL_EQUAL;
            case AlphaTests::AT_NOT_EQUAL: return GL_NOTEQUAL;

            case AlphaTests::AT_GREATER: return GL_GREATER;
            case AlphaTests::AT_GREATER_OR_EQUAL: return GL_GEQUAL;

            case AlphaTests::AT_LESS: return GL_LESS;
            case AlphaTests::AT_LESS_OR_EQUAL: return GL_LEQUAL;

            default: return GL_INVALID_ENUM;
        }
    }
};