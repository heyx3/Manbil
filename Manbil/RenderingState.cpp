#include "RenderingState.h"

#include <assert.h>
#include "OpenGLIncludes.h"


void RenderingState::EnableCullState(void) const
{
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
}
void RenderingState::EnableBlendState(void) const
{
    if (UseBlending)
    {
        glEnable(GL_BLEND);
        glBlendFunc(ToEnum(SourceBlend), ToEnum(DestBlend));
    }
    else glDisable(GL_BLEND);
}
void RenderingState::EnableAlphaTestState(void) const
{
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
void RenderingState::EnableDepthTestState(void) const
{
    if (UseDepthTesting)
        glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}
void RenderingState::EnableDepthWriteState(void) const
{
    if (WriteToDepthBuffer)
        glDepthMask(GL_TRUE);
    else glDepthMask(GL_FALSE);
}
void RenderingState::EnableTexturesState(void) const
{
    if (UseTextures)
        glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
}

void RenderingState::EnableState(void) const
{
    EnableDepthTestState();
    EnableDepthWriteState();
    EnableBlendState();
    EnableTexturesState();
    EnableCullState();
    EnableAlphaTestState();
}