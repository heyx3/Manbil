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
		case PrimitiveTypes::TriangleList:
			return GL_TRIANGLES;
        case PrimitiveTypes::TriangleStrip:
            return GL_TRIANGLE_STRIP;
        case PrimitiveTypes::LineList:
			return GL_LINES;
        case PrimitiveTypes::LineStrip:
            return GL_LINE_STRIP;

		default:
            assert(false);
            return GL_INVALID_ENUM;
	}
}
std::string PrimitiveTypeToGSInput(PrimitiveTypes t)
{
    switch (t)
    {
        case PrimitiveTypes::Points:
            return "points";
        case PrimitiveTypes::LineList:
            return "lines";
        case PrimitiveTypes::LineStrip:
            return "lines";
        case PrimitiveTypes::TriangleList:
            return "triangles";
        case PrimitiveTypes::TriangleStrip:
            return "triangles";

        default:
            assert(false);
            return "[INVALID GS INPUT TYPE. GLenum value: " + std::to_string(PrimitiveTypeToGLEnum(t)) + "]";
    }
}
std::string PrimitiveTypeToGSOutput(PrimitiveTypes t)
{
    switch (t)
    {
        case PrimitiveTypes::Points:
            return "points";
        case PrimitiveTypes::LineStrip:
            return "line_strip";
        case PrimitiveTypes::TriangleStrip:
            return "triangle_strip";

        default:
            assert(false);
            return "[INVALID GS OUTPUT TYPE. GLenum value: " + std::to_string(PrimitiveTypeToGLEnum(t)) + "]";
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

		default:
            assert(false);
            return GL_INVALID_ENUM;
	}
}