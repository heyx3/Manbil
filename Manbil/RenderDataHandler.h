#pragma once

#include "Math/Matrix4f.h"
#include "Math/Array2D.h"
#include "Math/Array3D.h"
#include "ShaderHandler.h"
#include "Rendering/Texture Management/TextureSettings.h"
#include <SFML/Graphics/Texture.hpp>


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
    static void SetSubroutineValue(UniformLocation loc, ShaderHandler::Shaders shader, RenderObjHandle valueName);
	

    //TODO: Whenever texture data is changed, mipmaps are immediately regenerated if enabled. Double-check that this is necessary and good design.
    //TODO: Pull all this texture stuff out of RenderDataHandler, rename MTexture to MTexture2D, and create MTexture1D and MTexture3D.
    

    //Attempts to load a texture from the given file path and put it into the given pixel array.
    //Returns whether it was successful.
    static bool LoadTextureFromFile(std::string filePath, Array2D<Vector4b> & outPixelData, bool useMipmaps,
                                    ColorTextureSettings::PixelSizes size, const TextureSettings & settings);
    //Attempts to load a texture from the given file path and put it into the currently-bound 2D texture.
    //Returns the dimensions of the texture, or { -1, -1 } if the function failed.
    static Vector2i LoadTextureFromFile(std::string filePath, bool useMipmaps, ColorTextureSettings::PixelSizes size, const TextureSettings & settings);
    //Attempts to load a texture from the given file path and put it into the currently-bound cubemap texture.
    //Returns the dimensions of the texture, or { -1, -1 } if the function failed.
    static Vector2i LoadTextureFromFile(std::string filePath, CubeTextureTypes face, ColorTextureSettings::PixelSizes size);


    //Setting/creating texture data via bytes.

    //1D textures.

    static void CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                unsigned int width, const Vector4b * pixelData);
    static void CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                unsigned int width, const unsigned char * rgbaColorData);
    static void SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const Vector4b * pixelData);
    static void SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const unsigned char * rgbaPixelData);

    //2D textures.

    static void CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                const TextureSettings & settings, const Array2D<Vector4b> & pixelData);
    static void CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                const TextureSettings & settings, unsigned int width, unsigned int height, const unsigned char * rgbaColorData);
    static void SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                             const Array2D<Vector4b> & pixelData);
    static void SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                             unsigned int width, unsigned int height, const unsigned char * pixelRGBA);
    static void SetTexture2D(RenderObjHandle texObjHandle, bool useMipmaps, ColorTextureSettings::PixelSizes size, const TextureSettings & settings, sf::Image & img);

    //3D textures.

    static void CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                const Array3D<Vector4b> & pixelData);
    static void CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                unsigned int width, unsigned int height, unsigned int depth, const unsigned char * rgbaColorData);
    static void SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const Array3D<Vector4b> & pixelData);
    static void SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                             unsigned int width, unsigned int height, unsigned int depth, const unsigned char * pixelRGBA);


    //Setting/creating texture data via floats.

    //1D textures.

    static void CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                unsigned int width, const Vector4f * pixelData);
    static void CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                unsigned int width, const float * rgbaColorData);
    static void SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const Vector4f * pixelData);
    static void SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const float * rgbaPixelData);

    //2D textures.

    static void CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                const TextureSettings & settings, const Array2D<Vector4f> & pixelData);
    static void CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                const TextureSettings & settings, unsigned int width, unsigned int height, const float * rgbaColorData);
    static void SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                             const Array2D<Vector4f> & pixelData);
    static void SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                             unsigned int width, unsigned int height, const float * pixelRGBA);

    //3D textures.

    static void CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                const Array3D<Vector4f> & pixelData);
    static void CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                                unsigned int width, unsigned int height, unsigned int depth, const float * rgbaColorData);
    static void SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const Array3D<Vector4f> & pixelData);
    static void SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                             unsigned int width, unsigned int height, unsigned int depth, const float * pixelRGBA);


    //Gets the pixel data from the currently-bound 1D texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(Vector4b * outColor);
    //Gets the pixel data from the currently-bound 1D texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(Vector4f * outColor);

    //Gets the pixel data from the currently-bound 2D texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(Array2D<Vector4b> & outColor);
    //Gets the pixel data from the currently-bound 2D texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(Array2D<Vector4f> & outColor);

    //Gets the pixel data from the currently-bound 3D texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(Array3D<Vector4b> & outColor);
    //Gets the pixel data from the currently-bound 3D texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(Array3D<Vector4f> & outColor);

    //Gets the pixel data from the given face of the currently-bound cubemap texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(CubeTextureTypes face, Array2D<Vector4b> & outColor);
    //Gets the pixel data from the given face of the currently-bound cubemap texture.
    //NOTE: Assumes that the out color array is already the correct size!
    static void GetTextureData(CubeTextureTypes face, Array2D<Vector4f> & outColor);


	//Creates a depth texture object for passing to a shader.
	static void CreateDepthTexture2D(RenderObjHandle & texObjectHandle, const DepthTextureSettings & settings);


    //Creates a cubemap texture. Leaves its faces empty.
    static void CreateTextureCubemap(RenderObjHandle & texObjectHandle);
    //Sets the data for the currently-bound cubemap's given face.
    static void SetTextureCubemapFace(CubeTextureTypes cubemapFace, ColorTextureSettings::PixelSizes size, const Array2D<Vector4b> & pixelData)
    {
        SetTextureCubemapFace(cubemapFace, size, pixelData.GetWidth(), pixelData.GetHeight(), &pixelData.GetArray()[0].x);
    }
    //Sets the data for the currently-bound cubemap's given face.
    static void SetTextureCubemapFace(CubeTextureTypes cubemapFace, ColorTextureSettings::PixelSizes pixelSize,
                                      unsigned int width, unsigned int height, const unsigned char * rgbaColor);
    //Sets the data for the currently-bound cubemap's given face.
    static void SetTextureCubemapFace(CubeTextureTypes cubemapFace, ColorTextureSettings::PixelSizes size, const Array2D<Vector4f> & pixelData)
    {
        SetTextureCubemapFace(cubemapFace, size, pixelData.GetWidth(), pixelData.GetHeight(), &pixelData.GetArray()[0].x);
    }
    //Sets the data for the currently-bound cubemap's given face.
    static void SetTextureCubemapFace(CubeTextureTypes cubemapFace, ColorTextureSettings::PixelSizes size,
                                      unsigned int width, unsigned int height, const float * rgbaColor);


    //Generates mipmaps for a texture that has already been created.
    static void GenerateTextureMipmaps(TextureTypes textureType)
    {
        glGenerateMipmap(TextureTypeToGLEnum(textureType));
    }


    //Gets the width/height of the given texture, or { -1, -1 } if the given texture doesn't exist.
    static Vector2i GetTextureDimensions(RenderObjHandle texture);


	//Deletes a texture object.
	static void DeleteTexture(RenderObjHandle & texObjHandle);
	//Sets a texture object as the active one.
	static void BindTexture(TextureTypes type, const RenderObjHandle & bo) { glBindTexture(TextureTypeToGLEnum(type), bo); }
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

		//The frame buffer is in an unknown state (presumably not a good one).
		UNKNOWN,

		//The frame buffer is fine and ready to be used.
		EVERYTHING_IS_FINE,
	};
	static FrameBufferStatus GetFramebufferStatus(const RenderObjHandle & fbo);
	static const char * GetFrameBufferStatusMessage(const RenderObjHandle & fbo);

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