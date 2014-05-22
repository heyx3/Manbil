#include "ChunkMesh.h"

#include <thread>
#include <iostream>
#include "../Math/Higher Math/GeometricMath.h"


//Vertices and indices arrays initially reserve this amount before building vertices/indices.
const unsigned int UpperBoundSize = VoxelChunk::ChunkSize * VoxelChunk::ChunkSize * VoxelChunk::ChunkSize;


ChunkMesh::ChunkMesh(VoxelChunkManager & mangr, Vector3i chunkIndex, VoxelChunk * chunk)
: vertices(0), indices(0), manager(mangr), status(ThreadStatus::TS_OFF), ChunkIndex(chunkIndex)
{
    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer<Vertex>(vbo);
    RenderDataHandler::CreateIndexBuffer(ibo);
    vid = VertexIndexData(0, vbo, 0, ibo);
}

VertexIndexData ChunkMesh::GetVID(void)
{
    //If vertices were just generated, update them in OpenGL.
    if (status == ThreadStatus::TS_DONE)
        BuildBuffers();

    return vid;
}

void ChunkMesh::RebuildMesh(bool buildNow)
{
    HangUntilThreadDone();

    status = ThreadStatus::TS_RUNNING;

    VoxelChunk * chnk = manager.GetChunk(ChunkIndex);
    if (chnk == 0) return;

    indices.reserve(UpperBoundSize);
    vertices.reserve(UpperBoundSize);
    chnk->BuildTriangles(vertices, indices,
                         manager.GetChunk(Vector3i(ChunkIndex.x - 1, ChunkIndex.y, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x + 1, ChunkIndex.y, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y - 1, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y + 1, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y, ChunkIndex.z - 1)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y, ChunkIndex.z + 1)));


    if (buildNow) BuildBuffers();
    else status = ThreadStatus::TS_DONE;
}
void ChunkMesh::RebuildMeshOnThread(void)
{
    HangUntilThreadDone();

    ChunkMesh * thisCM = this;
    currentThread = std::thread([thisCM]() { thisCM->RebuildMesh(false); });
}

void ChunkMesh::BuildBuffers(void)
{
    HangUntilThreadDone();

    ClearAllRenderingErrors();

    RenderDataHandler::UpdateVertexBuffer(vid.GetVerticesHandle(),
                                          vertices.data(), vertices.size(),
                                          RenderDataHandler::BufferPurpose::UPDATE_CONSTANTLY_AND_DRAW);
    RenderDataHandler::UpdateIndexBuffer(vid.GetIndicesHandle(),
                                         indices.data(), indices.size(),
                                         RenderDataHandler::BufferPurpose::UPDATE_CONSTANTLY_AND_DRAW);

    std::string err = GetCurrentRenderingError();
    if (!err.empty())
        std::cout << "Error updating index/vertex buffers: " + err + "\n";

    vid = VertexIndexData(vertices.size(), vid.GetVerticesHandle(), indices.size(), vid.GetIndicesHandle());

    vertices.clear();
    indices.clear();
    status = ThreadStatus::TS_OFF;
}

void ChunkMesh::HangUntilThreadDone(void)
{
    if (status.load() == ThreadStatus::TS_RUNNING && currentThread.joinable())
        currentThread.join();
    if (status.load() == ThreadStatus::TS_DONE)
    {
        vertices.clear();
        indices.clear();
    }

    status = ThreadStatus::TS_OFF;
}