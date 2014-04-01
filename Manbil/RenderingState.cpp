#include "RenderingState.h"

#include <assert.h>

#include "OpenGLIncludes.h"

void RenderingState::EnableState(void) const
{
	if (UseDepthTesting)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
	}

	if (UseBlending)
	{
		glEnable(GL_BLEND);
        glBlendFunc(ToEnum(SourceBlend), ToEnum(DestBlend));
	}
	else
	{
		glDisable(GL_BLEND);
	}

	if (UseTextures)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}

	switch (ToCull)
	{
		case Cullables::C_NONE:
			glDisable(GL_CULL_FACE);
			break;
		case Cullables::C_FRONT:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case Cullables::C_BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case Cullables::C_FRONT_AND_BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
			break;

		default: assert(false);
	}

    if (UsesAlphaTesting())
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(ToEnum(AlphaTest), AlphaTestValue);
    }
    else
    {
        glDisable(GL_ALPHA_TEST);
    }
}