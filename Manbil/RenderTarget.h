#pragma once

#include <string>
#include "OpenGLIncludes.h"
#include "Math/Vectors.h"
#include "TextureSettings.h"



//Settings for initialization of a render target.
struct RenderTargetSettings
{
public:

    enum ColorTextureSizes
    {
        CTS_8,
        CTS_16,
        CTS_32,
        CTS_UNUSED,
    };
    enum DepthTextureSizes
    {
        DTS_16,
        DTS_24,
        DTS_32,
    };

    static GLenum ToEnum(ColorTextureSizes size)
    {
        switch (size)
        {
            case ColorTextureSizes::CTS_8: return GL_RGBA8;
            case ColorTextureSizes::CTS_16: return GL_RGBA16;
            case ColorTextureSizes::CTS_32: return GL_RGBA32F;
            default: return GL_INVALID_ENUM;
        }
    }
    static GLenum ToEnum(DepthTextureSizes size)
    {
        switch (size)
        {
            case DepthTextureSizes::DTS_16: return GL_DEPTH_COMPONENT16;
            case DepthTextureSizes::DTS_24: return GL_DEPTH_COMPONENT24;
            case DepthTextureSizes::DTS_32: return GL_DEPTH_COMPONENT32;
            default: return GL_INVALID_ENUM;
        }
    }

    static std::string ToString(ColorTextureSizes size)
    {
        switch (size)
        {
            case ColorTextureSizes::CTS_8: return "8";
            case ColorTextureSizes::CTS_16: return "16";
            case ColorTextureSizes::CTS_32: return "32";
            case ColorTextureSizes::CTS_UNUSED: return "0 (unused)";
            default: return "Unknown size: " + std::to_string(size);
        }
    }
    static std::string ToString(DepthTextureSizes size)
    {
        switch (size)
        {
            case DepthTextureSizes::DTS_16: return "16";
            case DepthTextureSizes::DTS_24: return "24";
            case DepthTextureSizes::DTS_32: return "32";
            default: return "Unknown size: " + std::to_string(size);
        }
    }


    unsigned int Width, Height;
    TextureSettings ColTexSettings, DepthTexSettings;
    unsigned int ColorAttachment;
    ColorTextureSizes ColTexSize;
    DepthTextureSizes DepthTexSize;
    bool UsesDepthTexture;

    RenderTargetSettings(unsigned int width = 1, unsigned int height = 1, bool usesDepthTex = true,
                         TextureSettings colTexSettings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false),
                         TextureSettings depthTexSettings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false),
                         unsigned int colorAttachment = 0,
                         ColorTextureSizes colTexSize = ColorTextureSizes::CTS_32,
                         DepthTextureSizes depthTexSize = DepthTextureSizes::DTS_24)
        : Width(width), Height(height), UsesDepthTexture(usesDepthTex),
          ColTexSettings(colTexSettings), DepthTexSettings(depthTexSettings),
          ColorAttachment(colorAttachment), ColTexSize(colTexSize), DepthTexSize(depthTexSize)
    {

    }

    bool GetUsesColorTexture(void) const { return ColTexSize != ColorTextureSizes::CTS_UNUSED; }

    std::string ToString(void) const;
};


//Allows drawing into a texture.
class RenderTarget
{
public:

    //Takes in the size of the target.
    RenderTarget(const RenderTargetSettings & settings);
	~RenderTarget(void);

	RenderTarget(void); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.
	void operator=(const RenderTarget & other); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.


    bool HasError(void) const { return !errorMsg.empty(); }
	const std::string & GetErrorMessage(void) const { return errorMsg; }
	void ClearErrorMessage(void) { errorMsg.clear(); }

    const RenderTargetSettings & GetSettings(void) const { return settings; }

    RenderObjHandle GetColorTexture(void) const { return colorTex; }
    RenderObjHandle GetDepthTexture(void) const { return depthTex; }

    RenderObjHandle GetFramebuffer(void) const { return frameBuffer; }


	//Gets whether this render target is ready to be used.
	bool IsValid(void) const;

	//Sets up this render target so that any rendering will go into this render target.
	void EnableDrawingInto(void) const;

	//Sets up this render target so that any rendering will go into the default frame buffer (a.k.a. the window).
    //Takes in the size of the back buffer to reset to.
	void DisableDrawingInto(unsigned int width, unsigned int height) const;

private:

    RenderTargetSettings settings;

	RenderObjHandle frameBuffer, colorTex, depthTex;

	mutable std::string errorMsg;
};