#include "RenderTarget.h"

#include <assert.h>

#include "../Basic Rendering/Vertices.h"
#include "../Basic Rendering/RenderingState.h"
#include "../../DebugAssist.h"


//Gets the status of the currently-bound framebuffer.
//If everything is OK, an empty string is returned.
std::string GetFramebufferStatusMessage(void)
{
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return "";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return "Bad color/depth buffer attachment";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return "Nothing is attached!";
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            return "Texture and depth buffer are different dimensions.";
		case GL_FRAMEBUFFER_UNSUPPORTED:
            return "This combination of texture and depth buffer is not supported on this platform.";

		default: return "Unknown frame buffer error.";
    }
}



unsigned int RenderTarget::maxColorAttachments = 0,
             RenderTarget::maxWidth = 0,
             RenderTarget::maxHeight = 0;
const RenderTarget* RenderTarget::currentTarget = 0;


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


RenderTarget::RenderTarget(PixelSizes rendBuffSize, std::string& outError)
    : depthTex(0), width(0), height(0), depthRenderBufferSize(rendBuffSize)
{
    if (maxColorAttachments == 0)
    {
        maxColorAttachments = GetMaxNumbColorAttachments();
    }

	ClearAllRenderingErrors();


    //Create frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


    //Create the depth renderbuffer to fall back on if not using a depth texture.
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    if (!IsPixelSizeDepth(rendBuffSize))
    {
        outError = "Render buffer size specified in constructor is not a depth size type! " +
                       std::string("It is ") + DebugAssist::ToString(rendBuffSize);
        return;
    }
    glRenderbufferStorage(GL_RENDERBUFFER, ToGLenum(rendBuffSize), 1, 1);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    

    //Save the currently-bound framebuffer before binding this one to check it out.
	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);

    //Check framebuffer status.
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    std::string err = GetFramebufferStatusMessage();
    if (!err.empty())
    {
        outError = "Framebuffer is not ready! " + err;
    }

    //Re-bind the previously-bound framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);
}

RenderTarget& RenderTarget::operator=(RenderTarget&& other)
{
    frameBuffer = other.frameBuffer;
    other.frameBuffer = 0;

    colorTexes = std::move(other.colorTexes);

    depthTex = other.depthTex;
    other.depthTex = 0;

    depthRenderBuffer = other.depthRenderBuffer;
    other.depthRenderBuffer = 0;

    depthRenderBufferSize = other.depthRenderBufferSize;

    width = other.width;
    height = other.height;

    return *this;
}

RenderTarget::~RenderTarget(void)
{
    //Unbind this render target if it's currently bound.
    if (currentTarget == this)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        currentTarget = 0;
    }

	glDeleteFramebuffers(1, &frameBuffer);
    if (depthRenderBuffer != 0)
    {
        glDeleteRenderbuffers(1, &depthRenderBuffer);
    }
}

bool RenderTarget::IsUseable(std::string& outErrorMsg) const
{
    //Make sure there aren't too many color attachments for the hardware to handle.
    if (GetMaxNumbColorAttachments() < colorTexes.size())
    {
        outErrorMsg = std::string("You are limited to ") +
                           std::to_string(GetMaxNumbColorAttachments()) +
                           " color textures per frame buffer, but you tried to attach " +
                           std::to_string(colorTexes.size());
        return false;
    }


    //Save the currently-bound framebuffer before binding this one to check it out.
	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);

    //Check out the status of this framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    outErrorMsg = GetFramebufferStatusMessage();

    //Clean up and return the status.
    glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);
    return outErrorMsg.empty();
}

bool RenderTarget::SetColorAttachment(RenderTargetTex newColorTex, bool updateDepthSize)
{
    std::vector<RenderTargetTex> rtts;
    rtts.insert(rtts.end(), newColorTex);
    return SetColorAttachments(rtts, updateDepthSize);
}
bool RenderTarget::SetColorAttachments(RenderTargetTex* newColTexes, unsigned int nTexes,
                                       bool updateDepthSize)
{
    EnableDrawingInto();

    //Make sure there aren't too many attachments.
    if (nTexes > GetMaxNumbColorAttachments())
    {
        return false;
    }

    unsigned int newWidth = GetMaxAttachmentWidth(),
                 newHeight = GetMaxAttachmentHeight();

    //Set up each attachment.
    std::vector<GLenum> colAttachments;
    colAttachments.reserve(nTexes);
    for (unsigned int i = 0; i < maxColorAttachments; ++i)
    {
        if (i >= nTexes)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
        }
        else
        {
            colAttachments.insert(colAttachments.end(), GL_COLOR_ATTACHMENT0 + i);

            const RenderTargetTex& tex = newColTexes[i];

            if ((tex.MTex == 0 && tex.MTexCube == 0) || (tex.MTex != 0 && tex.MTexCube != 0))
            {
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
            if (colWidth > maxWidth || colHeight > maxHeight)
            {
                return false;
            }

            newWidth = Mathf::Min(newWidth, colWidth);
            newHeight = Mathf::Min(newHeight, colHeight);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                   textureType, texHandle, 0);
        }
    }

    width = newWidth;
    height = newHeight;
    if (colAttachments.size() == 0)
    {
        glDrawBuffer(GL_NONE);
    }
    else
    {
        glDrawBuffers(colAttachments.size(), colAttachments.data());
    }
    colorTexes.resize(nTexes);
    memcpy(colorTexes.data(), newColTexes, sizeof(RenderTargetTex) * nTexes);

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
    if (width == 0 && height == 0)
    {
        width = (depthTex.MTex != 0 ?
                    depthTex.MTex->GetWidth() :
                    (depthTex.MTexCube != 0 ?
                        depthTex.MTexCube->GetWidth() :
                        0));
        height = (depthTex.MTex != 0 ?
                     depthTex.MTex->GetHeight() :
                     (depthTex.MTexCube != 0 ?
                         depthTex.MTexCube->GetHeight() :
                         0));
    }

    if (depthTex.MTex != 0)
    {
        if (changeSize)
        {
            depthTex.MTex->ClearData(width, height);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               depthTex.MTex->GetTextureHandle(), 0);
    }
    else if (depthTex.MTexCube != 0)
    {
        if (changeSize)
        {
            depthTex.MTexCube->ClearData(width, height);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               TextureTypeToGLEnum(depthTex.MTexCube_Face),
                               depthTex.MTexCube->GetTextureHandle(), 0);
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
    if (colorTexes.size() == 0)
    {
        width = (depthTex.MTex != 0 ?
                    depthTex.MTex->GetWidth() :
                    depthTex.MTexCube->GetWidth());
        height = (depthTex.MTex != 0 ?
                    depthTex.MTex->GetHeight() :
                    depthTex.MTexCube->GetHeight());
    }

    for (unsigned int i = 0; i < colorTexes.size(); ++i)
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
        if (tempWidth > maxW || tempHeight > maxH)
        {
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

void RenderTarget::EnableDrawingInto(Viewport v) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    v.SetAsViewport();
    currentTarget = this;
}
void RenderTarget::DisableDrawingInto(unsigned int w, unsigned int h, bool updateMipmaps) const
{
    currentTarget = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if (w != 0 && h != 0)
    {
        glViewport(0, 0, w, h);
    }

    if (updateMipmaps)
    {
        //Color textures.
        for (unsigned int i = 0; i < colorTexes.size(); ++i)
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