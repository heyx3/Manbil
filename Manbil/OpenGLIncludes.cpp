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
unsigned int PrimitiveTypeToNVertices(PrimitiveTypes t)
{
    switch (t)
    {
        case PrimitiveTypes::Points:
            return 1;
        case PrimitiveTypes::LineList:
        case PrimitiveTypes::LineStrip:
            return 2;
        case PrimitiveTypes::TriangleList:
        case PrimitiveTypes::TriangleStrip:
            return 3;

        default:
            assert(false);
            return 0;
    }
}

GLenum TextureTypeToGLEnum(TextureTypes t)
{
	switch (t)
	{
		case TextureTypes::TT_1D: return GL_TEXTURE_1D;
		case TextureTypes::TT_2D: return GL_TEXTURE_2D;
		case TextureTypes::TT_3D: return GL_TEXTURE_3D;
		case TextureTypes::TT_CUBE: return GL_TEXTURE_CUBE_MAP;

		default:
            assert(false);
            return GL_INVALID_ENUM;
	}
}
GLenum TextureTypeToGLEnum(CubeTextureTypes ct)
{
    switch (ct)
    {
        case CubeTextureTypes::CTT_X_NEG: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case CubeTextureTypes::CTT_X_POS: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;

        case CubeTextureTypes::CTT_Y_NEG: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case CubeTextureTypes::CTT_Y_POS: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;

        case CubeTextureTypes::CTT_Z_NEG: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        case CubeTextureTypes::CTT_Z_POS: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;

        default: assert(false); return GL_INVALID_ENUM;
    }
}