#include "PostProcessEffect.h"
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