#include "VoxelWorldPPC.h"


VoxelWorldPPC::VoxelWorldPPC(VoxelWorld & _world)
    : world(_world)
{
    typedef std::shared_ptr<PostProcessEffect> PpePtr;
    std::vector<PpePtr> effects;


    effects.insert(effects.end(), PpePtr(new FogEffect()));
    PostProcessChain * chn = new PostProcessChain(effects, world.GetWindow()->getSize().x, world.GetWindow()->getSize().y,
                                                  false, TextureSampleSettings(TextureSampleSettings::FT_NEAREST, TextureSampleSettings::WT_CLAMP),
                                                  PixelSizes::PS_32F, *world.RenderTargets);
    if (chn->HasError())
    {
        errorMsg = "Error creating effect chain: " + chn->GetError();
        return;
    }
    chains.insert(chains.end(), chn);
}

bool VoxelWorldPPC::RenderPostProcessing(RenderObjHandle colorIn, RenderObjHandle depthIn, const ProjectionInfo & info)
{
    for (unsigned int chain = 0; chain < chains.size(); ++chain)
    {
        //Get the color source based on the current chain being rendered.
        RenderObjHandle srcRend;
        switch (chain)
        {
            case 0:
                srcRend = colorIn;
                break;

            default: assert(false); srcRend = 0;
        }

        //Try to render the chain.
        if (!chains[chain]->RenderChain(&world, info, srcRend, depthIn))
        {
            errorMsg = "Error rendering chain " + std::to_string(chain) + ": " + chains[chain]->GetError();
            return false;
        }
    }

    return true;
}