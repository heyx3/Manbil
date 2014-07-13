#pragma once

#include <string>
#include <vector>
#include "OpenGLIncludes.h"
#include "Math/Vectors.h"
#include "Rendering/Texture Management/MTexture.h"
#include "Rendering/Texture Management/MTextureCubemap.h"


//A texture that can be bound to a framebuffer.
//Either a 2D texture or a face of a cubemap texture.
struct RenderTargetTex
{
public:
    MTexture * MTex = 0;
    MTextureCubemap * MTexCube = 0;
    CubeTextureTypes MTexCube_Face;
    RenderTargetTex(void) { }
    RenderTargetTex(MTexture * mTex) : MTex(mTex) { }
    RenderTargetTex(MTextureCubemap * mTexCube, CubeTextureTypes face) : MTexCube(mTexCube), MTexCube_Face(face) { }
};


//Allows drawing into a texture.
class RenderTarget
{
public:

    static unsigned int GetMaxAttachmentWidth(void);
    static unsigned int GetMaxAttachmentHeight(void);
    static unsigned int GetMaxNumbColorAttachments(void);


    //Takes in the size of the default depth renderbuffer used if no depth texture is attached.
    RenderTarget(PixelSizes defaultDepthPixelSize);
	~RenderTarget(void);

	RenderTarget(void); //This function intentionally not implemented.
	void operator=(const RenderTarget & other); //This function intentionally not implemented.


    //Error-handling.

    bool HasError(void) const { return !errorMsg.empty(); }
	const std::string & GetErrorMessage(void) const { return errorMsg; }
	void ClearErrorMessage(void) { errorMsg.clear(); }


    //Getters.

    RenderObjHandle GetFramebufferHandle(void) const { return frameBuffer; }
    const std::vector<RenderTargetTex> & GetColorTextures(void) const { return colorTexes; }
    //The depth texture may or may not exist.
    RenderTargetTex GetDepthTexture(void) const { return depthTex; }
    
    //The effective width of the render target. Equal to the smallest width of the attached color textures.
    unsigned int GetWidth(void) const { return width; }
    //The effective height of the render target. Equal to the smallest height of the attached color textures.
    unsigned int GetHeight(void) const { return height; }


    //Render target operations.

    //Replaces the current color attachments with the given one.
    //If "updateDepthSize" is true, the depth texture/render buffer is resized to this render target's new width/height.
    //Returns whether the operation succeeded.
    bool SetColorAttachment(RenderTargetTex newColorTex, bool updateDepthSize)
    {
        std::vector<RenderTargetTex> rtts;
        rtts.insert(rtts.end(), newColorTex);
        return SetColorAttachments(rtts, updateDepthSize);
    }
    //Replaces the current color attachments with the given ones.
    //If "updateDepthSize" is true, the depth texture/render buffer is resized to this render target's new width/height.
    //Returns whether the operation succeeded.
    bool SetColorAttachments(std::vector<RenderTargetTex> newColorTexes, bool updateDepthSize);
    //Replaces the current depth attachment with the given attachment.
    //If the default value is supplied, a render buffer will be used instead (an optimized texture that can't be manipulated in software).
    //"changeToCorrectSize" indicates whether to resize the depth texture to be the effective size of this RenderTarget.
    //Returns whether the operation succeeded.
    bool SetDepthAttachment(RenderTargetTex newDepthTex = RenderTargetTex(), bool changeToCorrectSize = true);

    //Should be called whenever a color attachment's size is changed.
    //Returns whether the operation succeeded.
    bool UpdateSize(void);

	//Gets whether this render target is ready to be used.
	bool IsUseable(void) const;

    //Generates mipmaps for any attached color/depth textures that use it.
    void GenerateMipmaps(void) const;

	//Sets the OpenGL state so that any rendering will go into this render target.
	void EnableDrawingInto(void) const;
	//Sets the OpenGL state so that any rendering will go into the default frame buffer (a.k.a. the window).
    //Takes in the size of the default buffer and whether to update mipmaps for any color/depth textures that use them.
	void DisableDrawingInto(unsigned int width, unsigned int height, bool updateMipmaps) const;


private:
    
    RenderObjHandle frameBuffer;
    std::vector<RenderTargetTex> colorTexes;
    RenderTargetTex depthTex;

    RenderObjHandle depthRenderBuffer;
    PixelSizes depthRenderBufferSize;

    unsigned int width, height;

	mutable std::string errorMsg;


    static unsigned int maxColorAttachments, maxWidth, maxHeight;
};