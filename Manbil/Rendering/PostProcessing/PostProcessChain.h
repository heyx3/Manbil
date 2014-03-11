#pragma once

#include "../Rendering.hpp"
#include "PostProcessData.h"
#include "../Texture Management/RenderTargetManager.h"
#include "../Helper Classes/DrawingQuad.h"



//Represents a special kind of material (actually an arbitrary number of materials)
//   that applies a series of post-processing effects to the screen.
class PostProcessChain
{
public:

    PostProcessChain(std::vector<std::shared_ptr<PostProcessEffect>> effectChain,
                     unsigned int screenWidth, unsigned int screenHeight,
                     RenderTargetManager & manager);


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }

    //Gets the render target that will hold the final screen after this chain is done rendering.
    RenderTarget * GetFinalRender(void) const { return rtManager[(totalPasses % 2 == 1) ? rt1 : rt2]; }

    const std::vector<std::shared_ptr<Material>> & GetMaterials(void) const { return materials; }
    std::vector<std::shared_ptr<Material>> & GetMaterials(void) { return materials; }

    const DrawingQuad & GetQuad(void) const { return quad; }
    DrawingQuad & GetQuad(void) { return quad; }


    //Takes in a texture whose rgb values are the screen color,
    //    and whose alpha value is the screen depth.
    //Returns whether or not the render was successful.
    bool RenderChain(const RenderTarget * inColorDepth);


private:

    std::string errorMsg;

    std::vector<std::shared_ptr<Material>> materials;
    DrawingQuad quad;

    RenderTargetManager & rtManager;
    unsigned int rt1, rt2;
    unsigned int totalPasses;
};







/*

//Represents an effect, applied over a RenderTarget, to be rendered onto the screen (or to another RenderTarget).
class PostProcessEffect
{
public:

    //Prevent bad auto-generated functions from being called.

    PostProcessEffect(void);                          //This function intentionally not implemented.
    PostProcessEffect(const PostProcessEffect & cpy); //This function intentionally not implemented.
    void operator=(const PostProcessEffect & other);  //This function intentionally not implemented.


    //Takes in the post-process effects (as individual passes to be applied to a screen quad).
    //The color texture will be in "u_sampler0", and the depth texture will be in "u_sampler1".
    PostProcessEffect(unsigned int renderWidth, unsigned int renderHeight, std::vector<RenderingPass> passes);
    ~PostProcessEffect(void);


    bool HasError(void) const { return !errorMsg.empty() || renderTarget->HasError() || material.HasError(); }
    std::string GetErrorMessage(void) const;
    void ClearErrorMessage(void) { errorMsg.clear(); material.SetErrorMessage(""); renderTarget->ClearErrorMessage(); }
    
    bool UsesColorTexture(void) const { return usesCol; }
    bool UsesDepthTexture(void) const { return usesDepth; }

    const RenderTarget * GetRenderTarget(void) const { return renderTarget; }
    RenderTarget * GetRenderTarget(void) { return renderTarget; }

    const Material & GetMaterial(void) const { return material; }
    Material & GetMaterial(void) { return material; }

    const Mesh & GetMesh(void) const { return screenMesh; }
    Mesh & GetMesh(void) { return screenMesh; }


    void RenderEffect(const RenderInfo & info, Vector2f screenOffset = Vector2f(), Vector2f screenScale = Vector2f(1.0f, 1.0f));


private:

    mutable std::string errorMsg;

    bool usesCol, usesDepth;

    Material material;
    Mesh screenMesh;
    RenderTarget * renderTarget;
};

*/