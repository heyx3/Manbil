#pragma once

#include <string>
#include "../../../Math/Vectors.h"

//Represents a collection of 1-4 floats.
//If a VectorF has 0 floats, it is considered invalid.
struct VectorF
{
public:

    //Gets the number of floats this VectorF has.
    unsigned int GetSize(void) const { return size; }
    //The floats representing this VectorF.
    const float * GetValue(void) const { return values; }

    void SetValue(float newVal) { size = 1; values[0] = newVal; }
    void SetValue(Vector2f newVal) { size = 2; values[0] = newVal.x; values[1] = newVal.y; }
    void SetValue(Vector3f newVal) { size = 3; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; }
    void SetValue(Vector4f newVal) { size = 4; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; values[3] = newVal.w; }
    //Marks this VectorF as invalid.
    void SetValue(void) { size = 0; }


    VectorF(void) : size(0) { values[0] = 0.0f; values[1] = 0.0f; values[2] = 0.0f; values[3] = 0.0f; }

    VectorF(float value) { SetValue(value); }
    VectorF(Vector2f value) { SetValue(value); }
    VectorF(Vector3f value) { SetValue(value); }
    VectorF(Vector4f value) { SetValue(value); }

    VectorF(float x, float y) : VectorF(Vector2f(x, y)) { }
    VectorF(float x, float y, float z) : VectorF(Vector3f(x, y, z)) { }
    VectorF(float x, float y, float z, float w) : VectorF(Vector4f(x, y, z, w)) { }

    VectorF(unsigned int _size, float defaultValue = 0.0f) : size(_size) { for (unsigned int i = 0; i < size; ++i) values[i] = defaultValue; }

    VectorF(const VectorF & cpy) : size(cpy.size) { for (unsigned int i = 0; i < size; ++i) values[i] = cpy.values[i]; }


    //Assumes both vectors are the same size. Adds their components together.
    VectorF operator+(const VectorF & other) const;
    //Assumes both vectors are the same size. Subtracts the other's components from this one's components.
    VectorF operator-(const VectorF & other) const;
    //Assumes both vectors are the same size. Multiplies components together.
    VectorF operator*(const VectorF & other) const;
    //Assumes both vectors are the same size. Divides this VectorF's components by the other's components.
    VectorF operator/(const VectorF & other) const;

    bool operator==(const VectorF& other) const
    {
        if (size != other.size)
            return false;

        for (unsigned int i = 0; i < size; ++i)
            if (values[i] != other.values[i])
                return false;

        return true;
    }
    bool operator!=(const VectorF& other) const
    {
        if (size == other.size)
            return false;

        for (unsigned int i = 0; i < size; ++i)
            if (values[i] == other.values[i])
                return false;

        return true;
    }


    std::string GetGLSLType(void) const;

private:

    unsigned int size;
    float values[4];
};

//Represents a collection of 1-4 ints.
//If a VectorI has 0 ints, it is considered invalid.
struct VectorI
{
public:

    //Gets the number of ints this VectorI has.
    unsigned int GetSize(void) const { return size; }
    //The ints representing this VectorI.
    const int * GetValue(void) const { return values; }

    void SetValue(int newVal) { size = 1; values[0] = newVal; }
    void SetValue(Vector2i newVal) { size = 2; values[0] = newVal.x; values[1] = newVal.y; }
    void SetValue(Vector3i newVal) { size = 3; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; }
    void SetValue(Vector4i newVal) { size = 4; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; values[3] = newVal.w; }
    //Marks this VectorI as invalid.
    void SetValue(void) { size = 0; }


    VectorI(void) : size(0) { values[0] = 0.0f; values[1] = 0.0f; values[2] = 0.0f; values[3] = 0.0f; }

    VectorI(int value) { SetValue(value); }
    VectorI(Vector2i value) { SetValue(value); }
    VectorI(Vector3i value) { SetValue(value); }
    VectorI(Vector4i value) { SetValue(value); }

    VectorI(int x, int y) : VectorI(Vector2i(x, y)) { }
    VectorI(int x, int y, int z) : VectorI(Vector3i(x, y, z)) { }
    VectorI(int x, int y, int z, int w) : VectorI(Vector4i(x, y, z, w)) { }

    VectorI(unsigned int _size, int defaultValue = 0.0f) : size(_size) { for (unsigned int i = 0; i < size; ++i) values[i] = defaultValue; }

    VectorI(const VectorI & cpy) : size(cpy.size) { for (unsigned int i = 0; i < size; ++i) values[i] = cpy.values[i]; }


    //Assumes both VectorIs are the same size. Adds their components together.
    VectorI operator+(const VectorI & other) const;
    //Assumes both VectorIs are the same size. Subtracts the other's components from this one's components.
    VectorI operator-(const VectorI & other) const;
    //Assumes both VectorIs are the same size. Multiplies components together.
    VectorI operator*(const VectorI & other) const;
    //Assumes both VectorIs are the same size. Divides this VectorI's components by the other's components.
    VectorI operator/(const VectorI & other) const;

    bool operator==(const VectorI& other) const
    {
        if (size != other.size)
            return false;

        for (unsigned int i = 0; i < size; ++i)
            if (values[i] != other.values[i])
                return false;

        return true;
    }
    bool operator!=(const VectorI& other) const
    {
        if (size == other.size)
            return false;

        for (unsigned int i = 0; i < size; ++i)
            if (values[i] == other.values[i])
                return false;

        return true;
    }


    std::string GetGLSLType(void) const;

private:

    unsigned int size;
    int values[4];
};