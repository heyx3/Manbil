#pragma once

//Information about OpenGL rendering.
class RenderingState
{
public:

    //TODO: Add functionality for different kinds of blending, so that multiple render passes will actually be useful.

	enum Cullables
	{
		C_FRONT,
		C_BACK,
		C_FRONT_AND_BACK,
		C_NONE,
	};
	Cullables ToCull;

	bool UseDepthTesting, UseBlending, UseTextures;


	RenderingState(bool useDepthTesting = true, bool useBlending = false, bool useTextures = true, Cullables toCull = Cullables::C_NONE)
				   : ToCull(toCull), UseDepthTesting(useDepthTesting), UseBlending(useBlending), UseTextures(useTextures)
	{

	}


	//Sets the rendering API to use this state.
	void EnableState(void) const;
};