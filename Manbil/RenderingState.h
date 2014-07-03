#pragma once

#include "OpenGLIncludes.h"

//Information about OpenGL rendering.
struct RenderingState
{
public:

    //Surfaces that can be culled.
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
        BE_ZERO,
        BE_ONE,

        //Fragment color.
        BE_SOURCE_COLOR,
        //Back buffer color.
        BE_DEST_COLOR,

        //Inverse of fragment color.
        BE_ONE_MINUS_SOURCE_COLOR,
        //Inverse of back buffer color.
        BE_ONE_MINUS_DEST_COLOR,

        //Fragment alpha.
        BE_SOURCE_ALPHA,
        //Back buffer alpha.
        BE_DESTALPHA,

        //Inverse of fragment alpha.
        BE_ONE_MINUS_SOURCE_ALPHA,
        //Inverse of back buffer alpha.
        BE_ONE_MINUS_DEST_ALPHA,

        BE_SOURCE_ALHPA_SATURATE,
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

    //Whether to use depth testing.
	bool UseDepthTesting;
    //Whether to write new fragments to the depth buffer.
    bool WriteToDepthBuffer;
    //Whether to use blending (for objects that are partially see-through).
    bool UseBlending;
    //Whether to enable the use of textures.
    //TODO: This is unnecessary; it was part of the fixed-function pipeline. Remove it.
    bool UseTextures;

    //Rendering state that uses blending.
    RenderingState(Cullables toCull,
                   BlendingExpressions source = BlendingExpressions::BE_SOURCE_ALPHA,
                   BlendingExpressions dest = BlendingExpressions::BE_ONE_MINUS_SOURCE_ALPHA,
                   bool useDepthTesting = true, bool writeDepthBuffer = true,
                   AlphaTests test = AlphaTests::AT_ALWAYS, float testValue = 0.0f,
                   bool useTextures = true)
        : UseDepthTesting(useDepthTesting), WriteToDepthBuffer(writeDepthBuffer),
          UseBlending(true),
          UseTextures(useTextures),
          ToCull(toCull),
          SourceBlend(source), DestBlend(dest),
          AlphaTest(test), AlphaTestValue(testValue)
    {

    }
    //Rendering state that doesn't use blending.
    RenderingState(bool useDepthTesting = true, bool writeDepthBuffer = true,
                   Cullables toCull = Cullables::C_NONE,
                   AlphaTests test = AlphaTests::AT_ALWAYS, float testValue = 0.0f,
                   bool useTextures = true)
        : UseDepthTesting(useDepthTesting), WriteToDepthBuffer(writeDepthBuffer),
          UseBlending(false),
          UseTextures(useTextures),
          ToCull(toCull),
          SourceBlend(BlendingExpressions::BE_SOURCE_ALPHA),
          DestBlend(BlendingExpressions::BE_ONE_MINUS_SOURCE_ALPHA),
          AlphaTest(test), AlphaTestValue(testValue)
    {

    }


    //Gets whether or not this state uses alpha testing.
    bool UsesAlphaTesting(void) const { return AlphaTest != AlphaTests::AT_ALWAYS; }


    //Sets OpenGL to use this instance's culling state.
    void EnableCullState(void) const;
    //Sets OpenGL to use this instance's blending state.
    void EnableBlendState(void) const;
    //Sets OpenGL to use this instance's alpha test state.
    void EnableAlphaTestState(void) const;
    //Sets OpenGL to use this instance's depth testing state.
    void EnableDepthTestState(void) const;
    //Sets OpenGL to use this instance's depth writing state.
    void EnableDepthWriteState(void) const;
    //Sets OpenGL to use this instance's texture use state.
    void EnableTexturesState(void) const;


	//Sets OpenGL to use this state.
	void EnableState(void) const;


private:

    static GLenum ToEnum(BlendingExpressions expression)
    {
        switch (expression)
        {
            case BlendingExpressions::BE_ZERO: return GL_ZERO;
            case BlendingExpressions::BE_ONE: return GL_ONE;
               
            case BlendingExpressions::BE_SOURCE_COLOR: return GL_SRC_COLOR;
            case BlendingExpressions::BE_DEST_COLOR: return GL_DST_COLOR;

            case BlendingExpressions::BE_ONE_MINUS_SOURCE_COLOR: return GL_ONE_MINUS_SRC_COLOR;
            case BlendingExpressions::BE_ONE_MINUS_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;

            case BlendingExpressions::BE_SOURCE_ALPHA: return GL_SRC_ALPHA;
            case BlendingExpressions::BE_DESTALPHA: return GL_DST_ALPHA;

            case BlendingExpressions::BE_ONE_MINUS_SOURCE_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
            case BlendingExpressions::BE_ONE_MINUS_DEST_ALPHA: return GL_ONE_MINUS_SRC_COLOR;

            case BlendingExpressions::BE_SOURCE_ALHPA_SATURATE: return GL_SRC_ALPHA_SATURATE;

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