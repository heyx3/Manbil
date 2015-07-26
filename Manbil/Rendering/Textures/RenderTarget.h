#pragma once

#include <string>
#include <vector>
#include "../Basic Rendering/OpenGLIncludes.h"
#include "../Basic Rendering/Viewport.h"
#include "../../Math/Lower Math/Vectors.h"
#include "MTexture2D.h"
#include "MTextureCubemap.h"



//A texture that can be bound to a framebuffer.
//Either a 2D texture or a face of a cubemap texture.
struct RenderTargetTex
{
public:
    MTexture2D* MTex = 0;
    MTextureCubemap* MTexCube = 0;
    CubeTextureTypes MTexCube_Face;

    RenderTargetTex(void) { }
    RenderTargetTex(MTexture2D* mTex) : MTex(mTex) { }

    RenderTargetTex(MTextureCubemap* mTexCube, CubeTextureTypes face)
        : MTexCube(mTexCube), MTexCube_Face(face) { }
};



//A set of textures that can be rendered into instead of rendering into the window's buffer.
//Can have up to one depth texture and some platform-dependent number of color textures.
class RenderTarget
{
public:

    //Gets the maximum width of an attached texture.
    static unsigned int GetMaxAttachmentWidth(void);
    //Gets the maximum height of an attached texture.
    static unsigned int GetMaxAttachmentHeight(void);
    //Gets the maximum number of color attachments allowed at once.
    static unsigned int GetMaxNumbColorAttachments(void);

    //Gets the currently-bound render target, or 0 if the back buffer is currently bound.
    //Note that this only works if no other classes modify OpenGL framebuffer state.
    static const RenderTarget* GetCurrentTarget(void) { return currentTarget; }


    //The constructor creates an invalid render target.
    RenderTarget(void) : frameBuffer(0), depthTex(0), depthRenderBuffer(0), width(0), height(0) { }
    
    //If no depth texture gets attached, the depth must still be rendered into somewhere.
    //This constructor takes in in the pixel size of the default depth render buffer.
    //It also takes an error message that it will output to if something goes wrong.
    RenderTarget(PixelSizes defaultDepthPixelSize, std::string& outErrorMsg);
    
    RenderTarget(RenderTarget&& moveCpy) { *this = std::move(moveCpy); }
    RenderTarget& operator=(RenderTarget&& moveCpy);

	~RenderTarget(void);


    RenderTarget(const RenderTarget& cpy) = delete;
	void operator=(const RenderTarget& other) = delete;


    RenderObjHandle GetFramebufferHandle(void) const { return frameBuffer; }
    const std::vector<RenderTargetTex>& GetColorTextures(void) const { return colorTexes; }

    //Note that the depth texture may or may not exist.
    RenderTargetTex GetDepthTexture(void) const { return depthTex; }
    
    //The effective width of the render target.
    //Equal to the smallest width of the attached color textures.
    unsigned int GetWidth(void) const { return width; }
    //The effective height of the render target.
    //Equal to the smallest height of the attached color textures.
    unsigned int GetHeight(void) const { return height; }

    //Gets whether this render target is currently bound.
    bool IsBound(void) const { return currentTarget == this; }


    //Render target operations.

    //Replaces the current color attachments with the given one.
    //If "updateDepthSize" is true, the depth texture/render buffer is resized
    //    to this render target's new width/height.
    //Returns whether the operation succeeded.
    //This operation fails if the size of the attachment is too big for OpenGL,
    //    or if the RenderTargetTex data structure is malformed.
    //IMPORTANT NOTE: This operation binds this render target to OpenGL, removing the previous one.
    bool SetColorAttachment(RenderTargetTex newColorTex, bool updateDepthSize);
    //Replaces the current color attachments with the given ones.
    //If "updateDepthSize" is true, the currently-attached depth texture is resized
    //    to this render target's new width/height.
    //Returns whether the operation succeeded.
    //This operation fails if the size of the attachment is too big for OpenGL,
    //    or if too many attachments were passed in,
    //    or if any of the RenderTargetTex data structures are malformed.
    //IMPORTANT NOTE: This operation binds this render target to OpenGL, removing the previous one.
    bool SetColorAttachments(RenderTargetTex* newColorTexes, unsigned int nElements, bool updateDepthSize);
    //Replaces the current color attachments with the given ones.
    //If "updateDepthSize" is true, the currently-attached depth texture is resized
    //    to this render target's new width/height.
    //Returns whether the operation succeeded.
    //This operation fails if the size of the attachment is too big for OpenGL,
    //    or if too many attachments were passed in,
    //    or if any of the RenderTargetTex data structures are malformed.
    //IMPORTANT NOTE: This operation binds this render target to OpenGL, removing the previous one.
    bool SetColorAttachments(std::vector<RenderTargetTex> newColorTexes, bool updateDepthSize)
    {
        return SetColorAttachments(newColorTexes.data(), newColorTexes.size(), updateDepthSize);
    }
    //Replaces the current depth attachment with the given attachment.
    //If the default value is supplied, the current depth texture will just be removed.
    //"changeToCorrectSize" indicates whether to resize the depth texture
    //    to be the effective size of this RenderTarget.
    //Returns whether the operation succeeded.
    //This operation fails if the size of the attachment is too big for OpenGL,
    //    or if the RenderTargetTex data structure is malformed.
    //IMPORTANT NOTE: This operation binds this render target to OpenGL, removing the previous one.
    bool SetDepthAttachment(RenderTargetTex newDepthTex = RenderTargetTex(),
                            bool changeToCorrectSize = true);

    //Should be called whenever a color attachment's size is changed.
    //Updates the effective size of this render target and its depth buffer.
    //Returns whether the operation succeeded.
    //This operation fails if the size of the depth buffer would be too big for OpenGL.
    bool UpdateSize(void);

	//Gets whether this render target is ready to be used.
    //If this render target isn't ready, an error message is output to the given string.
	bool IsUseable(std::string& outErrorMsg) const;

    //Generates mipmaps for any attached color/depth textures that use it.
    void GenerateMipmaps(void) const;

	//Sets the OpenGL state so that any rendering will go into this render target.
    void EnableDrawingInto(void) const { EnableDrawingInto(Viewport(0, 0, width, height)); }
    //Sets the OpenGL state so that any rendering will go the given portion of this render target.
    void EnableDrawingInto(Viewport viewport) const;
	//Sets the OpenGL state so that any rendering will go into the back buffer instead of a texture.
    //Takes in the size of the default buffer and whether to update mipmaps
    //    for all textures attached to this buffer.
    //If 0 is passed in for the width and height, the back buffer will not be reset.
	void DisableDrawingInto(unsigned int width = 0, unsigned int height = 0,
                            bool updateMipmaps = true) const;


private:
    
    RenderObjHandle frameBuffer;
    std::vector<RenderTargetTex> colorTexes;
    RenderTargetTex depthTex;

    RenderObjHandle depthRenderBuffer;
    PixelSizes depthRenderBufferSize;

    unsigned int width, height;

    static const RenderTarget* currentTarget;
    static unsigned int maxColorAttachments, maxWidth, maxHeight;
};