#include "PostProcessChain.h"

#include "../Materials/Data Nodes/ShaderGenerator.h"
#include "../../ScreenClearer.h"


PostProcessChain::PostProcessChain(std::vector<std::shared_ptr<PostProcessEffect>> effectChain, unsigned int width, unsigned int height, RenderTargetManager & manager)
    : rtManager(manager), rt1(RenderTargetManager::ERROR_ID), rt2(RenderTargetManager::ERROR_ID)
{
    //TODO: Change to using a simple vector of the following struct instead of the "pass groups" thing.
    struct MiniEffect
    {
    public:
        PostProcessEffect* Effect;
        unsigned int Pass;
        MiniEffect(void) : Effect(0), Pass(0) { }
        MiniEffect(PostProcessEffect * effect, unsigned int pass = 1) : Effect(effect), Pass(pass) { assert(effect != 0 && Pass > 0); }
        bool IsBreak(void) const { return Effect == 0 || Pass == 0; }
    };


    //First separate the effects into "pass groups" -- a chain of effects grouped by pass.
    //Multi-pass effects are each in their own group.

    //All passes don't need any kind of transformation for the vertices.
    std::vector<DataLine> vectorBuilder;
    vectorBuilder.insert(vectorBuilder.end(), DataLine(DataNodePtr(new VertexInputNode(DrawingQuad::GetAttributeData())), 0));
    vectorBuilder.insert(vectorBuilder.end(), DataLine(1.0f));
    DataLine objectPos4(DataNodePtr(new CombineVectorNode(vectorBuilder)), 0);

    //Build each pass group.
    std::vector<std::vector<PostProcessEffect::PpePtr>> passGroups;
    unsigned int passGroup = 0;
    totalPasses = 0;
    for (unsigned int effect = 0; effect < effectChain.size(); ++effect)
    {
        //If this is the start of a new pass, create the collection of effects for it.
        if (passGroup >= passGroups.size())
        {
            totalPasses += 1;
            passGroups.insert(passGroups.end(), std::vector<PostProcessEffect::PpePtr>());
        }

        //If this effect only has one pass, just put it in the current pass group.
        if (effectChain[effect]->NumbPasses == 1)
        {
            passGroups[passGroup].insert(passGroups[passGroup].end(), effectChain[effect]);
        }
        //Otherwise, create a new pass group just for this effect.
        else
        {
            totalPasses += effectChain[effect]->NumbPasses - 2;
            if (effect == effectChain.size() - 1) totalPasses += 1;

            if (passGroups[passGroup].size() > 0)
            {
                passGroup += 1;
                passGroups.insert(passGroups.end(), std::vector<std::shared_ptr<PostProcessEffect>>());
            }
            passGroups[passGroup].insert(passGroups[passGroup].end(), effectChain[effect]);

            passGroup += 1;
        }
    }

    //Assemble each pass group into a material.
    for (passGroup = 0; passGroup < passGroups.size(); ++passGroup)
    {
        assert(passGroups[passGroup].size() > 0);

        std::unordered_map<RenderingChannels, DataLine> channels;

        //If this is a multi-pass group, create the multiple passes.
        if (passGroups[passGroup][0]->NumbPasses > 1)
        {
            assert(passGroups[passGroup].size() == 1);

            PostProcessEffect::PpePtr effct = passGroups[passGroup][0];
            effct->ChangePreviousEffect();
            channels[RenderingChannels::RC_Color] = DataLine(effct, PostProcessEffect::GetColorOutputIndex());

            //If there is a group before/after this, skip the first/last pass, since it will be lumped in with that other group.
            unsigned int startPass = 1,
                         endPass = effct->NumbPasses;
            if (passGroup > 0)
                startPass += 1;
            if (passGroup < passGroups.size() - 1)
                endPass -= 1;

            for (unsigned int pass = startPass; pass <= endPass; ++pass)
            {
                effct->CurrentPass = pass;

                channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(DataNodePtr(new VertexInputNode(DrawingQuad::GetAttributeData())), 1);
                channels[RenderingChannels::RC_VertexPosOutput] = objectPos4;
                effct->OverrideVertexOutputs(channels);

                UniformDictionary unfs;
                ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(channels, unfs, DrawingQuad::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
                if (!genM.ErrorMessage.empty())
                {
                    errorMsg = std::string() + "Error generating shaders for pass #" + std::to_string(pass) + " of multi-pass effect '" + effct->GetName() + "': " + genM.ErrorMessage;
                    return;
                }

                materials.insert(materials.end(), std::shared_ptr<Material>(genM.Mat));
                params.AddUniforms(unfs, true);
                uniforms.insert(uniforms.end(), UniformDictionary());

                if (materials[materials.size() - 1]->HasError())
                {
                    errorMsg = std::string() + "Error creating pass #" + std::to_string(pass) + " of multi-pass effect '" + effct->GetName() + "': " + materials[materials.size() - 1]->GetErrorMsg();
                    return;
                }
            }
        }
        //Otherwise, build up this pass using multiple effects.
        else
        {
            PostProcessEffect::PpePtr prev, current;

            //If there was a multi-pass effect before this group, put its final pass at the beginning of this group.
            if (passGroup > 0 && passGroups[passGroup - 1][0]->NumbPasses > 1)
            {
                prev = passGroups[passGroup - 1][0];
                prev->CurrentPass = prev->NumbPasses;
                prev->ChangePreviousEffect();
            }

            //Set each effect to happen on top of the previous.
            for (unsigned int effect = 0; effect < passGroups[passGroup].size(); ++effect)
            {
                current = passGroups[passGroup][effect];
                current->CurrentPass = 1;
                current->ChangePreviousEffect(prev);

                prev = current;
            }
            assert(current.get() != 0);

            //If the next pass group is a multi-pass effect, put its first pass on the end of this group.
            if (passGroup < passGroups.size() - 1 && passGroups[passGroup + 1][0]->NumbPasses > 1)
            {
                current = passGroups[passGroup + 1][0];
                current->CurrentPass = 1;
                current->ChangePreviousEffect(prev);

                prev = current;
            }


            //Now create the material.

            channels[RenderingChannels::RC_Color] = DataLine(current, PostProcessEffect::GetColorOutputIndex());
            channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(DataNodePtr(new VertexInputNode(DrawingQuad::GetAttributeData())), 1);
            channels[RenderingChannels::RC_VertexPosOutput] = objectPos4;
            current->OverrideVertexOutputs(channels);

            UniformDictionary unfs;
            ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(channels, unfs, DrawingQuad::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
            if (!genM.ErrorMessage.empty())
            {
                errorMsg = std::string() + "Error generating shaders for material #" + std::to_string(materials.size()) + ": " + genM.ErrorMessage;
                return;
            }

            materials.insert(materials.end(), std::shared_ptr<Material>(genM.Mat));
            params.AddUniforms(unfs, true);
            uniforms.insert(uniforms.end(), UniformDictionary());

            if (materials[materials.size() - 1]->HasError())
            {
                errorMsg = std::string() + "Error creating material #" + std::to_string(materials.size()) + ": " + materials[materials.size() - 1]->GetErrorMsg();
                return;
            }
        }
    }


    //Set up the render target settings.
    //TODO: Parameterize the render target settings somehow (probably the PPC constructor). Will also need to change all instances of the code "GetColorTextures()[0]".
    RendTargetColorTexSettings cts;
    cts.ColorAttachment = 0;
    cts.Settings.Width = width;
    cts.Settings.Height = height;
    cts.Settings.Size = ColorTextureSettings::CTS_32;
    cts.Settings.GenerateMipmaps = false;
    cts.Settings.BaseSettings = TextureSettings(TextureSettings::MTF_NEAREST, TextureSettings::MTF_CLAMP);
    RendTargetDepthTexSettings dts;
    dts.UsesDepthTexture = true;
    dts.Settings.Size = DepthTextureSettings::DTS_24;
    dts.Settings.GenerateMipmaps = false;
    dts.Settings.BaseSettings = TextureSettings(TextureSettings::MTF_NEAREST, TextureSettings::MTF_CLAMP);

    //Create needed render targets for rendering the post-process effect.
    if (materials.size() > 0)
    {
        rt1 = rtManager.CreateRenderTarget(cts, dts);
        if (rt1 == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating first render target (" + std::to_string(width) + "x" +
                        std::to_string(height) + "): " + rtManager.GetError();
            return;
        }
    }
    if (materials.size() > 1)
    {
        rt2 = rtManager.CreateRenderTarget(cts, dts);
        if (rt1 == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating second render target (" + std::to_string(width) + "x" +
                        std::to_string(height) + "): " + rtManager.GetError();
            return;
        }
    }
}


bool PostProcessChain::RenderChain(SFMLOpenGLWorld * world, const ProjectionInfo & pInfo, RenderObjHandle colorIn, RenderObjHandle depthIn)
{
    if (totalPasses == 0) return true;

    //Set up the RenderInfo struct.
    Camera cam;
    cam.Info = pInfo;
    TransformObject trans;
    Matrix4f identity;
    identity.SetAsIdentity();
    RenderInfo info(world, &cam, &trans, &identity, &identity, &identity);

    const RenderTarget * first = rtManager[rt1],
                       * second = rtManager[rt2];

    //The input and output color render targets for a single material render.
    RenderObjHandle source = colorIn;
    const RenderTarget * dest;

    UniformDictionary oldUniforms = params;

    //Render each material in turn.
    for (unsigned int i = 0; i < materials.size(); ++i)
    {
        if (source == colorIn || source == second->GetColorTextures()[0])
            dest = first;
        else dest = second;

        assert(source != 0 && dest != 0);

        //Set up the uniforms for this pass.
        const UniformList & matUniforms = materials[i]->GetUniforms(RenderPasses::BaseComponents);
        params.ClearUniforms();
        params.AddUniforms(oldUniforms, true);
        params.TextureUniforms[PostProcessEffect::ColorSampler] =
            UniformSamplerValue(source, PostProcessEffect::ColorSampler,
                                matUniforms.FindUniform(PostProcessEffect::ColorSampler, matUniforms.TextureUniforms).Loc);
        params.TextureUniforms[PostProcessEffect::DepthSampler] =
            UniformSamplerValue(depthIn, PostProcessEffect::DepthSampler,
                                matUniforms.FindUniform(PostProcessEffect::DepthSampler, matUniforms.TextureUniforms).Loc);
        params.AddUniforms(uniforms[i], true);

        //Set up the render target.
        dest->EnableDrawingInto();
        ScreenClearer().ClearScreen();

        //Render.
        if (!quad.Render(RenderPasses::BaseComponents, info, params, *materials[i]))
        {
            errorMsg = "Error rendering material " + std::to_string(i) + ": " + materials[i]->GetErrorMsg();
            return false;
        }

        //Disable the render target.
        dest->DisableDrawingInto(world->GetWindow()->getSize().x, world->GetWindow()->getSize().y);

        //Prepare for the next iteration.
        if (dest == first) source = first->GetColorTextures()[0];
        else source = second->GetColorTextures()[0];
    }

    params = oldUniforms;

    return true;
}


bool PostProcessChain::ResizeRenderTargets(unsigned int newWidth, unsigned int newHeight)
{
    if (rt1 != RenderTargetManager::ERROR_ID)
    {
        if (!rtManager.ResizeTarget(rt1, newWidth, newHeight))
        {
            errorMsg = "Error resizing first render target: " + rtManager.GetError();
            return false;
        }
    }
    if (rt2 != RenderTargetManager::ERROR_ID)
    {
        if (!rtManager.ResizeTarget(rt2, newWidth, newHeight))
        {
            errorMsg = "Error resizing second render target: " + rtManager.GetError();
            return false;
        }
    }

    return true;
}