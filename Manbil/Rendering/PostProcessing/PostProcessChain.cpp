#include "PostProcessChain.h"

#include "../Materials/Data Nodes/ShaderGenerator.h"


PostProcessChain::PostProcessChain(std::vector<std::shared_ptr<PostProcessEffect>> effectChain, unsigned int width, unsigned int height, RenderTargetManager & manager)
: rtManager(manager), rt1(RenderTargetManager::ERROR_ID), rt2(RenderTargetManager::ERROR_ID)
{
    std::unordered_map<RenderingChannels, DataLine> channels;

    totalPasses = 0;
    for (unsigned int effect = 0; effect < effectChain.size(); ++effect)
    {
        totalPasses += effectChain[effect]->NumbPasses;

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
                    ShaderGenerator::GenerateShaders(vs, fs, uns, RenderingModes::RM_Opaque, false, LightSettings(false), channels);
                    materials.insert(materials.end(),
                                     std::shared_ptr<Material>(new Material(vs, fs, uns,
                                                                            RenderingModes::RM_Opaque, false, LightSettings(false))));
                    uniforms.insert(uniforms.end(), uns);

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
        uniforms.insert(uniforms.end(), uns);

        //Check for shader errors.
        if (materials[materials.size() - 1]->HasError())
        {
            errorMsg = std::string() + "Error creating final material for node " + effectChain[effectChain.size() - 1]->GetName() +
                ": " + materials[materials.size() - 1]->GetErrorMsg();
            return;
        }
    }

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


bool PostProcessChain::RenderChain(SFMLOpenGLWorld * world, const RenderTarget * inWorld)
{
    //Set up the RenderInfo struct.
    Camera cam;
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

        //Set up the uniforms for this pass.
        quad.GetMesh().Uniforms.ClearUniforms();
        quad.GetMesh().Uniforms.TextureUniforms[PostProcessEffect::ColorSampler].SetData(source->GetColorTexture());
        quad.GetMesh().Uniforms.TextureUniforms[PostProcessEffect::DepthSampler].SetData(source->GetDepthTexture());
        quad.GetMesh().Uniforms.AddUniforms(oldUniforms, true);
        quad.GetMesh().Uniforms.AddUniforms(uniforms[i], true);

        //Set up the render target.
        dest->EnableDrawingInto();

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