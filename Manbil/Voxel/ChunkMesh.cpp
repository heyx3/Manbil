#include "ChunkMesh.h"

#include <thread>
#include <iostream>
#include "../Math/Higher Math/GeometricMath.h"


//Vertices and indices arrays initially reserve this amount before building vertices/indices.
const unsigned int UpperBoundSize = VoxelChunk::ChunkSize * VoxelChunk::ChunkSize * VoxelChunk::ChunkSize;


ChunkMesh::ChunkMesh(VoxelChunkManager & mangr, Vector3i chunkIndex, VoxelChunk * chunk)
: vertices(0), manager(mangr), status(ThreadStatus::TS_OFF), ChunkIndex(chunkIndex), mesh(PrimitiveTypes::Points)
{
    RenderObjHandle vbo;
    RenderDataHandler::CreateVertexBuffer<VoxelVertex>(vbo, 0, 0, RenderDataHandler::UPDATE_CONSTANTLY_AND_DRAW);
    mesh.SetVertexIndexData(VertexIndexData(0, vbo));
}

const Mesh & ChunkMesh::GetMesh(void)
{
    //If vertices were just generated, update them in OpenGL.
    if (status == ThreadStatus::TS_DONE)
        BuildBuffer();

    return mesh;
}

void ChunkMesh::RebuildMesh(bool buildNow)
{
    HangUntilThreadDone();

    status = ThreadStatus::TS_RUNNING;

    VoxelChunk * chnk = manager.GetChunk(ChunkIndex);
    if (chnk == 0) return;

    vertices.reserve(UpperBoundSize);
    chnk->BuildTriangles(vertices,
                         manager.GetChunk(Vector3i(ChunkIndex.x - 1, ChunkIndex.y, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x + 1, ChunkIndex.y, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y - 1, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y + 1, ChunkIndex.z)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y, ChunkIndex.z - 1)),
                         manager.GetChunk(Vector3i(ChunkIndex.x, ChunkIndex.y, ChunkIndex.z + 1)));


    if (buildNow) BuildBuffer();
    else status = ThreadStatus::TS_DONE;
}
void ChunkMesh::RebuildMeshOnThread(void)
{
    HangUntilThreadDone();

    ChunkMesh * thisCM = this;
    currentThread = std::thread([thisCM]() { thisCM->RebuildMesh(false); });
}

void ChunkMesh::BuildBuffer(void)
{
    HangUntilThreadDone();

    ClearAllRenderingErrors();

    RenderDataHandler::UpdateVertexBuffer(mesh.GetVertexIndexData(0).GetVerticesHandle(),
                                          vertices.data(), vertices.size(),
                                          RenderDataHandler::BufferPurpose::UPDATE_CONSTANTLY_AND_DRAW);

    std::string err = GetCurrentRenderingError();
    if (!err.empty())
        std::cout << "Error updating index/vertex buffers: " + err + "\n";

    mesh.SetVertexIndexData(VertexIndexData(vertices.size(), mesh.GetVertexIndexData(0).GetVerticesHandle()));

    vertices.clear();
    status = ThreadStatus::TS_OFF;
}

void ChunkMesh::HangUntilThreadDone(void)
{
    //Wait for the thread to finish.
    if (status.load() == ThreadStatus::TS_RUNNING && currentThread.joinable())
        currentThread.join();
    //Erase the vertices, since they have to be regenerated.
    if (status.load() == ThreadStatus::TS_DONE)
        vertices.clear();

    status = ThreadStatus::TS_OFF;
}