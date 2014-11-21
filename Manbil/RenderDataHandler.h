#pragma once

#include "Math/Lower Math/Matrix4f.h"
#include "Math/Lower Math/Array2D.h"
#include "Math/Lower Math/Array3D.h"
#include "ShaderHandler.h"


//Manages different kinds of data being passed between CPU and GPU.
class RenderDataHandler
{
public:
		
	//The value of an invalid uniform location handle.
	static const UniformLocation INVALID_UNIFORM_LOCATION = 0xFFFFFFFF;
	static bool UniformLocIsValid(UniformLocation loc) { return loc != INVALID_UNIFORM_LOCATION; }
	

	//Returns a string representing the most recent error to occur in the use of this class.
	static std::string GetErrorMessage(void) { return errorMsg; }
	//Removes the current error message.
	static void ClearErrorMessage(void) { errorMsg.clear(); }


	//Gets the location of a uniform variable in the given shader program. Returns whether or not it was successful.
	static bool GetUniformLocation(RenderObjHandle shaderProgram, const Char* name, UniformLocation & outHandle);
    //Gets the location of a subroutine uniform in the given shader program. Returns whether or not it was successful.
    static bool GetSubroutineUniformLocation(RenderObjHandle shaderProgram, ShaderHandler::Shaders shaderType, const Char * name, UniformLocation & outHandle);
    //Gets the id of the given subroutine implementation.
    static void GetSubroutineID(RenderObjHandle shaderProgram, ShaderHandler::Shaders shader, const Char* name, RenderObjHandle & outValue);
	
	//An error that can be thrown in "SetUniformValue".
	static const int EXCEPTION_ELEMENTS_OUT_OF_RANGE;
    //TODO: Inline as many of these as is feasible.
	//Sets a float/vec2/vec3/vec4 value. Assumes the correct shader program is already bound.
    static void SetUniformValue(UniformLocation loc, int elements, const float * values);
    //Sets an array of float/vec2/3/4 values. Assumes the correct shader program is already bound.
    static void SetUniformArrayValue(UniformLocation loc, int arrayElements, int floatsPerElement, const float * valuesSplit);
	//Sets an int/int2/int3/int4 value. Assumes the correct shader program is already bound.
    static void SetUniformValue(UniformLocation loc, int elements, const int * values);
    //Sets an array of int/2/3/4 values. Assumes the correct shader program is already bound.
    static void SetUniformArrayValue(UniformLocation loc, int arrayElements, int intsPerElement, const int * valuesSplit);
	//Sets a matrix value. Assumes the correct shader program is already bound.
	static void SetMatrixValue(UniformLocation loc, const Matrix4f & mat);
    //Sets a subroutine value. Assumes the correct shader program is already bound.
    //PRIORITY: The gl function used here sets ALL subroutine values. Either change this function's signature or find a GL function that only sets one subroutine uniform's value.
    static void SetSubroutineValue(UniformLocation loc, ShaderHandler::Shaders shader, RenderObjHandle valueName);


    //Sets a texture unit as the active one.
	static void ActivateTextureUnit(int texNumb) { glActiveTexture(GL_TEXTURE0 + texNumb); }



	//Gets the state of the frame buffer.
	enum FrameBufferStatus
	{
		//An attached renderbuffer/texture no longer exists, or has a width/height of 0, or is of an incorrect format.
		BAD_ATTACHMENT,
		//Two or more different attached renderbuffers/textures have different width/height.
		DIFFERENT_ATTACHMENT_DIMENSIONS,
		//Nothing is attached to the framebuffer.
		NO_ATTACHMENTS,
		//The combination of attached renderbuffer/texture formats isn't supported by this OpenGL implementation.
		NOT_SUPPORTED,

		//The frame buffer is in an unknown state (presumably not a good one).
		UNKNOWN,

		//The frame buffer is fine and ready to be used.
		EVERYTHING_IS_FINE,
	};
    //Gets the status of the currently-bound frame buffer.
	static FrameBufferStatus GetFramebufferStatus(void);
    //Same functionality as "GetFramebufferStatus" but returns an error message instead of an enum.
	static const char * GetFrameBufferStatusMessage(void);

	//The different uses (and frequencies of use) for buffer objects.
	enum BufferPurpose
	{
		UPDATE_ONCE_AND_DRAW,
		UPDATE_RARELY_AND_DRAW,
		UPDATE_CONSTANTLY_AND_DRAW,

		UPDATE_ONCE_AND_COPY,
		UPDATE_RARELY_AND_COPY,
		UPDATE_CONSTANTLY_AND_COPY,

		UPDATE_ONCE_AND_READ,
		UPDATE_RARELY_AND_READ,
		UPDATE_CONSTANTLY_AND_READ,
	};

	template<class VertexClass>
	//Creates a vertex buffer.
	static void CreateVertexBuffer(RenderObjHandle & vbo, const VertexClass * vertices = 0, int nVertices = 0, BufferPurpose usage = BufferPurpose::UPDATE_ONCE_AND_DRAW)
	{
		glGenBuffers(1, &vbo);
        if (vertices != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(VertexClass)* nVertices, vertices, ToEnum(usage));
        }
	}
	//Creates an index buffer.
    static void CreateIndexBuffer(RenderObjHandle & ibo, const unsigned int * indices = 0, int nIndices = 0, BufferPurpose usage = BufferPurpose::UPDATE_ONCE_AND_DRAW)
	{
		glGenBuffers(1, &ibo);
        if (indices != 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nIndices, indices, ToEnum(usage));
        }
	}

    template<class VertexClass>
    static void UpdateVertexBuffer(RenderObjHandle vbo, const VertexClass * vertices, int nVertices, BufferPurpose usage)
    {
        BindVertexBuffer(vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexClass) * nVertices, vertices, ToEnum(usage));
    }
    static void UpdateIndexBuffer(RenderObjHandle ibo, const unsigned int * indices, int nIndices, BufferPurpose usage)
    {
        BindIndexBuffer(ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nIndices, indices, ToEnum(usage));
    }

	//Sets the given vertex buffer as active.
	static void BindVertexBuffer(RenderObjHandle vbo = 0) { glBindBuffer(GL_ARRAY_BUFFER, vbo); }
	//Sets the given index buffer as active.
	static void BindIndexBuffer(RenderObjHandle ibo = 0) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); }

	//Deletes a vertex or index buffer.
	static void DeleteBuffer(RenderObjHandle bo) { glDeleteBuffers(1, &bo); }

private:
	
	static GLenum ToEnum(BufferPurpose p)
	{
        switch (p)
        {
            case BufferPurpose::UPDATE_ONCE_AND_DRAW: return GL_STATIC_DRAW;
            case BufferPurpose::UPDATE_ONCE_AND_COPY: return GL_STATIC_COPY;
            case BufferPurpose::UPDATE_ONCE_AND_READ: return GL_STATIC_READ;

            case BufferPurpose::UPDATE_RARELY_AND_DRAW: return GL_DYNAMIC_DRAW;
            case BufferPurpose::UPDATE_RARELY_AND_COPY: return GL_DYNAMIC_COPY;
            case BufferPurpose::UPDATE_RARELY_AND_READ: return GL_DYNAMIC_READ;

            case BufferPurpose::UPDATE_CONSTANTLY_AND_DRAW: return GL_STREAM_DRAW;
            case BufferPurpose::UPDATE_CONSTANTLY_AND_COPY: return GL_STREAM_COPY;
            case BufferPurpose::UPDATE_CONSTANTLY_AND_READ: return GL_STREAM_READ;

            default: return GL_INVALID_ENUM;
        }
	}


	static std::string errorMsg;

	RenderDataHandler(void) { }
	~RenderDataHandler(void) { }
};