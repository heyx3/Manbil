#include "ScreenClearer.h"

#include "OpenGLIncludes.h"

void ScreenClearer::ClearScreen(void) const
{
	if (ClearColor)
	{
		glClearColor(ClearedColor.x, ClearedColor.y, ClearedColor.z, ClearedColor.w);
	}
	if (ClearDepth)
	{
		glClearDepthf(ClearedDepth);
	}
	if (ClearStencil)
	{
		glClearStencil(ClearedStencil);
	}

	GLbitfield mask = 0;
	if (ClearColor)
    {
        mask |= GL_COLOR_BUFFER_BIT;
    }
	if (ClearDepth)
    {
        mask |= GL_DEPTH_BUFFER_BIT;
    }
	if (ClearStencil)
    {
        mask |= GL_STENCIL_BUFFER_BIT;
    }

	glClear(mask);
}