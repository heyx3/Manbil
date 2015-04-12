#pragma once

#include "../../Math/Lower Math/Matrix4f.h"
#include "OpenGLIncludes.h"


//Represents a 4x4 matrix.
struct UniformValueMatrix4f
{
public:

    Matrix4f Value;
    UniformLocation Location;
    std::string Name;

    UniformValueMatrix4f(void) { }
    UniformValueMatrix4f(const Matrix4f& value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Value(value) { }

    std::string GetDeclaration(void) const { return "uniform mat4 " + Name + ";"; }
};