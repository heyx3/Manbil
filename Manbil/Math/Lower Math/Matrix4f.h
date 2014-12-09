#pragma once

#include "Vectors.h"



#pragma warning(disable: 4100)

//A pre-multiplied matrix (in other words it is used like "Matrix * Vector" instead of "Vector * Matrix").
class Matrix4f
{
public:

   
    static inline Matrix4f Multiply(const Matrix4f& one, const Matrix4f& two, const Matrix4f& three)
    {
        return Multiply(one, Multiply(two, three));
    }
    static Matrix4f Multiply(const Matrix4f& lhs, const Matrix4f& rhs);
    static Vector4f Multiply(const Matrix4f& lhs, const Vector4f& rhs);

	
	Matrix4f(void) { SetAsIdentity(); }
	Matrix4f(const Matrix4f& cpy) { Set(cpy); }
	

    Matrix4f & operator=(const Matrix4f& cpy) { Set(cpy); return *this; }

	float& operator[](Vector2u l) { return values[l.y][l.x]; }
	const float& operator[](Vector2u l) const { return values[l.y][l.x]; }

    bool operator==(const Matrix4f& other) const;
    

	void Set(const Matrix4f& copy)
    {
        memcpy(values, copy.values, sizeof(float) * 16);
    }
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
	Vector3f Apply(Vector3f v) const { Vector4f val = Matrix4f::Multiply(*this, Vector4f(v, 1.0f)); float iW = 1.0f / val.w; return Vector3f(val.x * iW, val.y * iW, val.z * iW); }


    void SetAsIdentity(void);
    void SetAsScale(Vector3f scaleDimensions);
	void SetAsScale(float scale) { SetAsScale(Vector3f(scale, scale, scale)); }
	void SetAsRotateX(float degrees);
	void SetAsRotateY(float degrees);
	void SetAsRotateZ(float degrees);
    void SetAsTranslation(Vector3f pos);
	void SetAsRotation(Vector3f Target, Vector3f Up, bool alreadyNormalized = false);
	void SetAsPerspProj(float fovRadians, float screenW, float screenH, float zNear, float zFar);
    void SetAsOrthoProj(Vector3f minBounds, Vector3f maxBounds);
	void SetAsWVP(const Matrix4f& projM, const Matrix4f& camM, const Matrix4f& worldM);

private:

	float values[4][4];
};

#pragma warning(default: 4100)