#pragma once

#include "OpenGLIncludes.h"
#include "BlendMode.h"


//Information about OpenGL rendering.
struct RenderingState
{
public:

    //Surfaces that can be culled.
	enum Cullables : unsigned char
	{
		C_FRONT,
		C_BACK,
		C_FRONT_AND_BACK,
		C_NONE,
	};
    //How to test whether a fragment should be ignored based on its alpha.
    enum AlphaTests : unsigned char
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


    //Which faces to cull.
	Cullables ToCull;
    //The type of alpha testing to use.
    AlphaTests AlphaTest;
    //The value used for certain kinds of alpha tests.
    float AlphaTestValue;

    //Whether to use depth testing.
	bool UseDepthTesting;
    //Whether to write new fragments to the depth buffer.
    bool WriteToDepthBuffer;


    RenderingState(Cullables toCull,
                   bool useDepthTesting = true, bool writeDepthBuffer = true,
                   AlphaTests test = AlphaTests::AT_ALWAYS, float testValue = 0.0f)
        : UseDepthTesting(useDepthTesting), WriteToDepthBuffer(writeDepthBuffer),
          ToCull(toCull), AlphaTest(test), AlphaTestValue(testValue)
    {

    }


    //Gets whether or not this state uses alpha testing.
    bool UsesAlphaTesting(void) const { return AlphaTest != AlphaTests::AT_ALWAYS; }

    //Sets OpenGL to use this instance's culling state.
    void EnableCullState(void) const;
    //Sets OpenGL to use this instance's alpha test state.
    void EnableAlphaTestState(void) const;
    //Sets OpenGL to use this instance's depth testing state.
    void EnableDepthTestState(void) const;
    //Sets OpenGL to use this instance's depth writing state.
    void EnableDepthWriteState(void) const;

	//Sets OpenGL to use this state.
	void EnableState(void) const;


private:

    static GLenum ToEnum(AlphaTests test);
};