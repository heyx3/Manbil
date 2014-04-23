#pragma once

#include "../Rendering/PostProcessing/PostProcessChain.h"
#include "VoxelWorld.h"


//Manages the post-process chain for VoxelWorld.
class VoxelWorldPPC
{
public:

    VoxelWorldPPC(VoxelWorld & _world);
    ~VoxelWorldPPC(void)
    {
        for (unsigned int i = 0; i < chains.size(); ++i)
            delete chains[i];
    }


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }

    RenderTarget * GetFinalRender(void) const { return chains[chains.size() - 1]->GetFinalRender(); }


    //Returns false if there was an error resizing; returns true otherwise.
    bool OnWindowResized(unsigned int newW, unsigned int newH)
    {
        for (unsigned int i = 0; i < chains.size(); ++i)
            if (!chains[i]->ResizeRenderTargets(newW, newH))
                return false;
        return true;
    }


    //Renders the post-processing effects given the rendered world.
    //Returns whether or not the rendering succeeded.
   bool RenderPostProcessing(RenderTarget & worldRender, const ProjectionInfo & info);


private:

    std::string errorMsg;

    VoxelWorld & world;
    std::vector<PostProcessChain *> chains;
};