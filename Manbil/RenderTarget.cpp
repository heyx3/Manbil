#include "RenderTarget.h"

#include <assert.h>

#include "Vertices.h"
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "RenderingState.h"


RenderTarget::RenderTarget(unsigned int w, unsigned int h, bool useColor, bool useDepth)
	: width(w), height(h)
{

    //TODO: Take the below TODOs and create a single struct containing all data pertaining to a RenderTarget.
    //TODO: parameterize the size of the color/depth buffers. Extend this parameterization to RenderTargetManager.
    //TODO: parameterize which color attachment the color texture uses.
    //TODO :Parameterize the texture settings (filtering, wrapping, mipmaps).

	ClearAllRenderingErrors();

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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);

        //Attach the texture to the frame buffer.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    //Set up the depth texture.
    if (usesDepth)
    {
        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
        errorMsg = "Framebuffer is not ready! Error ";
        errorMsg += std::to_string(stat);
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


	glViewport(0, 0, width, height);
}

bool RenderTarget::IsValid(void) const
{
	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	bool goodFBO = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);
    glViewport(0, 0, width, height); //TODO: Get back buffer size and use that instead of frame buffer size.


	if (errorMsg.empty())
	{
		errorMsg = std::string(GetCurrentRenderingError());
	}

	return goodFBO && errorMsg.empty();
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