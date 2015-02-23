#include "RenderingState.h"

#include <assert.h>
#include "OpenGLIncludes.h"


GLenum RenderingState::ToEnum(AlphaTests test)
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
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}
void RenderingState::EnableDepthWriteState(void) const
{
    if (WriteToDepthBuffer)
    {
        glDepthMask(GL_TRUE);
    }
    else
    {
        glDepthMask(GL_FALSE);
    }
}

void RenderingState::EnableState(void) const
{
    EnableDepthTestState();
    EnableDepthWriteState();
    EnableCullState();
    EnableAlphaTestState();
}