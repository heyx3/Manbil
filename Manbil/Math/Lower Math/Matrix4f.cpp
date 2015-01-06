#include "Matrix4f.h"

#include "Quaternion.h"

#include <limits>


//Used to greatly simplify access of matrix elements.
#define El(x, y) ((*this)[Vector2u(x, y)])
#define ElMat(x, y, mat) mat[Vector2u(x, y)]



//Smaller matrices are used in finding the determinant/inverse of a 4x4 matrix.
#pragma region Smaller matrices

class Matrix2f
{
public:

    Matrix2f(void) { SetFunc([](Vector2u l, float* fOut) { *fOut = 0.0f; }); }
	Matrix2f(const float copy3_3[][3], Vector2u ignoreRowAndColumn)
	{
        SetFunc([copy3_3, ignoreRowAndColumn](Vector2u l, float* fOut)
		{
			if (l.x >= ignoreRowAndColumn.x)
			{
				l.x += 1;
			}
			if (l.y >= ignoreRowAndColumn.y)
			{
				l.y += 1;
			}

			*fOut = copy3_3[l.y][l.x];
		});
	}
	
	float& operator[](Vector2u l) { return values[l.y][l.x]; }
	const float& operator[](Vector2u l) const { return values[l.y][l.x]; }
	
    void Set(float fillValue)
    {
        SetFunc([fillValue](Vector2u l, float* fOut)
        {
            *fOut = fillValue;
        });
    }
    void Set(const Matrix2f& toCopy)
    {
        memcpy(values, toCopy.values, sizeof(float) * 4);
    }
	
	template<typename Func>
    void SetFunc(Func evaluator)
	{
        for (Vector2u loc; loc.y < 2; ++loc.y)
            for (loc.x = 0; loc.x < 2; ++loc.x)
                evaluator(loc, &values[loc.y][loc.x]);
	}

	float GetDeterminant(void) const
	{
		return (El(0, 0) * El(1, 1)) - (El(1, 0) * El(0, 1));
	}

private:

	float values[2][2];
};
class Matrix3f
{
public:

	Matrix3f(void) { SetFunc([](Vector2u l, float* fOut) { *fOut = 0.0f; }); }
	Matrix3f(const float copy4_4[][4], Vector2u ignoreRowAndColumn)
	{
		SetFunc([&copy4_4, ignoreRowAndColumn](Vector2u l, float* fOut)
		{
			if (l.x >= ignoreRowAndColumn.x)
			{
				l.x += 1;
			}
			if (l.y >= ignoreRowAndColumn.y)
			{
				l.y += 1;
			}

			*fOut = copy4_4[l.y][l.x];
		});
	}

	float& operator[](Vector2u l) { return values[l.y][l.x]; }
	const float& operator[](Vector2u l) const { return values[l.y][l.x]; }

	void Set(float fillValue)
    {
        SetFunc([fillValue](Vector2u l, float* fOut) { *fOut = fillValue; });
    }
	void Set(const Matrix3f& toCopy)
    {
        SetFunc([toCopy](Vector2u l, float* fOut)
        {
            *fOut = toCopy[l];
        });
    }
	
	template<typename Func>
	void SetFunc(Func evaluator)
    {
        for (Vector2u loc; loc.y < 3; ++loc.y)
            for (loc.x = 0; loc.x < 3; ++loc.x)
                evaluator(loc, &values[loc.y][loc.x]);
	}

	float GetDeterminant(void) const
	{
		//Use a recursive algorithm by splitting this matrix into smaller 2x2 matrices.
		
		//For this algorithm, we can freely choose any row/column to use in computing the determinant.
		//Here, we chose the top row.

		bool negative = false;
		Vector2u l(0, 0);
		float determinant = 0.0f;
        float value;

		for (l.x = 0; l.x < 3; ++l.x)
		{
			value = operator[](l);

			if (value != 0.0f)
			{
				determinant += value * (negative ? -1 : 1) * Matrix2f(values, l).GetDeterminant();
			}

			negative = !negative;
		}

		return determinant;
	}

private:

	float values[3][3];
};

#pragma endregion



bool Matrix4f::operator==(const Matrix4f& other) const
{
    for (Vector2u loc; loc.y < 4; ++loc.y)
        for (loc.x = 0; loc.x < 4; ++loc.x)
            if (El(loc.x, loc.y) != ElMat(loc.x, loc.y, other))
                return false;
    return true;
}

Matrix4f Matrix4f::Multiply(Matrix4f const& lhs, Matrix4f const& rhs)
{
	Matrix4f ret;
    ret.SetFunc([&lhs, &rhs](Vector2u loc, float* fOut)
    {
        *fOut = (ElMat(0, loc.y, lhs) * ElMat(loc.x, 0, rhs)) +
                (ElMat(1, loc.y, lhs) * ElMat(loc.x, 1, rhs)) +
                (ElMat(2, loc.y, lhs) * ElMat(loc.x, 2, rhs)) +
                (ElMat(3, loc.y, lhs) * ElMat(loc.x, 3, rhs));
    });
	return ret;
}

Vector4f Matrix4f::Multiply(Matrix4f const& lhs, Vector4f const& rhs)
{
	Vector4f ret;
	ret.x = (ElMat(0, 0, lhs) * rhs.x) + (ElMat(1, 0, lhs) * rhs.y) +
            (ElMat(2, 0, lhs) * rhs.z) + (ElMat(3, 0, lhs) * rhs.w);
    ret.y = (ElMat(0, 1, lhs) * rhs.x) + (ElMat(1, 1, lhs) * rhs.y) +
            (ElMat(2, 1, lhs) * rhs.z) + (ElMat(3, 1, lhs) * rhs.w);
    ret.z = (ElMat(0, 2, lhs) * rhs.x) + (ElMat(1, 2, lhs) * rhs.y) +
            (ElMat(2, 2, lhs) * rhs.z) + (ElMat(3, 2, lhs) * rhs.w);
    ret.w = (ElMat(0, 3, lhs) * rhs.x) + (ElMat(1, 3, lhs) * rhs.y) +
            (ElMat(2, 3, lhs) * rhs.z) + (ElMat(3, 3, lhs) * rhs.w);
	return ret;
}

Vector3f Matrix4f::Apply(Vector3f v) const
{
    Vector4f val = Matrix4f::Multiply(*this, Vector4f(v, 1.0f));
    float iW = 1.0f / val.w;
    return Vector3f(val.x * iW, val.y * iW, val.z * iW);
}

void Matrix4f::SetAsIdentity(void)
{
    SetFunc([](Vector2u l, float* fOut)
    {
        *fOut = (l.x == l.y ? 1.0f : 0.0f);
    });
}
void Matrix4f::SetAsScale(Vector3f scaleDimensions)
{
    Vector4f scale4(scaleDimensions, 1.0f);
    SetFunc([scale4](Vector2u loc, float* fOut)
    {
        *fOut = (loc.x == loc.y ? scale4[loc.x] : 0.0f);
    });
}
void Matrix4f::SetAsRotateX(float radians)
{
	float sinTheta = sinf(radians),
		  cosTheta = cosf(radians);
	
    Set(0.0f);
    El(0, 0) = 1.0f;
    El(3, 3) = 1.0f;

    El(1, 1) = cosTheta;
    El(2, 1) = -sinTheta;
    El(1, 2) = sinTheta;
    El(2, 2) = cosTheta;
}
void Matrix4f::SetAsRotateY(float radians)
{
	float sinTheta = sinf(radians),
		  cosTheta = cosf(radians);

    Set(0.0f);
    El(1, 1) = 1.0f;
    El(3, 3) = 1.0f;

    El(0, 0) = cosTheta;
    El(2, 0) = -sinTheta;
    El(0, 2) = sinTheta;
    El(2, 2) = cosTheta;
}
void Matrix4f::SetAsRotateZ(float radians)
{
	float sinTheta = sinf(radians),
		  cosTheta = cosf(radians);

    Set(0.0f);
    El(2, 2) = 1.0f;
    El(3, 3) = 1.0f;

    El(0, 0) = cosTheta;
    El(1, 0) = -sinTheta;
    El(1, 1) = cosTheta;
    El(0, 1) = sinTheta;
}
void Matrix4f::SetAsRotateXYZ(Vector3f eulers)
{
    Vector3f s(sinf(eulers.x), sinf(eulers.y), sinf(eulers.z)),
             c(cosf(eulers.x), cosf(eulers.y), cosf(eulers.z));

    El(0, 0) = (c.z * c.y) + (s.x * s.y * s.z);   El(1, 0) = -(s.z * c.x);   El(2, 0) = (s.x * c.y * s.z) - (c.z * s.y);     El(3, 0) = 0.0f;
    El(0, 1) = (s.z * c.y) - (s.x * s.y * c.z);   El(1, 1) = (c.x * c.z);    El(2, 1) = -((s.y * s.z) + (s.x * c.y * c.z));  El(3, 1) = 0.0f;
    El(0, 2) = (c.x * s.y);                       El(1, 2) = s.x;            El(2, 2) = (c.x * c.y);                         El(3, 2) = 0.0f;
    El(0, 3) = 0.0f;                              El(1, 3) = 0.0f;           El(2, 3) = 0.0f;                                El(3, 3) = 1.0f;
}
void Matrix4f::SetAsRotation(const Quaternion& rot)
{
    float x2 = rot.x * rot.x,
          y2 = rot.y * rot.y,
          z2 = rot.z * rot.z,
          w2 = rot.w * rot.w,
          xy = rot.x * rot.y,
          xz = rot.x * rot.z,
          yz = rot.y * rot.z,
          wx = rot.w * rot.x,
          wy = rot.w * rot.y,
          wz = rot.w * rot.z;

    El(0, 0) = (w2 + x2 - y2 - z2);  El(1, 0) = 2.0f * (xy - wz);     El(2, 0) = 2.0f * (xz + wy);     El(3, 0) = 0.0f;
    El(0, 1) = 2.0f * (xy + wz);     El(1, 1) = (w2 - x2 + y2 - z2);  El(2, 1) = 2.0f * (yz - wx);     El(3, 1) = 0.0f;
    El(0, 2) = 2.0f * (xz - wy);     El(1, 2) = 2.0f * (yz + wx);     El(2, 2) = (w2 - x2 - y2 + z2);  El(3, 2) = 0.0f;
    El(0, 3) = 0.0f;                 El(1, 3) = 0.0f;                 El(2, 3) = 0.0f;                 El(3, 3) = 1.0f;
}
void Matrix4f::SetAsTranslation(Vector3f pos)
{
	SetAsIdentity();
    El(3, 0) = pos.x;
    El(3, 1) = pos.y;
    El(3, 2) = pos.z;
}
void Matrix4f::SetAsOrientation(Vector3f target, Vector3f up, bool alreadyNormalized)
{
	if (!alreadyNormalized)
	{
		target.Normalize();
		up.Normalize();
	}

    Vector3f right = up.Cross(target);

    up = target.Cross(right);

    right.Normalize();
	up.Normalize();

    El(0, 0) = right.x;   El(1, 0) = right.y;   El(2, 0) = right.z;   El(3, 0) = 0.0f;
    El(0, 1) = up.x;      El(1, 1) = up.y;      El(2, 1) = up.z;      El(3, 1) = 0.0f;
    El(0, 2) = target.x;  El(1, 2) = target.y;  El(2, 2) = target.z;  El(3, 2) = 0.0f;
	El(0, 3) = 0.0f;      El(1, 3) = 0.0f;      El(2, 3) = 0.0f;      El(3, 3) = 1.0f;
}
void Matrix4f::SetAsPerspProj(float fovRadians, float screenW, float screenH, float zNear, float zFar)
{
	const float ar         = screenW / screenH;
	const float zRange     = (zNear - zFar);
	const float tanHalfFOV = tanf(fovRadians * 0.5f);

	El(0, 0) = 1.0f/(tanHalfFOV * ar); El(1, 0) = 0.0f;			     El(2, 0) = 0.0f;					 El(3, 0) = 0.0;
	El(0, 1) = 0.0f;                   El(1, 1) = 1.0f/(tanHalfFOV); El(2, 1) = 0.0f;				     El(3, 1) = 0.0;
	El(0, 2) = 0.0f;                   El(1, 2) = 0.0f;			     El(2, 2) = -(zNear + zFar)/zRange;  El(3, 2) = 2.0f * zFar * zNear / zRange;
	El(0, 3) = 0.0f;                   El(1, 3) = 0.0f;			     El(2, 3) = 1.0f;					 El(3, 3) = 0.0;    
}
void Matrix4f::SetAsOrthoProj(Vector3f minBounds, Vector3f maxBounds)
{
	float width = (maxBounds.x - minBounds.x),
		  height = (maxBounds.y - minBounds.y),
		  depth = (maxBounds.z - minBounds.z);
	
	El(0, 0) = 2 / width;	El(1, 0) = 0;			El(2, 0) = 0;			El(3, 0) = -(maxBounds.x + minBounds.x) / width;
    El(0, 1) = 0;			El(1, 1) = 2 / height;	El(2, 1) = 0;			El(3, 1) = -(maxBounds.y + minBounds.y) / height;
    El(0, 2) = 0;			El(1, 2) = 0;			El(2, 2) = 2 / depth;	El(3, 2) = -(maxBounds.z + minBounds.z) / depth;
	El(0, 3) = 0;			El(1, 3) = 0;			El(2, 3) = 0;			El(3, 3) = 1;
}
void Matrix4f::SetAsWVP(const Matrix4f& projM, const Matrix4f& camM, const Matrix4f& worldM)
{
	Set(Matrix4f::Multiply(projM, camM, worldM));
}

float Matrix4f::GetDeterminant(void) const
{
	//Use a recursive algorithm for finding it by splitting the matrix into smaller matrices.

	//For this algorithm, we can freely choose which row/column to use for computing the determinant.
	//The bottom row is the most likely to have a lot of zeros, so we use it to simplify computing.

	bool negative = true;
	Vector2u l(0, 3);
	float determinant = 0.0f;
    float value;
	Matrix3f temp;

	for (l.x = 0; l.x < 4; ++l.x)
	{
		value = operator[](l);

		//This is an expensive operation, so don't bother doing it if it will come out to 0 anyway.
		if (value != 0.0f)
		{
			temp = Matrix3f(values, l);
			determinant += value * (negative ? -1 : 1) * temp.GetDeterminant();
		}

		negative = !negative;
	}

	return determinant;
}
Matrix4f Matrix4f::GetInverse(void) const
{
	Matrix4f ret;

	// Compute the reciprocal determinant
	float det = GetDeterminant();
	if(det == 0.0f) 
	{
		ret.Set(Mathf::NaN);
		return ret;
	}

	float invdet = 1.0f / det;

    //There's no simple, efficient way to do this :(.
	ElMat(0, 0, ret) = invdet * ((El(1, 1) * El(2, 2) * El(3, 3)) +
						         (El(2, 1) * El(3, 2) * El(1, 3)) +
								 (El(3, 1) * El(1, 2) * El(2, 3)) -
								 (El(1, 1) * El(3, 2) * El(2, 3)) -
								 (El(2, 1) * El(1, 2) * El(3, 3)) -
								 (El(3, 1) * El(2, 2) * El(1, 3)));
	ElMat(1, 0, ret) = invdet * ((El(1, 0) * El(3, 2) * El(2, 3)) +
								 (El(2, 0) * El(1, 2) * El(3, 3)) +
							     (El(3, 0) * El(2, 2) * El(1, 3)) - 
								 (El(1, 0) * El(2, 2) * El(3, 3)) - 
								 (El(2, 0) * El(3, 2) * El(1, 3)) - 
								 (El(3, 0) * El(1, 2) * El(2, 3)));
	ElMat(2, 0, ret) = invdet * ((El(1, 0) * El(2, 1) * El(3, 3)) + 
								 (El(2, 0) * El(3, 1) * El(1, 3)) + 
								 (El(3, 0) * El(1, 1) * El(2, 3)) - 
								 (El(0, 1) * El(3, 1) * El(2, 3)) - 
								 (El(2, 0) * El(1, 1) * El(3, 3)) - 
								 (El(3, 0) * El(2, 1) * El(1, 3)));
    ElMat(3, 0, ret) = invdet * ((El(1, 0) * El(3, 1) * El(2, 2)) +
								 (El(2, 0) * El(1, 1) * El(3, 2)) + 
								 (El(3, 0) * El(2, 1) * El(1, 2)) - 
								 (El(1, 0) * El(2, 1) * El(3, 2)) - 
								 (El(2, 0) * El(3, 1) * El(1, 2)) - 
								 (El(3, 0) * El(1, 1) * El(2, 2)));
    ElMat(0, 1, ret) = invdet * ((El(0, 1) * El(3, 2) * El(2, 3)) +
								 (El(2, 1) * El(0, 2) * El(3, 3)) + 
								 (El(3, 1) * El(2, 2) * El(0, 3)) - 
								 (El(0, 1) * El(2, 2) * El(3, 3)) - 
								 (El(2, 1) * El(3, 2) * El(0, 3)) - 
								 (El(3, 1) * El(0, 2) * El(2, 3)));
    ElMat(1, 1, ret) = invdet * ((El(0, 0) * El(2, 2) * El(3, 3)) +
								 (El(2, 0) * El(3, 2) * El(0, 3)) + 
								 (El(3, 0) * El(0, 2) * El(2, 3)) - 
								 (El(0, 0) * El(3, 2) * El(2, 3)) - 
								 (El(2, 0) * El(0, 2) * El(3, 3)) - 
								 (El(3, 0) * El(2, 2) * El(0, 3)));
    ElMat(2, 1, ret) = invdet * ((El(0, 0) * El(3, 1) * El(2, 3)) +
								 (El(2, 0) * El(0, 1) * El(3, 3)) + 
								 (El(3, 0) * El(2, 1) * El(0, 3)) - 
								 (El(0, 0) * El(2, 1) * El(3, 3)) - 
								 (El(2, 0) * El(3, 1) * El(0, 3)) - 
								 (El(3, 0) * El(0, 1) * El(2, 3)));
    ElMat(3, 1, ret) = invdet * ((El(0, 0) * El(2, 1) * El(3, 2)) +
								 (El(2, 0) * El(3, 1) * El(0, 2)) + 
								 (El(3, 0) * El(0, 1) * El(2, 2)) - 
								 (El(0, 0) * El(3, 1) * El(2, 2)) - 
								 (El(2, 0) * El(0, 1) * El(3, 2)) - 
								 (El(3, 0) * El(2, 1) * El(0, 2)));
    ElMat(0, 2, ret) = invdet * ((El(0, 1) * El(1, 2) * El(3, 3)) +
								 (El(1, 1) * El(3, 2) * El(0, 3)) + 
								 (El(3, 1) * El(0, 2) * El(1, 3)) - 
								 (El(0, 1) * El(3, 2) * El(1, 3)) - 
								 (El(1, 1) * El(0, 2) * El(3, 3)) - 
								 (El(3, 1) * El(1, 2) * El(0, 3)));
    ElMat(1, 2, ret) = invdet * ((El(0, 0) * El(3, 2) * El(1, 3)) +
								 (El(1, 0) * El(0, 2) * El(3, 3)) + 
								 (El(3, 0) * El(1, 2) * El(0, 3)) - 
								 (El(0, 0) * El(1, 2) * El(3, 3)) - 
								 (El(1, 0) * El(3, 2) * El(0, 3)) - 
								 (El(3, 0) * El(0, 2) * El(1, 3)));
    ElMat(2, 2, ret) = invdet * ((El(0, 0) * El(1, 1) * El(3, 3)) +
								 (El(1, 0) * El(3, 1) * El(0, 3)) + 
								 (El(3, 0) * El(0, 1) * El(1, 3)) -
								 (El(0, 0) * El(3, 1) * El(1, 3)) - 
								 (El(1, 0) * El(0, 1) * El(3, 3)) - 
								 (El(3, 0) * El(1, 1) * El(0, 3)));
    ElMat(3, 2, ret) = invdet * ((El(0, 0) * El(3, 1) * El(1, 2)) +
								 (El(1, 0) * El(0, 1) * El(3, 2)) + 
								 (El(3, 0) * El(1, 1) * El(0, 2)) - 
								 (El(0, 0) * El(1, 1) * El(3, 2)) - 
								 (El(1, 0) * El(3, 1) * El(0, 2)) - 
								 (El(3, 0) * El(0, 1) * El(1, 2)));
    ElMat(0, 3, ret) = invdet * ((El(0, 1) * El(2, 2) * El(1, 3)) +
								 (El(1, 1) * El(0, 2) * El(2, 3)) + 
								 (El(2, 1) * El(1, 2) * El(0, 3)) - 
								 (El(0, 1) * El(1, 2) * El(2, 3)) - 
								 (El(1, 1) * El(2, 2) * El(0, 3)) - 
								 (El(2, 1) * El(0, 2) * El(1, 3)));
    ElMat(1, 3, ret) = invdet * ((El(0, 0) * El(1, 2) * El(2, 3)) +
								 (El(1, 0) * El(2, 2) * El(0, 3)) + 
								 (El(2, 0) * El(0, 2) * El(1, 3)) - 
								 (El(0, 0) * El(2, 2) * El(1, 3)) - 
								 (El(1, 0) * El(0, 2) * El(2, 3)) - 
								 (El(2, 0) * El(1, 2) * El(0, 3)));
    ElMat(2, 3, ret) = invdet * ((El(0, 0) * El(2, 1) * El(1, 3)) +
								 (El(1, 0) * El(0, 1) * El(2, 3)) + 
								 (El(2, 0) * El(1, 1) * El(0, 3)) - 
								 (El(0, 0) * El(1, 1) * El(2, 3)) - 
								 (El(1, 0) * El(2, 1) * El(0, 3)) - 
								 (El(2, 0) * El(0, 1) * El(1, 3)));
    ElMat(3, 3, ret) = invdet * ((El(0, 0) * El(1, 1) * El(2, 2)) +
								 (El(1, 0) * El(2, 1) * El(0, 2)) + 
								 (El(2, 0) * El(0, 1) * El(1, 2)) - 
								 (El(0, 0) * El(2, 1) * El(1, 2)) - 
								 (El(1, 0) * El(0, 1) * El(2, 2)) - 
								 (El(2, 0) * El(1, 1) * El(0, 2))); 

	return ret;
}
Matrix4f Matrix4f::GetTranspose(void) const
{
	Matrix4f ret, thisM = *this;

	ret.SetFunc([&thisM](Vector2u l, float* fOut)
	{
		*fOut = thisM[Vector2u(l.y, l.x)];
	});

	return ret;
}