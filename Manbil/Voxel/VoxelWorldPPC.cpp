#include "VoxelWorldPPC.h"


VoxelWorldPPC::VoxelWorldPPC(VoxelWorld & _world)
    : world(_world)
{
    typedef std::shared_ptr<PostProcessEffect> PpePtr;
    std::vector<PpePtr> effects;


    //First, just use a contrast effect.
    effects.insert(effects.end(), PpePtr(new ContrastEffect(ContrastEffect::S_Heavy, 1)));
    effects.clear();
    effects.insert(effects.end(), PpePtr(new GaussianBlurEffect()));
    PostProcessChain * chn = new PostProcessChain(effects, world.GetWindow()->getSize().x, world.GetWindow()->getSize().y,
                                                  world.RenderTargets);
    if (chn->HasError())
    {
        errorMsg = "Error creating contrast effect chain: " + chn->GetError();
        return;
    }
    chains.insert(chains.end(), chn);
}

bool VoxelWorldPPC::RenderPostProcessing(RenderTarget & worldRender, const ProjectionInfo & info)
{
    //TODO: The post process chain should be able to take in color from one render target and depth from another.
    for (unsigned int chain = 0; chain < chains.size(); ++chain)
    {
        //Get the color/depth source based on the current chain being rendered.
        RenderTarget * srcRender;
        switch (chain)
        {
            case 0:
                srcRender = &worldRender;
                break;

            default: assert(false);
        }

        //Try to render the chain.
        if (!chains[chain]->RenderChain(&world, info, srcRender))
        {
            errorMsg = "Error rendering chain " + std::to_string(chain) + ": " + chains[chain]->GetError();
            return false;
        }
    }

    return true;
}