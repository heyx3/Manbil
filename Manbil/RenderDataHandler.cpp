#include "RenderDataHandler.h"

#include <assert.h>
#include "ShaderHandler.h"
#include "DebugAssist.h"


typedef RenderDataHandler RDH;


std::string RDH::errorMsg = "";
const int RDH::EXCEPTION_ELEMENTS_OUT_OF_RANGE = 1;


bool RDH::GetUniformLocation(RenderObjHandle shaderProgram, const Char* name, UniformLocation & out_handle)
{
	out_handle = glGetUniformLocation(shaderProgram, name);

	if (!UniformLocIsValid(out_handle))
	{
		errorMsg = std::string("Shader does not contain '") + name + "' (or it was optimized out in compilation).";
		return false;
	}

	return true;
}
bool RDH::GetSubroutineUniformLocation(RenderObjHandle shaderProgram, ShaderHandler::Shaders shaderType, const Char * name, UniformLocation & outHandle)
{
    outHandle = glGetSubroutineUniformLocation(shaderProgram, ShaderHandler::ToEnum(shaderType), name);

    if (!UniformLocIsValid(outHandle))
    {
        errorMsg = std::string("Shader '" + std::to_string(shaderType) + "' does not contain the subroutine uniform '" + name + "'.");
        return false;
    }

    return true;
}
void RDH::GetSubroutineID(RenderObjHandle shaderProgram, ShaderHandler::Shaders shader, const Char* name, RenderObjHandle & outValue)
{
    outValue = glGetSubroutineIndex(shaderProgram, ShaderHandler::ToEnum(shader), name);
}

void RDH::SetUniformValue(UniformLocation loc, int elements, const float * value)
{
    //TODO: Remove exception handling; just assert().
	if (elements < 1 || elements > 4)
	{
		throw EXCEPTION_ELEMENTS_OUT_OF_RANGE;
	}

	switch (elements)
	{
		case 1:
			glUniform1f(loc, value[0]);
			break;
		case 2:
			glUniform2f(loc, value[0], value[1]);
			break;
		case 3:
			glUniform3f(loc, value[0], value[1], value[2]);
			break;
		case 4:
			glUniform4f(loc, value[0], value[1], value[2], value[3]);
			break;

		default: assert(false);
	}
}
void RDH::SetUniformArrayValue(UniformLocation loc, int arrayElements, int floatsPerElement, const float * valuesSplit)
{
    //TODO: Remove exception handling; just assert().
    if (floatsPerElement < 1 || floatsPerElement > 4)
    {
        throw EXCEPTION_ELEMENTS_OUT_OF_RANGE;
    }

    switch (floatsPerElement)
    {
        case 1:
            glUniform1fv(loc, arrayElements, valuesSplit);
            break;

        case 2:
            glUniform2fv(loc, arrayElements, valuesSplit);
            break;

        case 3:
            glUniform3fv(loc, arrayElements, valuesSplit);
            break;

        case 4:
            glUniform4fv(loc, arrayElements, valuesSplit);
            break;

        default: assert(false);
    }
}
void RDH::SetUniformValue(UniformLocation loc, int elements, const int * value)
{
    //TODO: Remove exception handling; just assert().
	if (elements < 1 || elements > 4)
	{
		throw EXCEPTION_ELEMENTS_OUT_OF_RANGE;
	}

	switch (elements)
	{
		case 1:
			glUniform1i(loc, value[0]);
			break;
		case 2:
			glUniform2i(loc, value[0], value[1]);
			break;
		case 3:
			glUniform3i(loc, value[0], value[1], value[2]);
			break;
		case 4:
			glUniform4i(loc, value[0], value[1], value[2], value[3]);
			break;

		default: assert(false);
	}
}
void RDH::SetUniformArrayValue(UniformLocation loc, int arrayElements, int intsPerElement, const int * valuesSplit)
{
    //TODO: Remove exception handling; just assert().
    if (intsPerElement < 1 || intsPerElement > 4)
    {
        throw EXCEPTION_ELEMENTS_OUT_OF_RANGE;
    }

    switch (intsPerElement)
    {
        case 1:
            glUniform1iv(loc, arrayElements, valuesSplit);
            break;

        case 2:
            glUniform2iv(loc, arrayElements, valuesSplit);
            break;

        case 3:
            glUniform3iv(loc, arrayElements, valuesSplit);
            break;

        case 4:
            glUniform4iv(loc, arrayElements, valuesSplit);
            break;

        default:
            assert(false);
    }
}
void RDH::SetMatrixValue(UniformLocation lc, const Matrix4f & mat)
{
	glUniformMatrix4fv(lc, 1, GL_TRUE, (const GLfloat*)(&mat));
}
void RDH::SetSubroutineValue(UniformLocation loc, ShaderHandler::Shaders shader, RenderObjHandle value)
{
    glUniformSubroutinesuiv(ShaderHandler::ToEnum(shader), 1, &value);
}



bool RDH::LoadTextureFromFile(std::string filePath, Array2D<Vector4b> & outPixelData,
                              bool useMipmaps, ColorTextureSettings::PixelSizes size, const TextureSettings & settings)
{
    //Load the image in using SFML.
    sf::Image img;
    if (!img.loadFromFile(filePath))
    {
        return false;
    }

    //Fill the out array with the image data.
    outPixelData.Reset(img.getSize().x, img.getSize().y);
    outPixelData.Fill((Vector4b*)img.getPixelsPtr());

    return true;
}
Vector2i RDH::LoadTextureFromFile(std::string filePath, bool useMipmaps,
                                  ColorTextureSettings::PixelSizes size, const TextureSettings & settings)
{
    //Load the image in using SFML.
    sf::Image img;
    if (!img.loadFromFile(filePath))
    {
        return Vector2i(-1, -1);
    }

    //Set the texture data to the image data.
    SetTexture2D(useMipmaps, size, settings, img.getSize().x, img.getSize().y, img.getPixelsPtr());

    return Vector2i((int)img.getSize().x, (int)img.getSize().y);
}
Vector2i RDH::LoadTextureFromFile(std::string filePath, CubeTextureTypes face, ColorTextureSettings::PixelSizes size)
{
    //Load the image in using SFML.
    sf::Image img;
    if (!img.loadFromFile(filePath))
    {
        return Vector2i(-1, -1);
    }

    //Set the texture data to the image data.
    SetTextureCubemapFace(face, size, img.getSize().x, img.getSize().y, img.getPixelsPtr());

    return Vector2i((int)img.getSize().x, (int)img.getSize().y);
}

void RDH::CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          unsigned int width, const Vector4b * pixelData)
{
    glGenTextures(1, &outTexHandle);
    glBindTexture(GL_TEXTURE_1D, outTexHandle);

    SetTexture1D(useMipmaps, pixelSize, width, pixelData);
}
void RDH::CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          unsigned int width, const unsigned char * rgbaColorData)
{
    glGenTextures(1, &outTexHandle);
    glBindTexture(GL_TEXTURE_1D, outTexHandle);

    SetTexture1D(useMipmaps, pixelSize, width, rgbaColorData);
}
void RDH::SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const Vector4b * pixelData)
{
    SetTexture1D(useMipmaps, pixelSize, width, &pixelData[0].x);
}
void RDH::SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const unsigned char * rgbaPixelData)
{
    glTexImage1D(GL_TEXTURE_1D, 0, ColorTextureSettings::ToInternalFormat(pixelSize), width, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaPixelData);
    if (useMipmaps) glGenerateMipmap(GL_TEXTURE_1D);
}

void RDH::CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          const TextureSettings & settings, const Array2D<Vector4b> & pixelData)
{
    CreateTexture2D(outTexHandle, useMipmaps, pixelSize, settings, pixelData.GetWidth(), pixelData.GetHeight(), &pixelData.GetArray()[0].x);
}
void RDH::CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          const TextureSettings & settings, unsigned int width, unsigned int height, const unsigned char * rgbaColorData)
{
    glGenTextures(1, &outTexHandle);
    glBindTexture(GL_TEXTURE_2D, outTexHandle);

    SetTexture2D(useMipmaps, pixelSize, settings, width, height, rgbaColorData);
}
void RDH::SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                       const Array2D<Vector4b> & pixelData)
{
    SetTexture2D(useMipmaps, pixelSize, settings, pixelData.GetWidth(), pixelData.GetHeight(), &pixelData.GetArray()[0].x);
}
void RDH::SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                       unsigned int width, unsigned int height, const unsigned char * pixelRGBA)
{
    glTexImage2D(GL_TEXTURE_2D, 0, ColorTextureSettings::ToInternalFormat(pixelSize), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelRGBA);
    if (useMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
    settings.ApplyAllSettings(useMipmaps);
}
void RDH::SetTexture2D(RenderObjHandle texObjHandle, bool useMipmaps, ColorTextureSettings::PixelSizes size, const TextureSettings & settings, sf::Image & img)
{
    glBindTexture(GL_TEXTURE_2D, texObjHandle);
    SetTexture2D(useMipmaps, size, settings, img.getSize().x, img.getSize().y, img.getPixelsPtr());
}

void RDH::CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          const Array3D<Vector4b> & pixelData)
{
    CreateTexture3D(outTexHandle, useMipmaps, pixelSize, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    &pixelData.GetArray()[0].x);
}
void RDH::CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          unsigned int width, unsigned int height, unsigned int depth, const unsigned char * rgbaColorData)
{
    glGenTextures(1, &outTexHandle);
    glBindTexture(GL_TEXTURE_3D, outTexHandle);
    SetTexture3D(useMipmaps, pixelSize, width, height, depth, rgbaColorData);
}
void RDH::SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const Array3D<Vector4b> & pixelData)
{
    SetTexture3D(useMipmaps, pixelSize, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(), &pixelData.GetArray()[0].x);
}
void RDH::SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                       unsigned int width, unsigned int height, unsigned int depth, const unsigned char * pixelRGBA)
{
    glTexImage3D(GL_TEXTURE_3D, 0, ColorTextureSettings::ToInternalFormat(pixelSize), width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelRGBA);
    if (useMipmaps) glGenerateMipmap(GL_TEXTURE_3D);
}


void RDH::CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          unsigned int width, const Vector4f * pixelData)
{
    CreateTexture1D(outTexHandle, useMipmaps, pixelSize, width, &pixelData[0].x);
}
void RDH::CreateTexture1D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          unsigned int width, const float * rgbaColorData)
{
    glGenTextures(1, &outTexHandle);
    glBindTexture(GL_TEXTURE_1D, outTexHandle);

    SetTexture1D(useMipmaps, pixelSize, width, rgbaColorData);
}
void RDH::SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const Vector4f * pixelData)
{
    SetTexture1D(useMipmaps, pixelSize, width, &pixelData[0].x);
}
void RDH::SetTexture1D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, unsigned int width, const float * rgbaPixelData)
{
    glTexImage1D(GL_TEXTURE_1D, 0, ColorTextureSettings::ToInternalFormat(pixelSize), width, 0, GL_RGBA, GL_FLOAT, rgbaPixelData);
    if (useMipmaps) glGenerateMipmap(GL_TEXTURE_1D);
}

void RDH::CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          const TextureSettings & settings, const Array2D<Vector4f> & pixelData)
{
    CreateTexture2D(outTexHandle, useMipmaps, pixelSize, settings, pixelData.GetWidth(), pixelData.GetHeight(), &pixelData.GetArray()[0].x);
}
void RDH::CreateTexture2D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          const TextureSettings & settings, unsigned int width, unsigned int height, const float * rgbaColorData)
{
    glGenTextures(1, &outTexHandle);
    glBindTexture(GL_TEXTURE_2D, outTexHandle);

    SetTexture2D(useMipmaps, pixelSize, settings, width, height, rgbaColorData);
}
void RDH::SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                       const Array2D<Vector4f> & pixelData)
{
    SetTexture2D(useMipmaps, pixelSize, settings, pixelData.GetWidth(), pixelData.GetHeight(), &pixelData.GetArray()[0].x);
}
void RDH::SetTexture2D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const TextureSettings & settings,
                       unsigned int width, unsigned int height, const float * pixelRGBA)
{
    glTexImage2D(GL_TEXTURE_2D, 0, ColorTextureSettings::ToInternalFormat(pixelSize), width, height, 0, GL_RGBA, GL_FLOAT, pixelRGBA);
    if (useMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
    settings.ApplyAllSettings(useMipmaps);
}

void RDH::CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          const Array3D<Vector4f> & pixelData)
{
    CreateTexture3D(outTexHandle, useMipmaps, pixelSize, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(),
                    &pixelData.GetArray()[0].x);
}
void RDH::CreateTexture3D(RenderObjHandle & outTexHandle, bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                          unsigned int width, unsigned int height, unsigned int depth, const float * rgbaColorData)
{
    glGenTextures(1, &outTexHandle);
    glBindTexture(GL_TEXTURE_3D, outTexHandle);
    SetTexture3D(useMipmaps, pixelSize, width, height, depth, rgbaColorData);
}
void RDH::SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize, const Array3D<Vector4f> & pixelData)
{
    SetTexture3D(useMipmaps, pixelSize, pixelData.GetWidth(), pixelData.GetHeight(), pixelData.GetDepth(), &pixelData.GetArray()[0].x);
}
void RDH::SetTexture3D(bool useMipmaps, ColorTextureSettings::PixelSizes pixelSize,
                       unsigned int width, unsigned int height, unsigned int depth, const float * pixelRGBA)
{
    glTexImage3D(GL_TEXTURE_3D, 0, ColorTextureSettings::ToInternalFormat(pixelSize), width, height, depth, 0, GL_RGBA, GL_FLOAT, pixelRGBA);
    if (useMipmaps) glGenerateMipmap(GL_TEXTURE_3D);
}



void RDH::GetTextureData(Vector4b * outColor)
{
    glGetTexImage(GL_TEXTURE_1D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)outColor);
}
void RDH::GetTextureData(Vector4f * outColor)
{
    glGetTexImage(GL_TEXTURE_1D, 0, GL_RGBA, GL_FLOAT, (void*)outColor);
}
void RDH::GetTextureData(Array2D<Vector4b> & outColor)
{
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)outColor.GetArray());
}
void RDH::GetTextureData(Array2D<Vector4f> & outColor)
{
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, (void*)outColor.GetArray());
}
void RDH::GetTextureData(Array3D<Vector4b> & outColor)
{
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)outColor.GetArray());
}
void RDH::GetTextureData(Array3D<Vector4f> & outColor)
{
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA, GL_FLOAT, (void*)outColor.GetArray());
}
void RDH::GetTextureData(CubeTextureTypes face, Array2D<Vector4b> & outColor)
{
    glGetTexImage(TextureTypeToGLEnum(face), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)outColor.GetArray());
}
void RDH::GetTextureData(CubeTextureTypes face, Array2D<Vector4f> & outColor)
{
    glGetTexImage(TextureTypeToGLEnum(face), 0, GL_RGBA, GL_FLOAT, (void*)outColor.GetArray());
}

Vector2i RDH::GetTextureDimensions(RenderObjHandle texture)
{
    BindTexture(TextureTypes::TT_2D, texture);

    Vector2i size;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &size.x);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &size.y);
    return size;
}

void RDH::CreateDepthTexture2D(RenderObjHandle & depthTexObjHandle, const DepthTextureSettings & settings)
{
	glGenTextures(1, &depthTexObjHandle);
	glBindTexture(GL_TEXTURE_2D, depthTexObjHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, DepthTextureSettings::ToEnum(settings.PixelSize),
                 settings.Width, settings.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    settings.BaseSettings.ApplyAllSettings(settings.GenerateMipmaps);
}

void RDH::CreateTextureCubemap(RenderObjHandle & texObjectHandle)
{
    glGenTextures(1, &texObjectHandle);
}
void RDH::SetTextureCubemapFace(CubeTextureTypes cubemapFace, ColorTextureSettings::PixelSizes size, unsigned int width, unsigned int height, const unsigned char * rgbaColor)
{
    GLenum faceType = TextureTypeToGLEnum(cubemapFace);

    glTexImage2D(faceType, 0, ColorTextureSettings::ToInternalFormat(size), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaColor);
    //if (settings.GenerateMipmaps) glGenerateMipmap(faceType);
    //settings.BaseSettings.ApplyAllSettingsCubemap(cubemapFace, settings.GenerateMipmaps);
}
void RDH::SetTextureCubemapFace(CubeTextureTypes cubemapFace, ColorTextureSettings::PixelSizes size,
                                unsigned int width, unsigned int height, const float * rgbaColor)
{
    GLenum faceType = TextureTypeToGLEnum(cubemapFace);

    glTexImage2D(faceType, 0, ColorTextureSettings::ToInternalFormat(size), width, height, 0, GL_RGBA, GL_FLOAT, rgbaColor);
    //if (settings.GenerateMipmaps) glGenerateMipmap(faceType);
    //settings.BaseSettings.ApplyAllSettingsCubemap(cubemapFace, settings.GenerateMipmaps);
}

void RDH::DeleteTexture(RenderObjHandle & texObjHandle)
{
	glDeleteTextures(1, &texObjHandle);
}


RDH::FrameBufferStatus RDH::GetFramebufferStatus(const RenderObjHandle & fbo)

{
	GLint prevBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);


	switch (result)
	{
		case GL_FRAMEBUFFER_COMPLETE: return FrameBufferStatus::EVERYTHING_IS_FINE;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return FrameBufferStatus::BAD_ATTACHMENT;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: return FrameBufferStatus::DIFFERENT_ATTACHMENT_DIMENSIONS;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return FrameBufferStatus::NO_ATTACHMENTS;
		case GL_FRAMEBUFFER_UNSUPPORTED: return FrameBufferStatus::NOT_SUPPORTED;

		default: return FrameBufferStatus::UNKNOWN;
	}
}
const char * RDH::GetFrameBufferStatusMessage(const RenderObjHandle & fbo)
{
	GLint prevBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	if (strcmp(GetCurrentRenderingError(), "") != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
		return "Unable to bind the frame buffer";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);


	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		case GL_FRAMEBUFFER_COMPLETE: return "";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "Bad texture or depth buffer attachment";
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: return "Texture and depth buffer are different dimensions.";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "Nothing is attached";
		case GL_FRAMEBUFFER_UNSUPPORTED: return "This combination of texture and depth buffer is not supported on this platform.";

		default: return "Unknown frame buffer error.";
	}
}