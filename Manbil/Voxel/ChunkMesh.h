#pragma once

#include <thread>
#include <atomic>
#include "VoxelChunk.h"
#include "VoxelChunkManager.h"
#include "../Mesh.h"


//Manages a chunk's mesh. Able to rebuild the chunk's mesh on another thread.
class ChunkMesh
{
public:

    Vector3i ChunkIndex;


    //Creates OpenGL vertex buffer.
    ChunkMesh(VoxelChunkManager & manager, Vector3i chunkIndex, VoxelChunk * chunk = 0);
    ~ChunkMesh(void) { HangUntilThreadDone(); RenderDataHandler::DeleteBuffer(mesh.GetVertexIndexData(0).GetVerticesHandle()); }


    //Rebuilds the chunk's mesh.
    //If "buildNow" is true, the OpenGL vertex buffer is immediately set.
    //Otherwise, the generated mesh is stored and the buffer will be set next time "GetVID()" is called.
    void RebuildMesh(bool buildNow = true);
    //Rebuilds the chunk's mesh in another thread.
    //Once the vertices are generated, the OpenGL vertex buffer
    //   will be automatically updated on the next "GetVID()" call.
    void RebuildMeshOnThread(void);

    //Must be called on the main thread, as this call may invoke some OpenGL calls
    //   if the vertices were just regenerated.
    const Mesh & GetMesh(void);


private:

    VoxelChunkManager & manager;

    Mesh mesh;
    

    //Asynchronous or delayed vertex generation.

    std::vector<VoxelVertex> vertices;

    enum ThreadStatus
    {
        //Neither running the thread nor waiting for buffer rebuild.
        TS_OFF,
        //Running the thread; currently generating the vertices/indices.
        TS_RUNNING,
        //Done the thread; waiting for the next GetVID() call to set the OpenGL buffers.
        TS_DONE,
    };
    std::atomic<ThreadStatus> status;
    std::thread currentThread;

    //Puts the vertex data into the OpenGL vertex buffer.
    void BuildBuffer(void);

    //Waits for the current building thread to finish if it is running, then clears the vertex buffer.
    void HangUntilThreadDone(void);
};