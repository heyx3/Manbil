#pragma once

#include "../Rendering/PostProcessing/PostProcessChain.h"
#include "VoxelWorld.h"


//Manages the post-process chain of effects for VoxelWorld.
class VoxelWorldPPC
{
public:

    VoxelWorldPPC(VoxelWorld& _world, std::string& outError);
    ~VoxelWorldPPC(void);


    const MTexture2D& GetFinalColor(void) const { return colorTex; }
    MTexture2D& GetFinalColor(void) { return colorTex; }


    void OnWindowResized(unsigned int newW, unsigned int newH);


    //Renders the post-processing effects, given the world color, depth, and projection info.
    void RenderPostProcessing(RenderObjHandle colorIn, RenderObjHandle depthIn,
                              const ProjectionInfo& info);


private:

    std::string errorMsg;
    VoxelWorld& world;

    Material* ppMat;
    UniformDictionary ppParams;

    RenderTarget rendTarg;
    MTexture2D colorTex;
};