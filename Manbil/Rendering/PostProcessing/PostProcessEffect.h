#pragma once

#include "../Rendering.hpp"
#include "PostProcessData.h"
#include "../Helper Classes/DrawingQuad.h"



//Represents a special kind of material (actually an arbitrary number of materials)
//   that applies a series of post-processing effects to the screen.
class PostProcessEffect
{
public:

    RenderTarget RenderingTarget;

    //If 
    PostProcessEffect(std::unordered_map<RenderingChannels, DataLine> channels);



    const Material & GetMaterial(void) const { return mat; }
    Material & GetMaterial(void) { return mat; }

    const DrawingQuad & GetQuad(void) const { return quad; }
    DrawingQuad & GetQuad(void) { return quad; }


private:

    Material mat;
    DrawingQuad quad;
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