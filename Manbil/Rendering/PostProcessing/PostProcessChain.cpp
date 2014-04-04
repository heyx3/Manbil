#include "PostProcessChain.h"

#include "../Materials/Data Nodes/ShaderGenerator.h"
#include "../../ScreenClearer.h"


PostProcessChain::PostProcessChain(std::vector<std::shared_ptr<PostProcessEffect>> effectChain, unsigned int width, unsigned int height, RenderTargetManager & manager)
: rtManager(manager), rt1(RenderTargetManager::ERROR_ID), rt2(RenderTargetManager::ERROR_ID)
{
    //First separate the effects into "pass groups" -- a chain of effects grouped by pass.
    //Multi-pass effects are each in their own group.

    //Build each pass group.
    std::vector<std::vector<PostProcessEffect::PpePtr>> passGroups;
    unsigned int passGroup = 0;
    for (unsigned int effect = 0; effect < effectChain.size(); ++effect)
    {
        //If this is the start of a new pass, create the collection of effects for it.
        if (passGroup >= passGroups.size())
            passGroups.insert(passGroups.end(), std::vector<PostProcessEffect::PpePtr>());

        //If this effect only has one pass, just put it in the current pass group.
        if (effectChain[effect]->NumbPasses == 1)
        {
            passGroups[passGroup].insert(passGroups[passGroup].end(), effectChain[effect]);
        }
        //Otherwise, create a new pass group just for this effect.
        else
        {
            passGroup += 1;
            passGroups.insert(passGroups.end(), std::vector<std::shared_ptr<PostProcessEffect>>());
            passGroups[passGroup].insert(passGroups[passGroup].end(), effectChain[effect]);

            passGroup += 1;
        }
    }

    //Assemble each pass group into a material.
    totalPasses = passGroups.size();
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
            channels[RenderingChannels::RC_Diffuse] = DataLine(effct, PostProcessEffect::GetColorOutputIndex());

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

                UniformDictionary unfs;
                materials.insert(materials.end(), std::shared_ptr<Material>(ShaderGenerator::GenerateMaterial(channels, unfs, RenderingModes::RM_Opaque, false, LightSettings(false))));
                uniforms.insert(uniforms.end(), unfs);
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


            channels[RenderingChannels::RC_Diffuse] = DataLine(current, PostProcessEffect::GetColorOutputIndex());


            //Now create the material.
            UniformDictionary unfs;
            materials.insert(materials.end(), std::shared_ptr<Material>(ShaderGenerator::GenerateMaterial(channels, unfs, RenderingModes::RM_Opaque, false, LightSettings(false))));
            uniforms.insert(uniforms.end(), unfs);
            if (materials[materials.size() - 1]->HasError())
            {
                errorMsg = std::string() + "Error creating material #" + std::to_string(materials.size()) + ": " + materials[materials.size() - 1]->GetErrorMsg();
                return;
            }
        }
    }

    /*
    totalPasses = BasicMath::Sign(effectChain.size());
    for (unsigned int effect = 0; effect < effectChain.size(); ++effect)
    {
        totalPasses += BasicMath::Max((unsigned int)0, effectChain[effect]->NumbPasses - 1);

        //If there's more than one pass, we have to create a new material for each pass.
        if (effectChain[effect]->NumbPasses > 1)
        {
            for (unsigned int pass = 0; pass < effectChain[effect]->NumbPasses; ++pass)
            {
                //If this is the first pass, just build on top of the previous effect as usual.
                if (pass == 0)
                {
                    //If this is the first effect in a new material, use the default input.
                    if (channels.find(RenderingChannels::RC_Diffuse) == channels.end())
                        channels[RenderingChannels::RC_Diffuse] = DataLine(effectChain[effect]->ReplaceWithDefaultInput(), 0);
                    //Otherwise, use the previous effect's output as the input into this effect.
                    else channels[RenderingChannels::RC_Diffuse] = DataLine(effectChain[effect], 0);
                }
                //Otherwise, create a new material and then reset the diffuse channel.
                else
                {
                    std::string vs, fs;
                    UniformDictionary uns;
                    effectChain[effect]->CurrentPass = pass - 1;
                    ShaderGenerator::GenerateShaders(vs, fs, uns, RenderingModes::RM_Opaque, false, LightSettings(false), channels);
                    effectChain[effect]->CurrentPass = pass;
                    materials.insert(materials.end(),
                                     std::shared_ptr<Material>(new Material(vs, fs, uns,
                                                                            RenderingModes::RM_Opaque, false, LightSettings(false))));
                    uniforms.insert(uniforms.end(), UniformDictionary());

                    //Check for shader errors.
                    if (materials[materials.size() - 1]->HasError())
                    {
                        errorMsg = std::string() + "Error creating material #" + std::to_string(materials.size()) +
                                   ", pass " + std::to_string(pass) + " for node " + effectChain[effect]->GetName() +
                                   ": " + materials[materials.size() - 1]->GetErrorMsg();
                        return;
                    }

                    channels[RenderingChannels::RC_Diffuse] = DataLine(effectChain[effect]->ReplaceWithDefaultInput(), 0);
                }
            }
        }
        //Otherwise, just add this effect on top of the previous effect.
        else
        {
            //If this is the first effect in a new material, use the default input.
            if (channels.find(RenderingChannels::RC_Diffuse) == channels.end())
                channels[RenderingChannels::RC_Diffuse] = DataLine(effectChain[effect]->ReplaceWithDefaultInput(), 0);
            //Otherwise, use the previous effect's output as the input into this effect.
            else channels[RenderingChannels::RC_Diffuse] = DataLine(effectChain[effect], 0);
        }
    }

    //Create a material from the last pass.
    if (channels.find(RenderingChannels::RC_Diffuse) != channels.end())
    {
        std::string vs, fs;
        UniformDictionary uns;
        ShaderGenerator::GenerateShaders(vs, fs, uns, RenderingModes::RM_Opaque, false, LightSettings(false), channels);
        materials.insert(materials.end(),
                         std::shared_ptr<Material>(new Material(vs, fs, uns,
                                                   RenderingModes::RM_Opaque, false, LightSettings(false))));
        uniforms.insert(uniforms.end(), UniformDictionary());

        //Check for shader errors.
        if (materials[materials.size() - 1]->HasError())
        {
            errorMsg = std::string() + "Error creating final material for node " + effectChain[effectChain.size() - 1]->GetName() +
                ": " + materials[materials.size() - 1]->GetErrorMsg();
            return;
        }
    }
    */


    //Create needed render targets for rendering the post-process effect.
    if (materials.size() > 0)
    {
        rt1 = rtManager.CreateRenderTarget(width, height, true, true);
        if (rt1 == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating first render target (" + std::to_string(width) + "x" +
                        std::to_string(height) + "): " + rtManager.GetError();
            return;
        }
    }
    if (materials.size() > 1)
    {
        rt2 = rtManager.CreateRenderTarget(width, height, true, true);
        if (rt1 == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating second render target (" + std::to_string(width) + "x" +
                        std::to_string(height) + "): " + rtManager.GetError();
            return;
        }
    }
}


bool PostProcessChain::RenderChain(SFMLOpenGLWorld * world, const ProjectionInfo & pInfo, const RenderTarget * inWorld)
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

    //The input and output render targets for a single material render.
    const RenderTarget * source = inWorld,
                       * dest = first;

    UniformDictionary oldUniforms = quad.GetMesh().Uniforms;

    //Render each material in turn.
    for (unsigned int i = 0; i < materials.size(); ++i)
    {
        if (source == inWorld || source == second)
            dest = first;
        else dest = second;

        assert(source != 0 && dest != 0);

        //Set up the uniforms for this pass.
        const UniformList & matUniforms = materials[i]->GetUniforms(RenderPasses::BaseComponents);
        quad.GetMesh().Uniforms.ClearUniforms();
        quad.GetMesh().Uniforms.TextureUniforms[PostProcessEffect::ColorSampler] =
            UniformSamplerValue(source->GetColorTexture(), PostProcessEffect::ColorSampler,
                                matUniforms.FindUniform(PostProcessEffect::ColorSampler, matUniforms.TextureUniforms).Loc);
        quad.GetMesh().Uniforms.TextureUniforms[PostProcessEffect::DepthSampler] =
            UniformSamplerValue(source->GetDepthTexture(), PostProcessEffect::DepthSampler,
                                matUniforms.FindUniform(PostProcessEffect::DepthSampler, matUniforms.TextureUniforms).Loc);
        quad.GetMesh().Uniforms.AddUniforms(oldUniforms, true);
        quad.GetMesh().Uniforms.AddUniforms(uniforms[i], true);

        //Set up the render target.
        dest->EnableDrawingInto();
        ScreenClearer().ClearScreen();

        //Render.
        if (!quad.Render(RenderPasses::BaseComponents, info, *materials[i]))
        {
            errorMsg = "Error rendering material " + std::to_string(i) + ": " + materials[i]->GetErrorMsg();
            return false;
        }

        //Disable the render target.
        dest->DisableDrawingInto(world->GetWindow()->getSize().x, world->GetWindow()->getSize().y);

        //Prepare for the next iteration.
        if (dest == first) source = first;
        else source = second;
    }

    quad.GetMesh().Uniforms = oldUniforms;

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











/*

PostProcessEffect::PostProcessEffect(unsigned int w, unsigned int h, std::vector<RenderingPass> passes)
    : errorMsg(""), material(passes), renderTarget(0), screenMesh(PrimitiveTypes::Triangles)
{
    //Make sure the material is valid.
    if (material.HasError())
    {
        errorMsg = "Error creating material: ";
        errorMsg += material.GetErrorMessage();
        return;
    }

    //Create the render target.
    usesCol = material.HasUniform("u_sampler0");
    usesDepth = material.HasUniform("u_sampler1");
    renderTarget = new RenderTarget(w, h, usesCol, usesDepth);
    if (renderTarget->HasError())
    {
        errorMsg = "Error creating render target: ";
        errorMsg += renderTarget->GetErrorMessage();
        return;
    }

    //Create the vbo.
    RenderObjHandle vbo;
    Vertex vertices[6];
    vertices[0] = Vertex(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(0.0f, 0.0f));
    vertices[1] = Vertex(Vector3f(-1.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
    vertices[2] = Vertex(Vector3f(1.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f));
    vertices[3] = vertices[0];
    vertices[4] = Vertex(Vector3f(1.0f, -1.0f, 0.0f), Vector2f(1.0f, 0.0f));
    vertices[5] = vertices[2];
    RenderDataHandler::CreateVertexBuffer(vbo, vertices, 6, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

    VertexIndexData vid(6, vbo);
    screenMesh.SetVertexIndexData(&vid, 1);


    //Set up the screen quad mesh to use the render target textures.
    for (int i = 0; i < passes.size(); ++i)
    {
        PassSamplers samplers;
        samplers[0] = renderTarget->GetColorTexture();
        samplers[1] = renderTarget->GetDepthTexture();
        screenMesh.TextureSamplers.insert(screenMesh.TextureSamplers.end(), samplers);

        material.SetTexture(i, 0, renderTarget->GetColorTexture());
        material.SetTexture(i, 1, renderTarget->GetDepthTexture());
    }
}

std::string PostProcessEffect::GetErrorMessage(void) const
{
    if (errorMsg.empty())
        if (!material.HasError())
            if (!renderTarget->HasError())
                return errorMsg;
            else return renderTarget->GetErrorMessage();
        else return material.GetErrorMessage();
    else return errorMsg;
}

PostProcessEffect::~PostProcessEffect(void)
{
    if (renderTarget != 0) delete renderTarget;

    //Delete the vertex buffer and possibly an index buffer if the quad used one.
    GLuint buf1 = screenMesh.GetVertexIndexData(0).GetVerticesHandle(),
           buf2 = screenMesh.GetVertexIndexData(0).GetIndicesHandle();
    GLuint buffs[] = { buf1, buf2 };

    glDeleteBuffers((screenMesh.GetVertexIndexData(0).UsesIndices() ? 2 : 1), buffs);
}

void PostProcessEffect::RenderEffect(const RenderInfo & info, Vector2f screenOffset, Vector2f screenScale)
{
    //Make sure this effect isn't rendering to its own frame buffer.
    int currentFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);
    if (currentFramebuffer == renderTarget->GetFramebuffer())
    {
        errorMsg = "Tried to render to the frame buffer that this effect reads from!";
        return;
    }


    ClearAllRenderingErrors();

    screenMesh.Transform.SetPosition(Vector3f(screenOffset.x, screenOffset.y, 0.0f));
    screenMesh.Transform.SetScale(Vector3f(screenScale.x, screenScale.y, 1.0f));
    std::vector<const Mesh*> meshes;
    meshes.insert(meshes.end(), &screenMesh);

    if (!material.Render(info, meshes))
    {
        errorMsg = "Error rendering post-process quad: " + material.GetErrorMessage();
    }
}

*/