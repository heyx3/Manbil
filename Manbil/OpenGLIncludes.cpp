#include "OpenGLIncludes.h"

#include <assert.h>

void ClearAllRenderingErrors(void)
{
	GLenum err = GL_NO_ERROR + 1;
	while (err != GL_NO_ERROR)
	{
		err = glGetError();
	}
}

const char * GetCurrentRenderingError(void)
{
   	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		const char * err = (const char*)gluErrorString(error);
		if (err == 0) err = "";
		return err;
	}
	else
	{
		return "";
	}
}

GLenum PrimitiveTypeToGLEnum(PrimitiveTypes t)
{
	switch (t)
	{
		case PrimitiveTypes::Points:
			return GL_POINTS;
		case PrimitiveTypes::Triangles:
			return GL_TRIANGLES;
		case PrimitiveTypes::Lines:
			return GL_LINES;

		default: assert(false);
	}
}

GLenum TextureTypeToGLEnum(TextureTypes t)
{
	switch (t)
	{
		case TextureTypes::Tex_OneD: return GL_TEXTURE_1D;
		case TextureTypes::Tex_TwoD: return GL_TEXTURE_2D;
		case TextureTypes::Tex_ThreeD: return GL_TEXTURE_3D;
		case TextureTypes::Tex_Cube: return GL_TEXTURE_CUBE_MAP;

		default: assert(false);
	}
}