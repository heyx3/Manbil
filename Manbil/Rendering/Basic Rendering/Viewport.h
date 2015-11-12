#pragma once

#include "OpenGLIncludes.h"


//Defines a sub-section of the framebuffer being rendered into.
//Viewport scissor functionality must be enabled with "EnableScissor" before using "SetAsScissor()".
//Note that viewports and scissors are not tied to framebuffers; they are a global state.
struct Viewport
{
public:

    //Enables viewport scissor.
    static void EnableScissor(void) { glEnable(GL_SCISSOR_TEST); }
    //Disables viewport scissor (saves performance when not using it).
    static void DisableScissor(void) { glDisable(GL_SCISSOR_TEST); }


    unsigned int XMin, YMin,
                 XSize, YSize;


    Viewport(unsigned int xMin = 0, unsigned int yMin = 0,
             unsigned int xSize = 0, unsigned int ySize = 0)
        : XMin(xMin), YMin(yMin), XSize(xSize), YSize(ySize) { }


    //Sets OpenGL to map the screen coordinates range [-1, 1] to this region on the framebuffer.
    void SetAsViewport(void) const { glViewport(XMin, YMin, XSize, YSize); }
    //Sets OpenGL to only allow draw calls affect this range in the framebuffer.
    void SetAsScissor(void) const { glScissor(XMin, YMin, XSize, YSize); }

    //Sets OpenGL to map screen coordinates to this region on the framebuffer
    //    and only allow draw calls inside of it.
    void Use(void) const { SetAsViewport(); SetAsScissor(); }
};