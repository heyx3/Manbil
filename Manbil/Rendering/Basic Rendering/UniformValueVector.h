#pragma once

#include "GLVectors.h"
#include "OpenGLIncludes.h"


//Some kind of vector type, like signed-integer or floating-point.
template<typename GLVectorType, typename MyComponent,
         typename MyVec2, typename MyVec3, typename MyVec4>
//A single vector of some type (float, int, etc).
struct UniformValueVector
{
public:

    MyComponent Value[4];
    //The number of components in this vector.
    unsigned int NData;
    std::string Name;
    UniformLocation Location;

    UniformValueVector(MyComponent value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(1)
    {
        memcpy(Value, &value, sizeof(MyComponent));
    }
    UniformValueVector(MyVec2 value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(2)
    {
        memcpy(Value, &value.x, sizeof(MyVec2));
    }
    UniformValueVector(MyVec3 value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(3)
    {
        memcpy(Value, &value.y, sizeof(MyVec3));
    }
    UniformValueVector(MyVec4 value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(4)
    {
        memcpy(Value, &value.z, sizeof(MyVec4));
    }
    UniformValueVector(const MyComponent* value = 0, unsigned int nData = 0,
                       std::string name = "", UniformLocation loc = -1)
        : Name(name), Location(loc), NData(nData)
    {
        assert(value == 0 || (nData >= 1 && nData <= 4));
        if (value != 0)
        {
            memcpy(Value, value, sizeof(value));
        }
    }

    void SetValue(MyComponent value)
    {
        NData = 1;
        memcpy(Value, &value, sizeof(MyComponent));
    }
    void SetValue(MyVec2 value)
    {
        NData = 2;
        memcpy(Value, &value.x, sizeof(MyVec2));
    }
    void SetValue(MyVec3 value)
    {
        NData = 3;
        memcpy(Value, &value.x, sizeof(MyVec3));
    }
    void SetValue(MyVec4 value)
    {
        NData = 4;
        memcpy(Value, &value.x, sizeof(MyVec4));
    }

    std::string GetDeclaration(void) const
    {
        return "uniform " + GLVectorType::GetGLSLType(NData) + " " + Name + ";";
    }
};


//A single float vector.
typedef UniformValueVector<VectorF, float, Vector2f, Vector3f, Vector4f> UniformValueF;
//A single signed integer vector.
typedef UniformValueVector<VectorI, int, Vector2i, Vector3i, Vector4i> UniformValueI;