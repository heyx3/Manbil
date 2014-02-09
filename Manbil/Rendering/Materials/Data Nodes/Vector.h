#pragma once

#include <string>
#include "../../Math/Vectors.h"

//Represents a collection of 1-4 floats.
//If a Vector has 0 floats, it is considered invalid.
struct Vector
{
public:

    //Gets the number of floats this Vector has.
    unsigned int GetSize(void) const { return size; }
    //The floats representing this Vector.
    const float * GetValue(void) const { return values; }

    void SetValue(float newVal) { size = 1; values[0] = newVal; }
    void SetValue(Vector2f newVal) { size = 2; values[0] = newVal.x; values[1] = newVal.y; }
    void SetValue(Vector3f newVal) { size = 3; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; }
    void SetValue(Vector4f newVal) { size = 4; values[0] = newVal.x; values[1] = newVal.y; values[2] = newVal.z; values[3] = newVal.w; }
    //Marks this Vector as invalid.
    void SetValue(void) { size = 0; }

    Vector(void) : size(0) { values[0] = 0.0f; values[1] = 0.0f; values[2] = 0.0f; values[3] = 0.0f; }
    Vector(float value) { SetValue(value); }
    Vector(Vector2f value) { SetValue(value); }
    Vector(Vector3f value) { SetValue(value); }
    Vector(Vector4f value) { SetValue(value); }
    Vector(unsigned int _size, float defaultValue = 0.0f) : size(_size) { for (unsigned int i = 0; i < size; ++i) values[i] = defaultValue; }
    Vector(const Vector & cpy) : size(cpy.size) { for (unsigned int i = 0; i < size; ++i) values[i] = cpy.values[i]; }

    //Assumes both vectors are the same size. Adds their components together.
    Vector operator+(const Vector & other) const;
    //Assumes both vectors are the same size. Subtracts the other's components from this one's components.
    Vector operator-(const Vector & other) const;
    //Assumes both vectors are the same size. Multiplies components together.
    Vector operator*(const Vector & other) const;
    //Assumes both vectors are the same size. Divides this Vector's components by the other's components.
    Vector operator/(const Vector & other) const;

    std::string GetGLSLType(void) const;

private:

    unsigned int size;
    float values[4];
};