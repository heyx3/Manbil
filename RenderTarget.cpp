#include "RenderTarget.h"

#include <assert.h>

#include "Vertices.h"
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "RenderingState.h"


RenderTarget::RenderTarget(unsigned int w, unsigned int h, bool useColor, bool useDepth)
	: width(w), height(h)
{
	ClearAllRenderingErrors();


    #pragma region Create frame buffer

    //Create frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //Set up the color texture.
    if (usesCol)
    {
        glGenTextures(1, &colorTex);
        glBindTexture(GL_TEXTURE_2D, colorTex);

        //Set some parameters for the texture.
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        //Attach the texture to the frame buffer.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    //Set up the depth texture.
    if (usesDepth)
    {
        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    }
    //Need SOME kind of depth testing so that the world looks OK.
    //Renderbuffers should be used in place of depth textures if the shader doesn't do anything with depth.
    else
    {
        glGenRenderbuffers(1, &depthTex);
        glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTex);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Check framebuffer status.
    RenderDataHandler::FrameBufferStatus stat = RenderDataHandler::GetFramebufferStatus(frameBuffer);
    if (stat != RenderDataHandler::FrameBufferStatus::EVERYTHING_IS_FINE)
    {
        errorMsg = "Framebuffer is not ready!";
        return;
    }


    //Check for any errors with setting up the frame buffer.
    std::string errIntro = std::string("Error setting up frame buffer object: ");
    errorMsg = GetCurrentRenderingError();
    ClearAllRenderingErrors();
    if (errorMsg.compare("") != 0)
    {
        errorMsg = errIntro + errorMsg;
        return;
    }

    #pragma endregion


	glViewport(0, 0, width, height);
}

void RenderTarget::ChangeSize(int newW, int newH)
{
	width = newW;
	height = newH;
	

    //Recreate the color texture and set properties.
    if (usesCol)
    {
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newW, newH, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
	
    //Recreate the depth texture and set properties.
    if (usesDepth)
    {
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    }
    else
    {
        glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    }


	//Attach the objects to the frame buffer.

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);

    if (usesCol)
    {
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    }
    if (usesDepth)
    {
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    }
    else
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTex);
    }


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    std::string str = GetCurrentRenderingError();
    if (!str.empty())
    {
        errorMsg = "Error resizing render target: ";
        errorMsg += str;
    }
}

bool RenderTarget::IsValid(void) const
{
	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	bool goodFBO = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);
    glViewport(0, 0, width, height);


	if (errorMsg.empty())
	{
		errorMsg = std::string(GetCurrentRenderingError());
	}

	return goodFBO && errorMsg.compare("") == 0;
}


RenderTarget::~RenderTarget(void)
{
	glDeleteFramebuffers(1, &frameBuffer);
	if (usesCol) glDeleteTextures(1, &colorTex);

    //Either "depthTex" holds a depth texture, or it actually holds a depth renderbuffer.
    if (usesDepth) glDeleteTextures(1, &depthTex);
    else glDeleteRenderbuffers(1, &depthTex);
}



void RenderTarget::EnableDrawingInto(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
}
void RenderTarget::DisableDrawingInto(unsigned int w, unsigned int h) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
}