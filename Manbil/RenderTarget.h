#pragma once

#include <string>
#include "OpenGLIncludes.h"
#include "Math/Vectors.h"
#include "TextureSettings.h"
#include "Rendering/Texture Management/TextureLoadSettings.h"


//Specifies a color texture for a render target.
struct RendTargetColorTexSettings
{
public:
    ColorTextureSettings Settings;
    unsigned int ColorAttachment;
};
//Specifies an optional depth texture for a render target.
struct RendTargetDepthTexSettings
{
public:
    DepthTextureSettings Settings;
    bool UsesDepthTexture;

    RendTargetDepthTexSettings(void) : UsesDepthTexture(false) { }
    RendTargetDepthTexSettings(const DepthTextureSettings & settings) : Settings(settings), UsesDepthTexture(true) { }
};


//Allows drawing into a texture.
class RenderTarget
{
public:

    RenderTarget(const std::vector<RendTargetColorTexSettings> & colTexSettings, const RendTargetDepthTexSettings & depthTexSettings);
    RenderTarget(const RendTargetColorTexSettings & colTexSettings, const RendTargetDepthTexSettings & depthTexSettings)
        : RenderTarget(BuildVector(colTexSettings), depthTexSettings)
    {

    }
	~RenderTarget(void);

	RenderTarget(void); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.
	void operator=(const RenderTarget & other); //This function intentionally not implemented so as to give a compile-time error if somebody tries to use it.


    bool HasError(void) const { return !errorMsg.empty(); }
	const std::string & GetErrorMessage(void) const { return errorMsg; }
	void ClearErrorMessage(void) { errorMsg.clear(); }


    unsigned int GetNumbColorTextures(void) const { return colorTexes.size(); }

    const std::vector<RendTargetColorTexSettings> & GetColorSettings(void) const { return colTexesSetts; }
    const RendTargetDepthTexSettings & GetDepthSettings(void) const { return depthTexSetts; }

    const std::vector<RenderObjHandle> & GetColorTextures(void) const { return colorTexes; }
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
    
    static std::vector<RendTargetColorTexSettings> BuildVector(const RendTargetColorTexSettings & settings)
    {
        std::vector<RendTargetColorTexSettings> setts;
        setts.insert(setts.end(), settings);
        return setts;
    }


    //The texture settings.

    std::vector<RendTargetColorTexSettings> colTexesSetts;
    RendTargetDepthTexSettings depthTexSetts;


    //The textures/buffer.

	RenderObjHandle frameBuffer;
    std::vector<RenderObjHandle> colorTexes;
    RenderObjHandle depthTex;


    //Error-handling.

	mutable std::string errorMsg;
};