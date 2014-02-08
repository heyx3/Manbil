#include "ShaderHandler.h"

#include <vector>


char ShaderHandler::errorMsg[ERROR_MESSAGE_SIZE];


bool ShaderHandler::CreateShaderProgram(RenderObjHandle & out_handle)
{
	out_handle = glCreateProgram();

	if (out_handle == 0)
	{
		SetErrorMsg("Couldn't create shader program.");
		return false;
	}

	return true;
}

bool ShaderHandler::CreateShader(RenderObjHandle shaderProgram, RenderObjHandle& out_handle, const Char* shaderText, GLenum shaderType)
{
	out_handle = glCreateShader(shaderType);

	if (out_handle == 0)
	{
		SetErrorMsg("Couldn't create shader type.");
		return false;
	}

	const Char* p[1];
	p[0] = shaderText;
	UniformLocation lengths[1];
	lengths[0] = strlen(shaderText);
	glShaderSource(out_handle, 1, p, lengths);

	glCompileShader(out_handle);

	UniformLocation success;
	glGetShaderiv(out_handle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		Char infoLog[ERROR_MESSAGE_SIZE];
		glGetShaderInfoLog(out_handle, sizeof(infoLog), NULL, infoLog);
		SetErrorMsg(infoLog);
		return false;
	}

	glAttachShader(shaderProgram, out_handle);

	return true;
}

bool ShaderHandler::FinalizeShaders(RenderObjHandle shaderProgram, bool validate)
{
	UniformLocation success;
	char error[ERROR_MESSAGE_SIZE];

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(error), NULL, error);
		SetErrorMsg(error);
		return false;
	}

	if (validate)
	{
		glValidateProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, sizeof(error), NULL, error);
			SetErrorMsg(error);
			return false;
		}
	}

	return true;
}