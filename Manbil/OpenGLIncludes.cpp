#include "OpenGLIncludes.h"

#include <assert.h>


UniformLocation INVALID_UNIFORM_LOCATION = 0xFFFFFFFF;


void ClearAllRenderingErrors(void)
{
	GLenum err = GL_NO_ERROR + 1;
	while (err != GL_NO_ERROR)
	{
		err = glGetError();
	}
}

const char* GetCurrentRenderingError(void)
{
   	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		const char * err = (const char*)gluErrorString(error);
		if (err == 0)
        {
            err = "";
        }
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
		case PrimitiveTypes::PT_POINTS:
			return GL_POINTS;
		case PrimitiveTypes::PT_TRIANGLE_LIST:
			return GL_TRIANGLES;
        case PrimitiveTypes::PT_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case PrimitiveTypes::PT_LINE_LIST:
			return GL_LINES;
        case PrimitiveTypes::PT_LINE_STRIP:
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
        case PrimitiveTypes::PT_POINTS:
            return "points";
        case PrimitiveTypes::PT_LINE_LIST:
            return "lines";
        case PrimitiveTypes::PT_LINE_STRIP:
            return "lines";
        case PrimitiveTypes::PT_TRIANGLE_LIST:
            return "triangles";
        case PrimitiveTypes::PT_TRIANGLE_STRIP:
            return "triangles";

        default:
            assert(false);
            return "[INVALID GS INPUT TYPE " + std::to_string(PrimitiveTypeToGLEnum(t)) + "]";
    }
}
std::string PrimitiveTypeToGSOutput(PrimitiveTypes t)
{
    switch (t)
    {
        case PrimitiveTypes::PT_POINTS:
            return "points";
        case PrimitiveTypes::PT_LINE_STRIP:
            return "line_strip";
        case PrimitiveTypes::PT_TRIANGLE_STRIP:
            return "triangle_strip";

        default:
            assert(false);
            return "[INVALID GS OUTPUT TYPE " + std::to_string(PrimitiveTypeToGLEnum(t)) + "]";
    }
}
unsigned int PrimitiveTypeToNVertices(PrimitiveTypes t)
{
    switch (t)
    {
        case PrimitiveTypes::PT_POINTS:
            return 1;
        case PrimitiveTypes::PT_LINE_LIST:
        case PrimitiveTypes::PT_LINE_STRIP:
            return 2;
        case PrimitiveTypes::PT_TRIANGLE_LIST:
        case PrimitiveTypes::PT_TRIANGLE_STRIP:
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
		case TT_1D: return GL_TEXTURE_1D;
		case TT_2D: return GL_TEXTURE_2D;
		case TT_3D: return GL_TEXTURE_3D;
		case TT_CUBE: return GL_TEXTURE_CUBE_MAP;

		default:
            assert(false);
            return GL_INVALID_ENUM;
	}
}
GLenum TextureTypeToGLEnum(CubeTextureTypes ct)
{
    switch (ct)
    {
        case CTT_X_NEG: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case CTT_X_POS: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;

        case CTT_Y_NEG: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case CTT_Y_POS: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;

        case CTT_Z_NEG: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        case CTT_Z_POS: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;

        default:
            assert(false);
            return GL_INVALID_ENUM;
    }
}
GLenum ShaderTypeToGLEnum(Shaders sh)
{
    switch (sh)
    {
        case SH_VERTEX: return GL_VERTEX_SHADER;
        case SH_FRAGMENT: return GL_FRAGMENT_SHADER;
        case SH_GEOMETRY: return GL_GEOMETRY_SHADER;
        default:
            assert(false);
            return GL_INVALID_ENUM;
    }
}