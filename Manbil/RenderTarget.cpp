#include "RenderTarget.h"

#include <assert.h>

#include "Vertices.h"
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "RenderingState.h"


RenderTarget::RenderTarget(const std::vector<RendTargetColorTexSettings> & colTexSettings, const RendTargetDepthTexSettings & depthTexSettings)
    : colTexesSetts(colTexSettings), depthTexSetts(depthTexSettings)
{
	ClearAllRenderingErrors();

    //Make sure there aren't too many color attachments for the hardware to handle.
    int maxColors;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColors);
    if (maxColors < colTexSettings.size())
    {
        errorMsg = std::string() + "You are limited to " + std::to_string(maxColors) +
                   " color textures per frame buffer, but you tried to attach " + std::to_string(colTexSettings.size());
        return;
    }


    //Create frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //Set up the color textures.
    std::vector<GLenum> colAttachments;
    unsigned int fbWidth = 1, fbHeight = 1;
    for (unsigned int colTexIndex = 0; colTexIndex < colTexSettings.size(); ++colTexIndex)
    {
        RenderObjHandle colTx;
        const RendTargetColorTexSettings & sett = colTexSettings[colTexIndex];

        fbWidth = BasicMath::Max(fbWidth, sett.Settings.Width);
        fbHeight = BasicMath::Max(fbHeight, sett.Settings.Height);

        colAttachments.insert(colAttachments.end(), GL_COLOR_ATTACHMENT0 + sett.ColorAttachment);

        glGenTextures(1, &colTx);
        glBindTexture(GL_TEXTURE_2D, colTx);

        //Set some parameters for the texture.
        sett.Settings.Settings.SetData();

        glTexImage2D(GL_TEXTURE_2D, 0, ColorTextureSettings::ToInternalFormat(sett.Settings.Size),
                     sett.Settings.Width, sett.Settings.Height, 0, ColorTextureSettings::ToFormat(sett.Settings.Size), GL_FLOAT, 0);

        //Attach the texture to the frame buffer.
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + sett.ColorAttachment, GL_TEXTURE_2D, colTx, 0);

        //Add the color texture to this RenderTarget.
        colorTexes.insert(colorTexes.end(), colTx);
    }
    glDrawBuffers(colAttachments.size(), colAttachments.data());

    //Set the depth texture size to encompass all color textures.
    depthTexSetts.Settings.Width = fbWidth;
    depthTexSetts.Settings.Height = fbHeight;

    //Set up the depth texture.
    if (depthTexSettings.UsesDepthTexture)
    {
        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);

        depthTexSettings.Settings.Settings.SetData();
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexImage2D(GL_TEXTURE_2D, 0, DepthTextureSettings::ToEnum(depthTexSettings.Settings.Size),
                     fbWidth, fbHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    }
    //Need SOME kind of depth testing so that the world looks OK.
    //Renderbuffers should be used in place of depth textures if the shader doesn't do anything with depth.
    else
    {
        glGenRenderbuffers(1, &depthTex);
        glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
        glRenderbufferStorage(GL_RENDERBUFFER, DepthTextureSettings::ToEnum(depthTexSettings.Settings.Size),
                              fbWidth, fbHeight);
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
    errorMsg = GetCurrentRenderingError();
    ClearAllRenderingErrors();
    if (errorMsg.compare("") != 0)
    {
        errorMsg = std::string("Error setting up frame buffer object: ") + errorMsg;
        return;
    }


    //TODO: Remove this; make sure it doesn't break any demos/post-processing stuff.
	glViewport(0, 0, fbWidth, fbHeight);
}

bool RenderTarget::IsValid(void) const
{
    //Make sure there aren't too many color attachments for the hardware to handle.
    int maxColors;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColors);
    if (maxColors < colorTexes.size())
    {
        errorMsg = std::string() + "You are limited to " + std::to_string(maxColors) +
            " color textures per frame buffer, but you tried to attach " + std::to_string(colorTexes.size());
        return false;
    }


	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	bool goodFBO = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);
    glViewport(0, 0, depthTexSetts.Settings.Width, depthTexSetts.Settings.Height); //TODO: Get rid of this line and make sure it doesn't break anything. Add a function somewhere that wraps this functionality.


	if (errorMsg.empty())
	{
		errorMsg = std::string(GetCurrentRenderingError());
	}

	return goodFBO && errorMsg.empty();
}


RenderTarget::~RenderTarget(void)
{
	glDeleteFramebuffers(1, &frameBuffer);

    glDeleteTextures(colorTexes.size(), colorTexes.data());

    //Either "depthTex" holds a depth texture, or it actually holds a depth renderbuffer.
    if (depthTexSetts.UsesDepthTexture) glDeleteTextures(1, &depthTex);
    else glDeleteRenderbuffers(1, &depthTex);
}



void RenderTarget::EnableDrawingInto(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, depthTexSetts.Settings.Width, depthTexSetts.Settings.Height);
}
void RenderTarget::DisableDrawingInto(unsigned int w, unsigned int h) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
}