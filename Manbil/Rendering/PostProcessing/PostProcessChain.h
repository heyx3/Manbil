#pragma once

#include "../Rendering.hpp"
#include "PostProcessData.h"
#include "../Texture Management/RenderTargetManager.h"
#include "../Helper Classes/DrawingQuad.h"



//TODO: Change from a piggy-backed DataNode system to a higher-level system -- a list of PostProcessEffects, each with their own behaviors/rendering material/uniforms. For example, a GaussianBlurEffect or a "SimpleEffect" (which is a combination of all the effects that can be simply done in one pass).


//Represents a special kind of material (actually an arbitrary number of materials)
//   that applies a series of post-processing effects to the screen.
//This class holds a DrawingQuad instance used for rendering the effects.
//The quad viewport is in the range {-1, -1, -1} to {1, 1, 1}.
//Note that the uniforms this chain stores for each material will overwrite
//    any duplicate uniforms set in the drawing quad.
class PostProcessChain
{
public:

    PostProcessChain(std::vector<std::shared_ptr<PostProcessEffect>> effectChain,
                     unsigned int screenWidth, unsigned int screenHeight, bool useMipmaps,
                     const TextureSampleSettings2D & renderTargetSettings, PixelSizes pixelSize,
                     RenderTargetManager & manager);
    ~PostProcessChain(void)
    {
        if (rt1 != RenderTargetManager::ERROR_ID)
        {
            rtManager.DeleteRenderTarget(rt1);
        }
        if (rt2 != RenderTargetManager::ERROR_ID)
        {
            rtManager.DeleteRenderTarget(rt2);
        }
    }


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }


    const std::vector<std::shared_ptr<Material>> & GetMaterials(void) const { return materials; }
    std::vector<std::shared_ptr<Material>> & GetMaterials(void) { return materials; }

    const std::vector<UniformDictionary> & GetUniforms(void) const { return uniforms; }
    std::vector<UniformDictionary> & GetUniforms(void) { return uniforms; }

    const DrawingQuad & GetQuad(void) const { return quad; }
    DrawingQuad & GetQuad(void) { return quad; }

    const TransformObject & GetTransform(void) const { return quad.GetMesh().Transform; }
    TransformObject & GetTransform(void) { return quad.GetMesh().Transform; }


    //Gets the render target that will hold the final result after this chain is done rendering.
    //Returns 0 if this PostProcessChain doesn't apply any effects.
    RenderTarget * GetFinalRender(void) const
    {
        if (totalPasses == 0) return 0;
        return rtManager[(totalPasses % 2 == 1) ? rt1 : rt2];
    }

    //Resizes the render targets used to render this chain's effects.
    //Returns whether or not the resizing was successful.
    bool ResizeRenderTargets(unsigned int newWidth, unsigned int newHeight);

    //Takes in the render target that this chain should be rendered onto.
    //Returns whether or not the render was successful.
    //TODO: Optionally let the user specify to draw the final pass directly into the default buffer.
    bool RenderChain(SFMLOpenGLWorld * world, const ProjectionInfo & info, RenderObjHandle colorIn, RenderObjHandle depthIn);


private:

    std::string errorMsg;

    std::vector<std::shared_ptr<Material>> materials;
    std::vector<UniformDictionary> uniforms;

    DrawingQuad quad;
    UniformDictionary params;

    RenderTargetManager & rtManager;
    unsigned int rt1, rt2;
    MTexture2D ct1, ct2;
    unsigned int totalPasses;
};