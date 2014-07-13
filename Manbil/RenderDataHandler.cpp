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


RDH::FrameBufferStatus RDH::GetFramebufferStatus(void)
{
	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		case GL_FRAMEBUFFER_COMPLETE: return FrameBufferStatus::EVERYTHING_IS_FINE;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return FrameBufferStatus::BAD_ATTACHMENT;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: return FrameBufferStatus::DIFFERENT_ATTACHMENT_DIMENSIONS;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return FrameBufferStatus::NO_ATTACHMENTS;
		case GL_FRAMEBUFFER_UNSUPPORTED: return FrameBufferStatus::NOT_SUPPORTED;

		default: return FrameBufferStatus::UNKNOWN;
	}
}
const char * RDH::GetFrameBufferStatusMessage(void)
{
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