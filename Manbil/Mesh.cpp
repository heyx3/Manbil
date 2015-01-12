#include "Mesh.h"


void Mesh::DestroySubMeshes(void)
{
    while (!SubMeshes.empty())
    {
        RenderDataHandler::DeleteBuffer((SubMeshes.end() - 1)->GetVerticesHandle());
        RenderDataHandler::DeleteBuffer((SubMeshes.end() - 1)->GetIndicesHandle());
        SubMeshes.erase(SubMeshes.end() - 1);
    }
}