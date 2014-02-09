#include "Vector.h"

#include <assert.h>


std::string Vector::GetGLSLType(void) const
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

Vector Vector::operator+(const Vector & other) const
{
    assert(size == other.size);

    Vector retVal(size);
    for (int i = 0; i < size; ++i)
        retVal.values[i] = values[i] + other.values[i];
    return retVal;
}
Vector Vector::operator-(const Vector & other) const
{
    assert(size == other.size);

    Vector retVal(size);
    for (int i = 0; i < size; ++i)
        retVal.values[i] = values[i] - other.values[i];
    return retVal;
}
Vector Vector::operator*(const Vector & other) const
{
    assert(size == other.size);

    Vector retVal(size);
    for (int i = 0; i < size; ++i)
        retVal.values[i] = values[i] * other.values[i];
    return retVal;
}
Vector Vector::operator/(const Vector & other) const
{
    assert(size == other.size);

    Vector retVal(size);
    for (int i = 0; i < size; ++i)
        retVal.values[i] = values[i] / other.values[i];
    return retVal;
}