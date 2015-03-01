#pragma once

#include <string>
#include "../../Math/Lower Math/Vectors.h"



//This file defines GLVectors, which are values with between 1 and 4 components.
//In order to reduce duplicate code, a template is defined that covers the various types of vectors.
//At the bottom of the file are specific versions of GLVector for float vectors ("VectorF")
//    and signed int vectors ("VectorI").



//Template arguments giving the various vector types.
//The first four arguments are the vector types on the software side.
//The second four arguments are the vector types on the GPU shader side.
template<typename ComponentType, typename Vec2, typename Vec3, typename Vec4,
         const char* GLSLVec1, const char* GLSLVec2, const char* GLSLVec3, const char* GLSLVec4>
//A GLSL vector with anywhere from 1 to 4 components.
//Note that its multiply and divide operators do component-wise multiplication/division.
struct GLVector
{
private:
    typedef GLVector<ComponentType, Vec2, Vec3, Vec4,
                     GLSLVec1, GLSLVec2, GLSLVec3, GLSLVec4> ThisType;

public:

    static std::string GetGLSLType(unsigned int nComponents)
    {
        switch (nComponents)
        {
            case 1: return GLSLVec1;
            case 2: return GLSLVec2;
            case 3: return GLSLVec3;
            case 4: return GLSLVec4;
            default:
                assert(false);
                return "invalid_type_name";
        }
    }


    //Creates a GL vector of size 0.
    GLVector(void)
    {
        size = 0;
    }
    GLVector(ComponentType value)
    {
        size = 1;
        values[0] = value;
    }
    GLVector(Vec2 value)
    {
        size = 2;
        memcpy(values, &value.x, sizeof(Vec2));
    }
    GLVector(Vec3 value)
    {
        size = 3;
        memcpy(values, &value.x, sizeof(Vec3));
    }
    GLVector(Vec4 value)
    {
        size = 4;
        memcpy(values, &value.x, sizeof(Vec4));
    }
    GLVector(unsigned int nComponents, ComponentType* _values)
    {
        size = nComponents;
        if (_values != 0)
        {
            memcpy(values, _values, size * sizeof(ComponentType));
        }
    }


    ThisType operator+(const ThisType& other)
    {
        assert(size == other.size);

        VectorF retVal(other);
        for (unsigned int i = 0; i < size; ++i)
            retVal.values[i] += values[i];
        return retVal;
    }
    ThisType operator-(const ThisType& other)
    {
        assert(size == other.size);

        VectorF retVal(*this);
        for (unsigned int i = 0; i < size; ++i)
            retVal.values[i] -= other.values[i];
        return retVal;
    }
    ThisType operator*(const ThisType& other)
    {
        assert(size == other.size);

        VectorF retVal(other);
        for (unsigned int i = 0; i < size; ++i)
            retVal.values[i] *= values[i];
        return retVal;
    }
    ThisType operator/(const ThisType& other)
    {
        assert(size == other.size);

        VectorF retVal(*this);
        for (unsigned int i = 0; i < size; ++i)
            retVal.values[i] /= other.values[i];
        return retVal;
    }

    bool operator==(const ThisType& other) const
    {
        if (size != other.size)
            return false;

        for (unsigned int i = 0; i < size; ++i)
            if (values[i] != other.values[i])
                return false;

        return true;
    }
    bool operator!=(const ThisType& other) const { return !(operator==(other)); }

    
    //Gets the number of components this instance has.
    unsigned int GetSize(void) const { return size; }

    //The values this Vector contains.
    const ComponentType* GetValue(void) const { return values; }
    //The values this Vector contains.
    ComponentType* GetValue(void) { return values; }

    void SetValue(ComponentType newVal)
    {
        size = 1;
        values[0] = newVal;
    }
    void SetValue(Vec2 newVal)
    {
        size = 2;
        memcpy(values, &newVal.x, sizeof(Vec2));
    }
    void SetValue(Vec3 newVal)
    {
        size = 3;
        memcpy(values, &newVal.x, sizeof(Vec3));
    }
    void SetValue(Vec4 newVal)
    {
        size = 4;
        memcpy(values, &newVal.x, sizeof(Vec4));
    }
    void SetValue(unsigned int nComponents, ComponentType* newVal)
    {
        assert(nComponents <= 4);
        size = nComponents;
        memcpy(values, &newVal, sizeof(ComponentType) * nComponents);
    }

    std::string GetGLSLType(void) const { return GetGLSLType(size); }

private:

    unsigned int size;
    ComponentType values[4];

};


struct GLSLVectorNames
{
    static const char glslF1Name[],
                      glslF2Name[],
                      glslF3Name[],
                      glslF4Name[];

    static const char glslI1Name[],
                      glslI2Name[],
                      glslI3Name[],
                      glslI4Name[];
};


typedef GLVector<float, Vector2f, Vector3f, Vector4f,
                 GLSLVectorNames::glslF1Name, GLSLVectorNames::glslF2Name,
                 GLSLVectorNames::glslF3Name, GLSLVectorNames::glslF4Name> VectorF;

typedef GLVector<int, Vector2i, Vector3i, Vector4i,
                 GLSLVectorNames::glslI1Name, GLSLVectorNames::glslI2Name,
                 GLSLVectorNames::glslI3Name, GLSLVectorNames::glslI4Name> VectorI;