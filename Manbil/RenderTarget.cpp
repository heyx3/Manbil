#include "RenderTarget.h"

#include <assert.h>

#include "Vertices.h"
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "RenderingState.h"




std::string RenderTargetSettings::ToString(void) const
{
    return std::string() + std::to_string(Width) + "x" + std::to_string(Height) +
          ", color texture: size " + ToString(ColTexSize) + ", depth texture: size " + ToString(DepthTexSize) + ", uses depth tex: " + std::to_string(UsesDepthTexture) +
          ", color texture mipmapping: " + std::to_string(ColTexSettings.GenerateMipmaps) + ", depth tex mipmapping: " + std::to_string(DepthTexSettings.GenerateMipmaps);
}



RenderTarget::RenderTarget(const RenderTargetSettings & _settings)
	: settings(_settings)
{
	ClearAllRenderingErrors();

    //Create frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //Set up the color texture.
    if (settings.GetUsesColorTexture())
    {
        glGenTextures(1, &colorTex);
        glBindTexture(GL_TEXTURE_2D, colorTex);

        //Set some parameters for the texture.
        settings.ColTexSettings.SetData();

        glTexImage2D(GL_TEXTURE_2D, 0, RenderTargetSettings::ToEnum(settings.ColTexSize),
                     settings.Width, settings.Height, 0, GL_RGBA, GL_FLOAT, 0);

        //Attach the texture to the frame buffer.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + settings.ColorAttachment, GL_TEXTURE_2D, colorTex, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + settings.ColorAttachment);
    }

    //Set up the depth texture.
    if (settings.UsesDepthTexture)
    {
        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);

        settings.DepthTexSettings.SetData();
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexImage2D(GL_TEXTURE_2D, 0, RenderTargetSettings::ToEnum(settings.DepthTexSize),
                     settings.Width, settings.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    }
    //Need SOME kind of depth testing so that the world looks OK.
    //Renderbuffers should be used in place of depth textures if the shader doesn't do anything with depth.
    else
    {
        glGenRenderbuffers(1, &depthTex);
        glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
        glRenderbufferStorage(GL_RENDERBUFFER, RenderTargetSettings::ToEnum(settings.DepthTexSize),
                              settings.Width, settings.Height);
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


	glViewport(0, 0, settings.Width, settings.Height);
}

bool RenderTarget::IsValid(void) const
{
	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	bool goodFBO = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);
    glViewport(0, 0, settings.Width, settings.Height); //TODO: Get rid of this line and make sure it doesn't break anything. Add a function somewhere that wraps this functionality.


	if (errorMsg.empty())
	{
		errorMsg = std::string(GetCurrentRenderingError());
	}

	return goodFBO && errorMsg.empty();
}


RenderTarget::~RenderTarget(void)
{
	glDeleteFramebuffers(1, &frameBuffer);
	if (settings.GetUsesColorTexture()) glDeleteTextures(1, &colorTex);

    //Either "depthTex" holds a depth texture, or it actually holds a depth renderbuffer.
    if (settings.UsesDepthTexture) glDeleteTextures(1, &depthTex);
    else glDeleteRenderbuffers(1, &depthTex);
}



void RenderTarget::EnableDrawingInto(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, settings.Width, settings.Height);
}
void RenderTarget::DisableDrawingInto(unsigned int w, unsigned int h) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
}