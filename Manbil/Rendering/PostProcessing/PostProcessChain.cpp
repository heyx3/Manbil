#include "PostProcessChain.h"


#include "../Materials/Data Nodes/ShaderGenerator.h"
#include "../../ScreenClearer.h"


PostProcessChain::PostProcessChain(std::vector<std::shared_ptr<PostProcessEffect>> effectChain,
                                   unsigned int screenWidth, unsigned int screenHeight, bool useMipmaps,
                                   const TextureSampleSettings2D & renderTargetSettings, PixelSizes pixelSize,
                                   RenderTargetManager & manager)
    : rtManager(manager), rt1(RenderTargetManager::ERROR_ID), rt2(RenderTargetManager::ERROR_ID),
      ct1(renderTargetSettings, pixelSize, useMipmaps), ct2(renderTargetSettings, pixelSize, useMipmaps)
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
    vectorBuilder.insert(vectorBuilder.end(), DataLine(VertexInputNode::GetInstanceName(), 0));
    vectorBuilder.insert(vectorBuilder.end(), DataLine(1.0f));
    DataNode::Ptr combineToPos4(new CombineVectorNode(vectorBuilder, "ppeVertPosOut"));


    //Set up DataNode material data structures.
    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetAttributeData();
    DataNode::MaterialOuts.VertexPosOutput = DataLine(combineToPos4->GetName());


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

        DataNode::MaterialOuts.FragmentOutputs.clear();
        DataNode::MaterialOuts.VertexOutputs.clear();

        //If this is a multi-pass group, create the multiple passes.
        if (passGroups[passGroup][0]->NumbPasses > 1)
        {
            assert(passGroups[passGroup].size() == 1);

            PostProcessEffect::PpePtr effct = passGroups[passGroup][0];
            effct->ChangePreviousEffect();
            DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                          ShaderOutput("out_FinalColor",
                                                                       DataLine(effct->GetName(),
                                                                                PostProcessEffect::GetColorOutputIndex())));

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

                DataNode::MaterialOuts.VertexOutputs.clear();
                DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                            ShaderOutput("fIn_UV", DataLine(VertexInputNode::GetInstanceName(), 1)));
                effct->OverrideVertexOutputs(DataNode::MaterialOuts.VertexOutputs);

                UniformDictionary unfs;
                ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(unfs, RenderingModes::RM_Opaque);
                if (!genM.ErrorMessage.empty())
                {
                    errorMsg = "Error generating shaders for pass #" + std::to_string(pass) + " of multi-pass effect '" + effct->GetName() + "': " + genM.ErrorMessage;
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

            DataNode::MaterialOuts.VertexOutputs.clear();
            DataNode::MaterialOuts.FragmentOutputs.clear();
            DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                        ShaderOutput("fIn_UV", DataLine(VertexInputNode::GetInstanceName(), 1)));
            DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                          ShaderOutput("out_FinalColor", DataLine(current->GetName(), current->GetColorOutputIndex())));
            current->OverrideVertexOutputs(DataNode::MaterialOuts.VertexOutputs);

            UniformDictionary unfs;
            ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(unfs, RenderingModes::RM_Opaque);
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


    //Create needed render targets for rendering the post-process effect.
    if (materials.size() > 0)
    {
        ct1.Create(renderTargetSettings, useMipmaps, pixelSize);
        ct1.ClearData(screenWidth, screenHeight);

        rt1 = rtManager.CreateRenderTarget(PixelSizes::PS_16U_DEPTH);
        if (rt1 == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating first render target: " + rtManager.GetError();
            return;
        }

        rtManager[rt1]->SetColorAttachment(RenderTargetTex(&ct1), true);
    }
    if (materials.size() > 1)
    {
        ct2.Create(renderTargetSettings, useMipmaps, pixelSize);
        ct2.ClearData(screenWidth, screenHeight);

        rt2 = rtManager.CreateRenderTarget(PixelSizes::PS_16U_DEPTH);
        if (rt2 == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating second render target: " + rtManager.GetError();
            return;
        }

        rtManager[rt2]->SetColorAttachment(RenderTargetTex(&ct2), true);
    }
}


bool PostProcessChain::RenderChain(SFMLOpenGLWorld * world, const ProjectionInfo & pInfo, RenderObjHandle colorIn, RenderObjHandle depthIn)
{
    if (totalPasses == 0) return true;

    //Set up the RenderInfo struct.
    Camera cam;
    cam.PerspectiveInfo = pInfo;
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
        if (source == colorIn || source == ct2.GetTextureHandle())
            dest = first;
        else dest = second;

        assert(source != 0 && dest != 0);

        //Set up the uniforms for this pass.
        const UniformList & matUniforms = materials[i]->GetUniforms();
        params.ClearUniforms();
        params.AddUniforms(oldUniforms, true);
        params.Texture2DUniforms[PostProcessEffect::ColorSampler] =
            UniformValueSampler2D(source, PostProcessEffect::ColorSampler,
                                matUniforms.FindUniform(PostProcessEffect::ColorSampler, matUniforms.Texture2DUniforms).Loc);
        params.Texture2DUniforms[PostProcessEffect::DepthSampler] =
            UniformValueSampler2D(depthIn, PostProcessEffect::DepthSampler,
                                matUniforms.FindUniform(PostProcessEffect::DepthSampler, matUniforms.Texture2DUniforms).Loc);
        params.AddUniforms(uniforms[i], true);

        //Set up the render target.
        dest->EnableDrawingInto();
        ScreenClearer().ClearScreen();

        //Render.
        if (!quad.Render(info, params, *materials[i]))
        {
            errorMsg = "Error rendering material " + std::to_string(i) + ": " + materials[i]->GetErrorMsg();
            return false;
        }

        //Disable the render target.
        dest->DisableDrawingInto(world->GetWindow()->getSize().x, world->GetWindow()->getSize().y, true);

        //Prepare for the next iteration.
        if (dest == first) source = ct1.GetTextureHandle();
        else source = ct2.GetTextureHandle();
    }

    params = oldUniforms;

    return true;
}


bool PostProcessChain::ResizeRenderTargets(unsigned int newWidth, unsigned int newHeight)
{
    if (rt1 != RenderTargetManager::ERROR_ID)
    {
        ct1.ClearData(newWidth, newHeight);
        if (!rtManager[rt1]->UpdateSize())
        {
            errorMsg = "Error resizing first render target: " + rtManager[rt1]->GetErrorMessage();
            return false;
        }
    }
    if (rt2 != RenderTargetManager::ERROR_ID)
    {
        ct2.ClearData(newWidth, newHeight);
        if (!rtManager[rt2]->UpdateSize())
        {
            errorMsg = "Error resizing second render target: " + rtManager[rt2]->GetErrorMessage();
            return false;
        }
    }

    return true;
}