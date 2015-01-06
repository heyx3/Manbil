#include "RenderTarget.h"

#include <assert.h>

#include "Vertices.h"
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "RenderingState.h"
#include "DebugAssist.h"



unsigned int RenderTarget::maxColorAttachments = 0,
             RenderTarget::maxWidth = 0,
             RenderTarget::maxHeight = 0;
const RenderTarget * RenderTarget::currentTarget = 0;


unsigned int RenderTarget::GetMaxAttachmentWidth(void)
{
    if (maxWidth == 0)
    {
        int param;
        glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &param);
        assert(param >= 0);
        maxWidth = (unsigned int)param;
    }
    return maxWidth;
}
unsigned int RenderTarget::GetMaxAttachmentHeight(void)
{
    if (maxHeight == 0)
    {
        int param;
        glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &param);
        assert(param >= 0);
        maxHeight = (unsigned int)param;
    }
    return maxHeight;
}
unsigned int RenderTarget::GetMaxNumbColorAttachments(void)
{
    if (maxColorAttachments == 0)
    {
        int param;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &param);
        assert(param >= 0);
        maxColorAttachments = (unsigned int)param;
    }
    return maxColorAttachments;
}


RenderTarget::RenderTarget(PixelSizes rendBuffSize)
    : depthTex(0), width(0), height(0), depthRenderBufferSize(rendBuffSize)
{
    if (maxColorAttachments == 0) maxColorAttachments = GetMaxNumbColorAttachments();

	ClearAllRenderingErrors();

    //Create frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //Create the depth renderbuffer to fall back on if not using a depth texture.
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    if (!IsPixelSizeDepth(rendBuffSize))
    {
        errorMsg = "Render buffer size specified in constructor is not a depth size type! It is " + DebugAssist::ToString(rendBuffSize);
        return;
    }
    glRenderbufferStorage(GL_RENDERBUFFER, ToGLenum(rendBuffSize), 1, 1);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

    //Check framebuffer status.
    RenderDataHandler::FrameBufferStatus stat = RenderDataHandler::GetFramebufferStatus();
    if (stat != RenderDataHandler::FrameBufferStatus::EVERYTHING_IS_FINE)
    {
        errorMsg = "Framebuffer is not ready! Error ";
        errorMsg += RenderDataHandler::GetFrameBufferStatusMessage();
        return;
    }
}

RenderTarget::~RenderTarget(void)
{
	glDeleteFramebuffers(1, &frameBuffer);
    glDeleteRenderbuffers(1, &depthRenderBuffer);
}

bool RenderTarget::IsUseable(void) const
{
    //Make sure there aren't too many color attachments for the hardware to handle.
    if (GetMaxNumbColorAttachments() < colorTexes.size())
    {
        errorMsg = std::string() + "You are limited to " + std::to_string(GetMaxNumbColorAttachments()) +
            " color textures per frame buffer, but you tried to attach " + std::to_string(colorTexes.size());
        return false;
    }


	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	RenderDataHandler::FrameBufferStatus status = RenderDataHandler::GetFramebufferStatus();

    if (status != RenderDataHandler::FrameBufferStatus::EVERYTHING_IS_FINE)
    {
        errorMsg = RenderDataHandler::GetFrameBufferStatusMessage();
        glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);
        return false;
    }

    return true && errorMsg.empty();
}

bool RenderTarget::SetColorAttachments(std::vector<RenderTargetTex> newColorTexes, bool updateDepthSize)
{
    //Make sure there aren't too many attachments.
    if (newColorTexes.size() > GetMaxNumbColorAttachments())
    {
        errorMsg = "You tried to attach " + std::to_string(newColorTexes.size()) + " color textures, but you can only attach up to " + std::to_string(GetMaxNumbColorAttachments());
        return false;
    }

    unsigned int newWidth = GetMaxAttachmentWidth(),
                 newHeight = GetMaxAttachmentHeight();

    //Set up each attachment.
    std::vector<GLenum> colAttachments;
    colAttachments.reserve(newColorTexes.size());
    for (unsigned int i = 0; i < maxColorAttachments; ++i)
    {
        if (i >= newColorTexes.size())
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
        }
        else
        {
            colAttachments.insert(colAttachments.end(), GL_COLOR_ATTACHMENT0 + i);

            const RenderTargetTex & tex = newColorTexes[i];
            if (tex.MTex == 0 && tex.MTexCube == 0)
            {
                errorMsg = "Color attachment " + std::to_string(i) + " doesn't have an associated texture";
                return false;
            }
            if (tex.MTex != 0 && tex.MTexCube != 0)
            {
                errorMsg = "Color attachment " + std::to_string(i) + " has an associated 2D texture AND cubemap face texture";
                return false;
            }

            //Get texture information.
            RenderObjHandle texHandle;
            GLenum textureType;
            unsigned int colWidth;
            unsigned int colHeight;
            if (tex.MTex != 0)
            {
                texHandle = tex.MTex->GetTextureHandle();
                textureType = GL_TEXTURE_2D;
                colWidth = tex.MTex->GetWidth();
                colHeight = tex.MTex->GetHeight();
                //TODO: Try removing the below line and making sure things still work. Do the same with the other clause's version of this.
                tex.MTex->Bind();
            }
            else
            {
                texHandle = tex.MTexCube->GetTextureHandle();
                textureType = TextureTypeToGLEnum(tex.MTexCube_Face);
                colWidth = tex.MTexCube->GetWidth();
                colHeight = tex.MTexCube->GetHeight();
                tex.MTexCube->Bind();
            }

            //Make sure the texture will work and, if it will, attach it.
            if (colWidth > maxWidth)
            {
                errorMsg = "Color attachment " + std::to_string(i) + " is " + std::to_string(width) + " wide, but can't be more than " + std::to_string(GetMaxAttachmentWidth());
                return false;
            }
            if (colHeight > maxHeight)
            {
                errorMsg = "Color attachment " + std::to_string(i) + " is " + std::to_string(height) + " tall, but can't be more than " + std::to_string(GetMaxAttachmentHeight());
                return false;
            }

            newWidth = Mathf::Min(newWidth, colWidth);
            newHeight = Mathf::Min(newHeight, colHeight);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textureType, texHandle, 0);
        }
    }

    width = newWidth;
    height = newHeight;
    glDrawBuffers(colAttachments.size(), colAttachments.data());
    colorTexes = newColorTexes;

    if (updateDepthSize)
    {
        if (depthTex.MTex != 0)
        {
            depthTex.MTex->ClearData(width, height);
        }
        else if (depthTex.MTexCube != 0)
        {
            depthTex.MTexCube->ClearData(width, height);
        }
        else
        {
            glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, ToGLenum(depthRenderBufferSize), width, height);
        }
    }

    return true;
}
bool RenderTarget::SetDepthAttachment(RenderTargetTex newDepthTex, bool changeSize)
{
    depthTex = newDepthTex;

    if (depthTex.MTex != 0)
    {
        if (changeSize)
            depthTex.MTex->ClearData(width, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex.MTex->GetTextureHandle(), 0);
    }
    else if (depthTex.MTexCube != 0)
    {
        if (changeSize)
            depthTex.MTexCube->ClearData(width, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureTypeToGLEnum(depthTex.MTexCube_Face), depthTex.MTexCube->GetTextureHandle(), 0);
    }
    else
    {
        if (changeSize)
        {
            glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, ToGLenum(depthRenderBufferSize), width, height);
        }
    }

    return true;
}


bool RenderTarget::UpdateSize(void)
{
    unsigned int maxW = GetMaxAttachmentWidth(),
                 maxH = GetMaxAttachmentHeight();
    width = maxW;
    height = maxH;

    for (int i = 0; i < colorTexes.size(); ++i)
    {
        //Figure out the attachment's width/height.
        unsigned int tempWidth, tempHeight;
        if (colorTexes[i].MTex != 0)
        {
            tempWidth = colorTexes[i].MTex->GetWidth();
            tempHeight = colorTexes[i].MTex->GetHeight();
        }
        else
        {
            tempWidth = colorTexes[i].MTexCube->GetWidth();
            tempHeight = colorTexes[i].MTexCube->GetHeight();
        }

        //Make sure the size is valid.
        if (tempWidth > maxW)
        {
            errorMsg = "Color attachment index " + std::to_string(i) + " is " + std::to_string(tempWidth) +
                           " wide, but must be no more than " + std::to_string(maxW);
            return false;
        }
        if (tempHeight > maxH)
        {
            errorMsg = "Color attachment index " + std::to_string(i) + " is " + std::to_string(tempHeight) +
                           " tall, but must be no more than " + std::to_string(maxH);
            return false;
        }

        //Update the size of this frame buffer.
        width = Mathf::Min(width, tempWidth);
        height = Mathf::Min(height, tempHeight);
    }

    //Update the depth buffer.
    if (depthTex.MTex != 0)
    {
        depthTex.MTex->ClearData(width, height);
    }
    else if (depthTex.MTexCube != 0)
    {
        depthTex.MTexCube->ClearData(width, height);
    }
    else
    {
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, ToGLenum(depthRenderBufferSize), width, height);
    }

    return true;
}

void RenderTarget::EnableDrawingInto(void) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
    currentTarget = this;
}
void RenderTarget::DisableDrawingInto(unsigned int w, unsigned int h, bool updateMipmaps) const
{
    currentTarget = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if (w == 0 && h == 0) glViewport(0, 0, w, h);

    if (updateMipmaps)
    {
        //Color textures.
        for (int i = 0; i < colorTexes.size(); ++i)
        {
            if (colorTexes[i].MTex != 0 && colorTexes[i].MTex->UsesMipmaps())
            {
                colorTexes[i].MTex->Bind();
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else if (colorTexes[i].MTexCube != 0 && colorTexes[i].MTexCube->UsesMipmaps())
            {
                colorTexes[i].MTexCube->Bind();
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }
        //Depth texture
        if (depthTex.MTex != 0 && depthTex.MTex->UsesMipmaps())
        {
            depthTex.MTex->Bind();
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (depthTex.MTexCube != 0 && depthTex.MTexCube->UsesMipmaps())
        {
            depthTex.MTexCube->Bind();
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
}