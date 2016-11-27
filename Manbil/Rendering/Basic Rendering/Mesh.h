#pragma once

#include <vector>
#include <assert.h>

#include "OpenGLIncludes.h"
#include "RenderIOAttributes.h"


//A collection of vertices and (optionally) indices for rendering.
//Can optionally store a copy of the data on RAM for quick CPU access.
struct Mesh
{
	//Note that this class assumes you aren't doing any manual binding of vertex/index buffers.
	//If you are, but you still want to use this class, then you have to remember to re-bind
	//    "Mesh::GetCurrentVHandle()" and "Mesh::GetCurrentIHandle()" when you're done.

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


	//Gets the OpenGL handle for the most recently-bound mesh vertex buffer.
	static RenderObjHandle GetCurrentVHandle(void) { return currentVHandle; }
	//Gets the OpenGL handle for the most recently-bound mesh index buffer.
	static RenderObjHandle GetCurrentIHandle(void) { return currentIHandle; }


    PrimitiveTypes PrimType;


    //The first argument is whether to store a local copy of the mesh data for easy querying.
	//The second argument is the kind of primitives this mesh will be storing.
    Mesh(bool shouldStoreData, PrimitiveTypes primType);
    ~Mesh(void);

	//Move semantics:
	Mesh(Mesh&& other) { *this = std::move(other); }
	Mesh& operator=(Mesh&& other);

	//No implicit copying of the mesh is allowed,
	//    because that involves some expensive OpenGL calls and allocations.
	Mesh(const Mesh& cpy) = delete;
	Mesh& operator=(const Mesh& cpy) = delete;

	Mesh MakeCopy(void) const;


    void SetStoresData(bool newValue);
    bool GetStoresData(void) const { return storesData; }

    bool GetUsesIndices(void) const { return indicesHandle != 0; }
    
    unsigned int GetNVertices(void) const { return nVertices; }
    unsigned int GetNIndices(void) const { return nIndices; }
    
    RenderObjHandle GetVerticesHandle(void) const { return verticesHandle; }
    RenderObjHandle GetIndicesHandle(void) const { return indicesHandle; }

    const RenderIOAttributes& GetVertexAttributes(void) const { return vertexAttributes; }


    //Sets the range of indices to use (or vertices, if not using indices).
    //Defaults to the entire range of available vertices/indices.
    //Every time the vertex or index buffer is changed, this range will reset to cover all elements.
    void SetUseRange(unsigned int _start, unsigned int _range) { start = _start; range = _range; }

	//Gets the beginning of the range of indices to use.
    unsigned int GetRangeStart(void) const { return start; }
	//Gets the number of indices to actually use.
    unsigned int GetRangeSize(void) const { return range; }


    template<typename VertexType>
    //Returns a pointer to a copy of the vertex data.
	//Assumes that this instance stores the mesh data.
	const VertexType* GetVertexData(void) const { assert(storesData); return (VertexType*)verticesData.data(); }
    //Returns a pointer to a copy of the index data.
    //Assumes that this instance's mesh data uses indices, and that this instance stores the mesh data.
	const unsigned int* GetIndexData(void) const { assert(storesData && GetUsesIndices()); return indicesData.data(); }


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


    template<typename VertexType>
    void SetVertexData(const std::vector<VertexType>& newVertices,
                       BufferUsageFrequency usage, RenderIOAttributes _vertexAttributes)
    {
        SetVertexData(newVertices.data(), newVertices.size(), usage, _vertexAttributes);
    }

    template<typename VertexType>
    void SetVertexData(const VertexType* newVertices, unsigned int _nVertices,
                       BufferUsageFrequency usage, RenderIOAttributes _vertexAttributes)
    {
        vertexAttributes = _vertexAttributes;
        nVertices = _nVertices;
		bytesPerVertex = sizeof(VertexType);
        if (storesData)
        {
            verticesData.resize(nVertices * bytesPerVertex);
            memcpy(verticesData.data(), newVertices, verticesData.size());
        }
        if (!GetUsesIndices())
        {
            start = 0;
            range = nVertices;
        }

        currentVHandle = verticesHandle;
        glBindBuffer(GL_ARRAY_BUFFER, verticesHandle);
        glBufferData(GL_ARRAY_BUFFER, nVertices * bytesPerVertex, newVertices, ToGLEnum(usage));
    }

    void SetIndexData(const std::vector<unsigned int>& newIndices, BufferUsageFrequency usage);
    void SetIndexData(const unsigned int* newIndices, unsigned int _nIndices, BufferUsageFrequency usage);


    //Removes this data's indices if they exist. Returns whether they existed.
    bool RemoveIndexData(void);

    //Tells OpenGL to use this mesh's vertex/index data for future operations.
    void Bind(void) const;


private:

    RenderObjHandle verticesHandle;
    unsigned int nVertices, bytesPerVertex;

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