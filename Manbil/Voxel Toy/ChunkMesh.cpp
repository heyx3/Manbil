#include "ChunkMesh.h"

#include <thread>
#include <iostream>
#include "../Math/Higher Math/Geometryf.h"



ChunkMesh::ChunkMesh(VoxelChunkManager& mangr, Vector3i chunkIndex, VoxelChunk* chunk)
    : vertices(0), manager(mangr), status(ThreadStatus::TS_OFF), ChunkIndex(chunkIndex)
{
    mesh.SubMeshes.push_back(MeshData(false, PT_POINTS));
    mesh.SubMeshes[0].SetVertexData((VoxelVertex*)0, 0, MeshData::BUF_DYNAMIC,
                                    VoxelVertex::GetVertexAttributes());
}

const Mesh& ChunkMesh::GetMesh(void)
{
    //If vertices were just generated, update them in OpenGL.
    if (status == ThreadStatus::TS_DONE)
    {
        BuildBuffer();
    }

    return mesh;
}

void ChunkMesh::RebuildMesh(bool buildNow)
{
    HangUntilThreadDone();

    status = ThreadStatus::TS_RUNNING;

    VoxelChunk* chnk = manager.GetChunk(ChunkIndex);
    if (chnk == 0)
    {
        return;
    }

    vertices.reserve(chnk->GetNumbSolidVoxels());
    chnk->BuildTriangles(vertices,
                         manager.GetChunk(Vector3i(ChunkIndex.x - 1, ChunkIndex.y, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x + 1, ChunkIndex.y, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y - 1, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y + 1, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y, ChunkIndex.z - 1)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y, ChunkIndex.z + 1)));


    if (buildNow)
    {
        BuildBuffer();
    }
    else
    {
        status = ThreadStatus::TS_DONE;
    }
}
void ChunkMesh::RebuildMeshOnThread(void)
{
    HangUntilThreadDone();

    ChunkMesh* thisCM = this;
    currentThread = std::thread([thisCM]() { thisCM->RebuildMesh(false); });
}

void ChunkMesh::BuildBuffer(void)
{
    HangUntilThreadDone();

    ClearAllRenderingErrors();

    mesh.SubMeshes[0].SetVertexData(vertices, MeshData::BUF_DYNAMIC,
                                    VoxelVertex::GetVertexAttributes());

    std::string err = GetCurrentRenderingError();
    if (!err.empty())
    {
        std::cout << "Error updating index/vertex buffers: " + err + "\n";
    }

    vertices.clear();
    status = ThreadStatus::TS_OFF;
}

void ChunkMesh::HangUntilThreadDone(void)
{
    //Wait for the thread to finish.
    if (status.load() == ThreadStatus::TS_RUNNING && currentThread.joinable())
    {
        currentThread.join();
    }
    //Erase the vertices, since they have to be regenerated.
    if (status.load() == ThreadStatus::TS_DONE)
    {
        vertices.clear();
    }

    status = ThreadStatus::TS_OFF;
}