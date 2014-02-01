#pragma once

#include "OpenGLIncludes.h"
#include "SFML/Graphics.hpp"
#include "Math/Matrix4f.h"

//Manages different kinds of data being passed between CPU and GPU.
class RenderDataHandler
{
public:
		
	//The value of an invalid uniform location handle.
	static const UniformLocation INVALID_UNIFORM_LOCATION = 0xFFFFFFFF;
	static bool UniformLocIsValid(UniformLocation loc) { return loc != INVALID_UNIFORM_LOCATION; }
	
	//The size of the error message string.
	static const int ERROR_MESSAGE_SIZE = 1024;
	//Returns a string representing the most recent error to occur in the use of this class.
	static const char* const GetErrorMessage(void) { return errorMsg; }
	//Removes the current error message.
	static void ClearErrorMessage(void) { errorMsg[0] = '\0'; }

	//Gets the location of a uniform variable in the given shader program. Returns whether or not it was successful.
	static bool GetUniformLocation(BufferObjHandle shaderProgram, const Char* name, UniformLocation & out_handle);
	
	//An error that can be thrown in "SetUniformValue".
	static const int EXCEPTION_ELEMENTS_OUT_OF_RANGE;
	//Sets a float/vec2/vec3/vec4 value. Assumes the correct shader program is already bound.
	static void SetUniformValue(UniformLocation loc, int elements, const float * values);
	//Sets an int/int2/int3/int4 value. Assumes the correct shader program is already bound.
	static void SetUniformValue(UniformLocation loc, int elements, const int * values);
	//Sets a matrix value. Assumes the correct shader program is already bound.
	static void SetMatrixValue(UniformLocation loc, const Matrix4f & mat);
	
	//Creates a texture object for passing to a shader.
	static void CreateTexture2D(BufferObjHandle & texObjectHandle, sf::Image & img, bool createMipmaps = false);
	//Creates a texture object for passing to a shader.
	static void CreateTexture2D(BufferObjHandle & texObjectHandle, Vector2i size);
	//Creates a depth texture object for passing to a shader.
	static void CreateDepthTexture2D(BufferObjHandle & texObjectHandle, Vector2i size);
    //Generates mipmaps for a texture that has already been created.
    static void GenerateTexture2DMipmaps(BufferObjHandle texture);

	//Sets the texture data using float4 color.
	static void SetTexture2DDataFloats(const BufferObjHandle & texObjectHandle, Vector2i texSize, Void* pixelData = 0);
	//Sets the texture data using unsigned byte4 color.
	static void SetTexture2DDataUBytes(const BufferObjHandle & texObjectHandle, Vector2i texSize, Void* pixelData = 0);
	//Sets the texture data using an SFML Image.
	static void SetTexture2DData(const BufferObjHandle & texObjHandle, sf::Image & img) { SetTexture2DDataUBytes(texObjHandle, Vector2i(img.getSize().x, img.getSize().y), (Void*)img.getPixelsPtr()); }
	//Sets the given depth texture to the given size.
	static void SetDepthTexture2DSize(const BufferObjHandle & texObjHandle, Vector2i size);
	//Deletes a texture object.
	static void DeleteTexture2D(BufferObjHandle & texObjHandle);
	//Sets a texture object as the active one.
	static void BindTexture(TextureTypes type, const BufferObjHandle & bo) { glBindTexture(TextureTypeToGLEnum(type), bo); }
	//Sets a texture unit as active.
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

		//The frame buffer is in an unknown state (probably not a good one).
		UNKNOWN,

		//The frame buffer is fine and ready to be used.
		EVERYTHING_IS_FINE,
	};
	static FrameBufferStatus GetFramebufferStatus(const BufferObjHandle & fbo);
	static const char * GetFrameBufferStatusMessage(const BufferObjHandle & fbo);

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
	static void CreateVertexBuffer(BufferObjHandle & vbo, VertexClass * vertices, int nVertices, BufferPurpose usage)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexClass) * nVertices, vertices, ToEnum(usage));
	}
	//Creates an index buffer.
	static void CreateIndexBuffer(BufferObjHandle & ibo, unsigned int * indices, int nIndices, BufferPurpose usage)
	{
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * nIndices, indices, ToEnum(usage));
	}

	//Sets the given vertex buffer as active.
	static void BindVertexBuffer(BufferObjHandle vbo = 0) { glBindBuffer(GL_ARRAY_BUFFER, vbo); }
	//Sets the given index buffer as active.
	static void BindIndexBuffer(BufferObjHandle ibo = 0) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); }

	//Deletes a vertex or index buffer.
	static void DeleteBuffer(BufferObjHandle bo) { glDeleteBuffers(1, &bo); }

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
		}
	}


	static char errorMsg[ERROR_MESSAGE_SIZE];
	static void SetErrorMsg(const char * error) { strncpy(errorMsg, error, ERROR_MESSAGE_SIZE); }

	RenderDataHandler(void) { }
	~RenderDataHandler(void) { }
};