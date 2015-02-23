#include "MeshData.h"

#include <assert.h>


RenderObjHandle MeshData::currentVHandle = 0,
                MeshData::currentIHandle = 0;

MeshData::MeshData(bool shouldStoreData, PrimitiveTypes primType)
    : storesData(shouldStoreData), PrimType(primType),
      verticesHandle(0), nVertices(0),
      indicesHandle(0), nIndices(0)
{
    glGenBuffers(1, &verticesHandle);
}

MeshData::~MeshData(void)
{
    if (verticesHandle != 0)
    {
        glDeleteBuffers(1, &verticesHandle);
    }
    if (indicesHandle != 0)
    {
        glDeleteBuffers(1, &indicesHandle);
    }
}

MeshData& MeshData::operator=(MeshData&& other)
{
    if (this != &other)
    {
        storesData = other.storesData;
        verticesHandle = other.verticesHandle;
        indicesHandle = other.indicesHandle;
        nVertices = other.nVertices;
        nIndices = other.nIndices;
        vertexAttributes = other.vertexAttributes;
        verticesData = std::move(other.verticesData);
        indicesData = std::move(other.indicesData);
        PrimType = other.PrimType;

        other.verticesHandle = 0;
        other.indicesHandle = 0;
        other.nVertices = 0;
        other.nIndices = 0;
    }

    return *this;
}
void MeshData::MoveTo(MeshData& newM)
{
    //This function has to be debugged to double-check it works correctly!
    assert(false);

    newM = std::move(*this);
}

void MeshData::SetStoresData(bool newValue)
{
    if (newValue != storesData)
    {
        storesData = newValue;

        if (storesData)
        {
            Bind();
            
            verticesData.resize(nVertices * vertexAttributes.GetTotalIOSize());
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexAttributes.GetTotalIOSize() * nVertices,
                               verticesData.data());

            if (GetUsesIndices())
            {
                indicesData.resize(nIndices);
                glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, nIndices * sizeof(unsigned int),
                                   indicesData.data());
            }
        }
        else
        {
            verticesData.clear();
            indicesData.clear();
        }
    }
}

void MeshData::ReadIndexData(std::vector<unsigned int>& outIndexData)
{
    outIndexData.resize(nIndices);

    if (storesData)
    {
        memcpy(outIndexData.data(), indicesData.data(), indicesData.size() * sizeof(unsigned int));
    }
    else
    {
        Bind();
        glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, nIndices * sizeof(unsigned int),
                           outIndexData.data());
    }
}

void MeshData::SetIndexData(const std::vector<unsigned int>& newIndices, BufferUsageFrequency usage)
{
    SetIndexData(newIndices.data(), newIndices.size(), usage);
}
void MeshData::SetIndexData(const unsigned int* newIndices, unsigned int _nIndices,
                            BufferUsageFrequency usage)
{
    if (indicesHandle == 0)
    {
        glGenBuffers(1, &indicesHandle);
    }

    nIndices = _nIndices;
    if (storesData)
    {
        indicesData.resize(nIndices);
        memcpy(indicesData.data(), newIndices, nIndices * sizeof(unsigned int));
    }

    currentIHandle = indicesHandle;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int),
                 newIndices, ToGLEnum(usage));
}

bool MeshData::RemoveIndexData(void)
{
    if (indicesHandle != 0)
    {
        //First, unbind the indices if they are currently bound.
        if (currentIHandle == indicesHandle)
        {
            currentIHandle = 0;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        glDeleteBuffers(1, &indicesHandle);
        nIndices = 0;
        indicesHandle = 0;
        indicesData.clear();

        return true;
    }

    return false;
}

void MeshData::Bind(void) const
{
    if (verticesHandle != currentVHandle)
    {
        currentVHandle = verticesHandle;
        glBindBuffer(GL_ARRAY_BUFFER, verticesHandle);
    }
    if (indicesHandle != 0 && indicesHandle != currentIHandle)
    {
        currentIHandle = indicesHandle;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesHandle);
    }
}

GLenum MeshData::ToGLEnum(BufferUsageFrequency usage)
{
    switch (usage)
    {
        case BUF_DYNAMIC:
            return GL_DYNAMIC_DRAW;
        case BUF_STATIC:
            return GL_STATIC_DRAW;
        case BUF_STREAM:
            return GL_STREAM_DRAW;
        default:
            assert(false);
            return GL_INVALID_ENUM;
    }
}