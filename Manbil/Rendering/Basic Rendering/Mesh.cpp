#include "Mesh.h"

#include <assert.h>


RenderObjHandle Mesh::currentVHandle = 0,
                Mesh::currentIHandle = 0;

Mesh::Mesh(bool shouldStoreData, PrimitiveTypes primType)
    : storesData(shouldStoreData), PrimType(primType),
      verticesHandle(0), nVertices(0), bytesPerVertex(0),
      indicesHandle(0), nIndices(0)
{
    glGenBuffers(1, &verticesHandle);
}

Mesh::~Mesh(void)
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

Mesh& Mesh::operator=(Mesh&& other)
{
    if (this != &other)
    {
		bytesPerVertex = other.bytesPerVertex;
        storesData = other.storesData;
        verticesHandle = other.verticesHandle;
        indicesHandle = other.indicesHandle;
        nVertices = other.nVertices;
        nIndices = other.nIndices;
        vertexAttributes = std::move(other.vertexAttributes);
        verticesData = std::move(other.verticesData);
        indicesData = std::move(other.indicesData);
        PrimType = other.PrimType;
        start = other.start;
        range = other.range;

        other.verticesHandle = 0;
        other.indicesHandle = 0;
        other.nVertices = 0;
        other.nIndices = 0;
        other.start = 0;
        other.range = 0;
    }

    return *this;
}

Mesh Mesh::MakeCopy(void) const
{
	Mesh m(storesData, PrimType);

	m.nVertices = nVertices;
	m.nIndices = nIndices;
	m.vertexAttributes = vertexAttributes;
	m.start = start;
	m.range = range;
	m.bytesPerVertex = bytesPerVertex;
	
	if (m.storesData)
	{
		m.verticesData = verticesData;
		m.indicesData = indicesData;
	}

	//Copy vertex data.
	if (verticesHandle != 0)
	{
		glBindBuffer(GL_COPY_READ_BUFFER, verticesHandle);
		glBindBuffer(GL_COPY_WRITE_BUFFER, m.verticesHandle);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
							0, 0, nVertices * bytesPerVertex);
	}

	//Copy index data.
	if (GetUsesIndices())
	{
		glGenBuffers(1, &m.indicesHandle);
		glBindBuffer(GL_COPY_READ_BUFFER, indicesHandle);
		glBindBuffer(GL_COPY_WRITE_BUFFER, m.indicesHandle);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
							0, 0, nIndices * sizeof(unsigned int));
	}

	return m;
}

void Mesh::SetStoresData(bool newValue)
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

void Mesh::ReadIndexData(std::vector<unsigned int>& outIndexData)
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

void Mesh::SetIndexData(const std::vector<unsigned int>& newIndices, BufferUsageFrequency usage)
{
    SetIndexData(newIndices.data(), newIndices.size(), usage);
}
void Mesh::SetIndexData(const unsigned int* newIndices, unsigned int _nIndices,
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

    start = 0;
    range = nIndices;

    currentIHandle = indicesHandle;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int),
                 newIndices, ToGLEnum(usage));
}

bool Mesh::RemoveIndexData(void)
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

        start = 0;
        range = nVertices;

        return true;
    }

    return false;
}

void Mesh::Bind(void) const
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

GLenum Mesh::ToGLEnum(BufferUsageFrequency usage)
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