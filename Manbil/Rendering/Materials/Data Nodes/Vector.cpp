#include "Vector.h"

#include <assert.h>


std::string VectorF::GetGLSLType(void) const
{
    switch (size)
    {
        case 0: return "invalid_type_name";
        case 1: return "float";
        case 2: return "vec2";
        case 3: return "vec3";
        case 4: return "vec4";
        default: assert(false);
    }
}

VectorF VectorF::operator+(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] + other.values[i];
    return retVal;
}
VectorF VectorF::operator-(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] - other.values[i];
    return retVal;
}
VectorF VectorF::operator*(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] * other.values[i];
    return retVal;
}
VectorF VectorF::operator/(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] / other.values[i];
    return retVal;
}


std::string VectorI::GetGLSLType(void) const
{
    switch (size)
    {
        case 0: return "invalid_type_name";
        case 1: return "int";
        case 2: return "ivec2";
        case 3: return "ivec3";
        case 4: return "ivec4";
        default: assert(false);
    }
}

VectorI VectorI::operator+(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] + other.values[i];
    return retVal;
}
VectorI VectorI::operator-(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] - other.values[i];
    return retVal;
}
VectorI VectorI::operator*(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] * other.values[i];
    return retVal;
}
VectorI VectorI::operator/(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] / other.values[i];
    return retVal;
}