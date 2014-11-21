#pragma once

#include "Vectors.h"
#include "../Higher Math/ProjectionInfo.h"

#define ToRadian(x) (x * 0.0174532925f)


class Matrix4f
{
public:

	//Constructors.

	Matrix4f(void) { SetAsIdentity(); }
	Matrix4f(const Matrix4f & cpy) { SetValues(&cpy); }
	
	//Getter/setter stuff.

	void SetValues(const Matrix4f * copy) { SetValuesF([copy](Vector2i l, float & fOut) { fOut = (*copy)[l]; }); }
#pragma warning(disable: 4100)
	void SetValues(float fillValue) { SetValuesF([fillValue](Vector2i l, float &fOut) { fOut = fillValue; }); }
#pragma warning(default: 4100)

	template<typename Func>
	//'evaluator' must be a function in the form "void Func(Vector2i l, float & out)".
	void SetValuesF(Func evaluator)
	{
		int x, y;
		Vector2i l;

		for (x = 0; x < 4; ++x)
		{
			l.x = x;

			for (y = 0; y < 4; ++y)
			{
				l.y = y;

				evaluator(l, values[y][x]);
			}
		}
	}

	float& operator[](Vector2i l) { return values[l.y][l.x]; }
	const float& operator[](Vector2i l) const { return values[l.y][l.x]; }

    Matrix4f & operator=(const Matrix4f & cpy) { SetValues(&cpy); return *this; }
    
	
	//Matrix math.

	static Matrix4f Multiply(const Matrix4f & first, const Matrix4f & second, const Matrix4f & third) { return Multiply(first, Multiply(second, third)); }
	static Matrix4f Multiply(const Matrix4f & lhs, const Matrix4f & rhs);
	static Vector4f Multiply(const Matrix4f & lhs, const Vector4f & rhs);

	float GetDeterminant(void) const;
	Matrix4f GetInverse(void) const;
	Matrix4f GetTranspose(void) const;

	//Rounds all the elements in this matrix to the given number of decimals.
#pragma warning(disable: 4100)
	void Round(int decimals) { SetValuesF([decimals](Vector2i l, float & fOut) { fOut = BasicMath::Round(fOut, decimals); }); }
#pragma warning(default: 4100)

	//Transforms the given vector using this matrix.
	Vector3f Apply(Vector3f v) const { Vector4f val = Matrix4f::Multiply(*this, Vector4f(v, 1.0f)); float iW = 1.0f / val.w; return Vector3f(val.x * iW, val.y * iW, val.z * iW); }


	//Set this matrix to different kinds of transforms.

    void SetAsIdentity() { SetValuesF([](Vector2i l, float& fOut) { if (l.x == l.y) fOut = 1.0f; else fOut = 0.0f; }); }
    void SetAsScale(Vector3f scaleDimensions);
	void SetAsScale(float scale) { SetAsScale(Vector3f(scale, scale, scale)); }
	void SetAsRotateX(float degrees);
	void SetAsRotateY(float degrees);
	void SetAsRotateZ(float degrees);
    void SetAsTranslation(Vector3f pos);
	void SetAsRotation(Vector3f Target, Vector3f Up, bool alreadyNormalized = false);
	void SetAsPerspProj(const ProjectionInfo& p);
    void SetAsOrthoProj(Vector3f minBounds, Vector3f maxBounds);
	void SetAsWVP(const Matrix4f & projM, const Matrix4f & camM, const Matrix4f & worldM);

private:

	float values[4][4];
};
