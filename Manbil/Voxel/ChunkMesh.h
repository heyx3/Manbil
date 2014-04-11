#pragma once

#include <thread>
#include <atomic>
#include "VoxelChunk.h"
#include "VoxelChunkManager.h"
#include "../Vertex.h"
#include "../Mesh.h"


//Manages a chunk's mesh. Able to rebuild the chunk's mesh on another thread.
class ChunkMesh
{
public:

    Vector3i ChunkIndex;


    //Creates OpenGL vertex/index buffers.
    ChunkMesh(VoxelChunkManager & manager, VoxelChunk * chunk = 0);
    ~ChunkMesh(void) { HangUntilThreadDone(); RenderDataHandler::DeleteBuffer(vid.GetVerticesHandle()); RenderDataHandler::DeleteBuffer(vid.GetIndicesHandle()); }


    //Rebuilds the chunk's mesh.
    //If "buildNow" is true, the OpenGL vertex/index buffers are immediately set.
    //Otherwise, the generated mesh is stored and the buffers will be set next time "GetVID()" is called.
    void RebuildMesh(bool buildNow = true);
    //Rebuilds the chunk's mesh in another thread.
    //Once the vertices/indices are generated, the OpenGL vertex/index buffers
    //   will be automatically updated on the next "GetVID()" call.
    void RebuildMeshOnThread(void);

    //Must be called on the main thread, as this call may invoke some OpenGL calls
    //   if the vertices/indices were just regenerated.
    VertexIndexData GetVID(void);


private:

    VoxelChunkManager & manager;

    VertexIndexData vid;
    

    //Asynchronous or delayed vertex/index generation.

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

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

    void BuildBuffers(void);
    //Waits for the current building thread to finish if it is running.
    //Then clears "vertices" and "indices".
    void HangUntilThreadDone(void);
};