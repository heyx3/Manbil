#pragma once

#include "../Rendering.hpp"

//Represents an effect, applied over a RenderTarget, to be rendered onto the screen (or to another RenderTarget).
class PostProcessEffect
{
public:

    //Prevent bad auto-generated functions from being called.

    PostProcessEffect(void);                          //This function intentionally not implemented.
    PostProcessEffect(const PostProcessEffect & cpy); //This function intentionally not implemented.
    void operator=(const PostProcessEffect & other);  //This function intentionally not implemented.


    //Takes in the post-process effects (as individual passes to be applied to a screen quad).
    PostProcessEffect(std::vector<RenderingPass> passes, RenderTarget target);
    ~PostProcessEffect(void);


    bool HasError(void) const { return !errorMsg.empty() || renderTarget.HasError() || material.HasError(); }
    const std::string & GetErrorMessage(void) const;
    void ClearErrorMessage(void) { errorMsg.clear(); material.SetErrorMessage(""); renderTarget.ClearErrorMessage(); }
    \
    bool UsesColorTexture(void) const { return usesCol; }
    bool UsesDepthTexture(void) const { return usesDepth; }

    const RenderTarget & GetRenderTarget(void) const { return renderTarget; }
    RenderTarget & GetRenderTarget(void) { return renderTarget; }

    const Material & GetMaterial(void) const { return material; }
    Material & GetMaterial(void) { return material; }


private:

    mutable std::string errorMsg;

    bool usesCol, usesDepth;

    Material material;
    RenderTarget renderTarget;
    BufferObjHandle vbo;
};