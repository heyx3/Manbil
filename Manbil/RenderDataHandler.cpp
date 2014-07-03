#include "RenderDataHandler.h"

#include <assert.h>
#include "ShaderHandler.h"
#include "DebugAssist.h"


std::string RenderDataHandler::errorMsg = "";
const int RenderDataHandler::EXCEPTION_ELEMENTS_OUT_OF_RANGE = 1;


bool RenderDataHandler::GetUniformLocation(RenderObjHandle shaderProgram, const Char* name, UniformLocation & out_handle)
{
	out_handle = glGetUniformLocation(shaderProgram, name);

	if (!UniformLocIsValid(out_handle))
	{
		errorMsg = std::string("Shader does not contain '") + name + "' (or it was optimized out in compilation).";
		return false;
	}

	return true;
}
bool RenderDataHandler::GetSubroutineUniformLocation(RenderObjHandle shaderProgram, ShaderHandler::Shaders shaderType, const Char * name, UniformLocation & outHandle)
{
    outHandle = glGetSubroutineUniformLocation(shaderProgram, ShaderHandler::ToEnum(shaderType), name);

    if (!UniformLocIsValid(outHandle))
    {
        errorMsg = std::string("Shader '" + std::to_string(shaderType) + "' does not contain the subroutine uniform '" + name + "'.");
        return false;
    }

    return true;
}
void RenderDataHandler::GetSubroutineID(RenderObjHandle shaderProgram, ShaderHandler::Shaders shader, const Char* name, RenderObjHandle & outValue)
{
    outValue = glGetSubroutineIndex(shaderProgram, ShaderHandler::ToEnum(shader), name);
}

void RenderDataHandler::SetUniformValue(UniformLocation loc, int elements, const float * value)
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
void RenderDataHandler::SetUniformArrayValue(UniformLocation loc, int arrayElements, int floatsPerElement, const float * valuesSplit)
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
void RenderDataHandler::SetUniformValue(UniformLocation loc, int elements, const int * value)
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
void RenderDataHandler::SetUniformArrayValue(UniformLocation loc, int arrayElements, int intsPerElement, const int * valuesSplit)
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
void RenderDataHandler::SetMatrixValue(UniformLocation lc, const Matrix4f & mat)
{
	glUniformMatrix4fv(lc, 1, GL_TRUE, (const GLfloat*)(&mat));
}
void RenderDataHandler::SetSubroutineValue(UniformLocation loc, ShaderHandler::Shaders shader, RenderObjHandle value)
{
    glUniformSubroutinesuiv(ShaderHandler::ToEnum(shader), 1, &value);
}



void RenderDataHandler::GetTexture2DData(RenderObjHandle texObjectHandle, Vector2i texSize, Array2D<Vector4b> & outColor)
{
    BindTexture(TextureTypes::Tex_TwoD, texObjectHandle);
    outColor.Reset((unsigned int)texSize.x, (unsigned int)texSize.y);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)outColor.GetArray());
}
void RenderDataHandler::GetTexture2DData(RenderObjHandle texObjectHandle, Vector2i texSize, Array2D<Vector4f> & outColor)
{
    BindTexture(TextureTypes::Tex_TwoD, texObjectHandle);
    outColor.Reset((unsigned int)texSize.x, (unsigned int)texSize.y);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, (void*)outColor.GetArray());
}

//TODO: Whenever texture data is changed, mipmaps are immediately regenerated if enabled. Double-check that this is necessary and good design.
void RenderDataHandler::SetTexture2DDataColor(RenderObjHandle texObjectHandle, const ColorTextureSettings & settings, Vector4b color)
{
    Array2D<Vector4b> colors(settings.Width, settings.Height, color);

    glBindTexture(GL_TEXTURE_2D, texObjectHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, ColorTextureSettings::ToInternalFormat(settings.Size), settings.Width, settings.Height, 0,
                 ColorTextureSettings::ToFormat(settings.Size), GL_UNSIGNED_BYTE, (Void*)colors.GetArray());

    if (settings.GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);
}
void RenderDataHandler::SetTexture2DDataColor(RenderObjHandle texObjectHandle, const ColorTextureSettings & settings, Vector4f color)
{
    Array2D<Vector4f> colors(settings.Width, settings.Height, color);

    glBindTexture(GL_TEXTURE_2D, texObjectHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, ColorTextureSettings::ToInternalFormat(settings.Size), settings.Width, settings.Height, 0,
                 ColorTextureSettings::ToFormat(settings.Size), GL_FLOAT, (Void*)colors.GetArray());

    if (settings.GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);
}
void RenderDataHandler::SetTexture2DDataPixels(RenderObjHandle texObjectHandle, const ColorTextureSettings & settings, const unsigned char* pixelData)
{
    glBindTexture(GL_TEXTURE_2D, texObjectHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, ColorTextureSettings::ToInternalFormat(settings.Size), settings.Width, settings.Height, 0,
                 ColorTextureSettings::ToFormat(settings.Size), GL_UNSIGNED_BYTE, (const Void*)pixelData);

    if (settings.GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);
}
void RenderDataHandler::SetTexture2DDataPixels(RenderObjHandle texObjectHandle, const ColorTextureSettings & settings, const float* pixelData)
{
	glBindTexture(GL_TEXTURE_2D, texObjectHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, ColorTextureSettings::ToInternalFormat(settings.Size), settings.Width, settings.Height, 0,
                 ColorTextureSettings::ToFormat(settings.Size), GL_FLOAT, (const Void*)pixelData);

    if (settings.GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);
}

void RenderDataHandler::GenerateTexture2DMipmaps(RenderObjHandle texture)
{
    BindTexture(TextureTypes::Tex_TwoD, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
}
Vector2i RenderDataHandler::GetTextureDimensions(RenderObjHandle texture)
{
    BindTexture(TextureTypes::Tex_TwoD, texture);

    Vector2i size;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &size.x);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &size.y);
    return size;
}

void RenderDataHandler::CreateDepthTexture2D(RenderObjHandle & depthTexObjHandle, const DepthTextureSettings & settings)
{
	glGenTextures(1, &depthTexObjHandle);
	glBindTexture(GL_TEXTURE_2D, depthTexObjHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, DepthTextureSettings::ToEnum(settings.Size),
                 settings.Width, settings.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    settings.BaseSettings.ApplyAllSettings(settings.GenerateMipmaps);
}

void RenderDataHandler::DeleteTexture2D(RenderObjHandle & texObjHandle)
{
	glDeleteTextures(1, &texObjHandle);
}


RenderDataHandler::FrameBufferStatus RenderDataHandler::GetFramebufferStatus(const RenderObjHandle & fbo)

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
const char * RenderDataHandler::GetFrameBufferStatusMessage(const RenderObjHandle & fbo)
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