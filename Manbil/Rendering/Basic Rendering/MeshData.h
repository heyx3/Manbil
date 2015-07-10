#pragma once

#include <vector>
#include "OpenGLIncludes.h"
#include "RenderIOAttributes.h"


//A collection of vertices and (optionally) indices for rendering.
//Can optionally store a copy of the data on RAM for quick CPU access.
//If any code binds vertex/index buffers without using this class, this class's behavior
//    is undefined (unless the binds are set back to their old values as soon as they're done being used).
struct MeshData
{
public:

    //Different usage patterns for the mesh data.
    //Only affects performance, not functionality.
    enum BufferUsageFrequency
    {
        //The data will not need to be modified after it's first set.
        BUF_STATIC,
        //The data will be modified occasionally, but not constantly.
        BUF_DYNAMIC,
        //The data will be modified as often as it is used.
        BUF_STREAM,
    };


    static RenderObjHandle GetCurrentVHandle(void) { return currentVHandle; }
    static RenderObjHandle GetCurrentIHandle(void) { return currentIHandle; }
    

    PrimitiveTypes PrimType;


    //Takes in whether to store a local CPU copy of the mesh data for easy querying.
    MeshData(bool shouldStoreData, PrimitiveTypes primType);
    MeshData(MeshData&& other) { *this = std::move(other); }
    ~MeshData(void);

    MeshData(const MeshData& cpy) = delete;
    MeshData& operator=(const MeshData& cpy) = delete;


    MeshData& operator=(MeshData&& other);
    void MoveTo(MeshData& newData);


    void SetStoresData(bool newValue);
    bool GetStoresData(void) const { return storesData; }

    //Gets whether this mesh data uses indices.
    bool GetUsesIndices(void) const { return indicesHandle != 0; }
    
    unsigned int GetNVertices(void) const { return nVertices; }
    unsigned int GetNIndices(void) const { return nIndices; }
    
    RenderObjHandle GetVerticesHandle(void) const { return verticesHandle; }
    RenderObjHandle GetIndicesHandle(void) const { return indicesHandle; }

    RenderIOAttributes GetVertexAttribute(void) const { return vertexAttributes; }


    //Sets the range of indices to use (or vertices, if not using indices).
    //Defaults to the entire range of available vertices/indices.
    //Every time the vertex or index buffer is changed, this range will reset to cover all elements.
    void SetUseRange(unsigned int start, unsigned int count);

    unsigned int GetRangeStart(void) const { return start; }
    unsigned int GetRangeSize(void) const { return range; }


    //The type of vertex this instance is storing.
    template<typename VertexType>
    //Returns a pointer to a copy of the vertex data. Assumes that this instance stores the mesh data.
    const VertexType* GetVertexData(void) const { return (VertexType*)verticesData.data(); }
    //Returns a pointer to a copy of the index data.
    //Assumes that this instance's mesh data uses indices, and that this instance stores the mesh data.
    const unsigned int* GetIndexData(void) const { return indicesData.data(); }


    //The type of vertex this instance is storing.
    template<typename VertexType>
    //Reads this instance's vertex data into the given std::vector.
    //If the mesh data is not stored locally, it will be read from the GPU.
    //Otherwise, it will be copied in from this instance's local copy of the data.
    //Assumes that the given vertex type matches the vertices stored by this instance.
    void ReadVertexData(std::vector<VertexType>& outVertexData)
    {
        outVertexData.resize(nVertices);
        assert(sizeof(VertexType) == vertexAttributes.GetTotalIOSize());
        
        if (storesData)
        {
            memcpy(outVertexData.data(), verticesData.data(), verticesData.size());
        }
        else
        {
            currentVHandle = verticesHandle;
            glBindBuffer(GL_ARRAY_BUFFER, verticesHandle);
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexAttributes.GetTotalIOSize() * nVertices,
                               outVertexData.data());
        }
    }
    //Reads this instance's index data into the given std::vector.
    //If the mesh data is not stored locally, it will be read from the GPU.
    //Otherwise, it will be copied in from this instance's local copy of the data.
    //Assumes that this instance's mesh data uses indices.
    //Also assumes that the given std::vector is empty.
    void ReadIndexData(std::vector<unsigned int>& outIndexData);


    //The new type of vertex this instance will store.
    template<typename VertexType>
    //Sets this instance's vertex data to a new collection of vertices.
    void SetVertexData(const std::vector<VertexType>& newVertices,
                       BufferUsageFrequency usage, RenderIOAttributes _vertexAttributes)
    {
        SetVertexData(newVertices.data(), newVertices.size(), usage, _vertexAttributes);
    }
    //The new type of vertex this instance will store.
    template<typename VertexType>
    void SetVertexData(const VertexType* newVertices, unsigned int _nVertices,
                       BufferUsageFrequency usage, RenderIOAttributes _vertexAttributes)
    {
        vertexAttributes = _vertexAttributes;
        nVertices = _nVertices;
        if (storesData)
        {
            verticesData.resize(nVertices * sizeof(VertexType));
            memcpy(verticesData.data(), newVertices, verticesData.size());
        }
        if (!GetUsesIndices())
        {
            start = 0;
            range = nVertices;
        }

        currentVHandle = verticesHandle;
        glBindBuffer(GL_ARRAY_BUFFER, verticesHandle);
        glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(VertexType), newVertices, ToGLEnum(usage));
    }

    //Sets this instance's index data to a new set of indices.
    void SetIndexData(const std::vector<unsigned int>& newIndices, BufferUsageFrequency usage);
    //Sets this instance's index data to a new set of indices.
    void SetIndexData(const unsigned int* newIndices, unsigned int _nIndices, BufferUsageFrequency usage);


    //Removes this data's indices if they exist. Returns whether they existed.
    bool RemoveIndexData(void);

    //Sets this instance's vertex/index data as the currently-active OpenGL vertex/index buffers.
    void Bind(void) const;


private:

    RenderObjHandle verticesHandle;
    unsigned int nVertices;

    RenderObjHandle indicesHandle;
    unsigned int nIndices;

    RenderIOAttributes vertexAttributes;

    unsigned int start = 0,
                 range = 0;

    bool storesData;
    std::vector<unsigned char> verticesData;
    std::vector<unsigned int> indicesData;


    //The currently-bound data handles.
    static RenderObjHandle currentVHandle, currentIHandle;

    //Converts the given value to the corresponding GL enum value.
    static GLenum ToGLEnum(BufferUsageFrequency usage);
};