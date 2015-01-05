#pragma once

#include "Vectors.h"


class Quaternion;

#pragma warning(disable: 4100)

//A pre-multiplied matrix (in other words it is used like "Matrix * Vector" instead of "Vector * Matrix").
class Matrix4f
{
public:

   
    //Gets a new matrix whose transform is equivalent to "rhs" transform followed by "lhs" transform.
    static Matrix4f Multiply(const Matrix4f& lhs, const Matrix4f& rhs);
    //Pre-multiplies the given matrix by a Vector4f (essentially a 1x4 matrix).
    static Vector4f Multiply(const Matrix4f& lhs, const Vector4f& rhs);
    //Gets a new matrix whose transform is equivalent to "three" transform
    //    followed by "two" transform followed by "one" transform.
    static inline Matrix4f Multiply(const Matrix4f& one, const Matrix4f& two, const Matrix4f& three)
    {
        return Multiply(one, Multiply(two, three));
    }

	
	Matrix4f(void) { SetAsIdentity(); }
	Matrix4f(const Matrix4f& cpy) { Set(cpy); }
	

    Matrix4f& operator=(const Matrix4f& cpy) { Set(cpy); return *this; }

	float& operator[](Vector2u l) { return values[l.y][l.x]; }
	const float& operator[](Vector2u l) const { return values[l.y][l.x]; }

    bool operator==(const Matrix4f& other) const;
    

	void Set(const Matrix4f& copy) { memcpy(values, copy.values, sizeof(float) * 16); }
	void Set(float fillValue) { SetFunc([fillValue](Vector2u l, float* f) { *f = fillValue; }); }

    //A function with the signature "void GetElement(Vector2u gridLoc, float* outVal)".
	template<typename Func>
    //Sets this matrix's value using the given function.
	void SetFunc(Func evaluator)
	{
        for (Vector2u loc; loc.y < 4; ++loc.y)
            for (loc.x = 0; loc.x < 4; ++loc.x)
                evaluator(loc, &values[loc.y][loc.x]);
	}


	float GetDeterminant(void) const;
	Matrix4f GetTranspose(void) const;

    //If this matrix cannot be inverted, returns a matrix with all values set to NaN.
    Matrix4f GetInverse(void) const;


	//Transforms the given vector using this matrix.
	Vector3f Apply(Vector3f v) const;


    void SetAsIdentity(void);
    void SetAsScale(Vector3f scaleDimensions);
	void SetAsScale(float scale) { SetAsScale(Vector3f(scale, scale, scale)); }
	void SetAsRotateX(float radians);
    void SetAsRotateY(float radians);
    void SetAsRotateZ(float radians);
    //Creates a matrix that rotates by the given amounts around each axis.
    //First by Y axis, then by X, then by Z.
    void SetAsRotateXYZ(Vector3f eulerAngles);
    void SetAsRotation(const Quaternion& rot);
    void SetAsTranslation(Vector3f pos);
    //Creates a matrix that rotates space so that "target" is pointing along the Z
    //    and "up" is pointing along the Y.
	void SetAsOrientation(Vector3f target, Vector3f up, bool alreadyNormalized = false);
	void SetAsPerspProj(float fovRadians, float screenW, float screenH, float zNear, float zFar);
    void SetAsOrthoProj(Vector3f minBounds, Vector3f maxBounds);
	void SetAsWVP(const Matrix4f& projM, const Matrix4f& camM, const Matrix4f& worldM);

private:

	float values[4][4];
};

#pragma warning(default: 4100)