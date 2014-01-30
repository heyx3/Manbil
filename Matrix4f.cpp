#include "Matrix4f.h"

#include <limits>

#define El(x, y) ((*this)[Vector2i(x, y)])

#pragma region Smaller matrices

//Smaller matrices used in finding the determinant/inverse of a 4x4.

class Matrix2f
{
public:

	Matrix2f(void) { SetValues([](Vector2i l, float & fOut) { fOut = 0.0f; }); }
	Matrix2f(const float copy3_3[][3], Vector2i ignoreRowAndColumn)
	{
		SetValues([copy3_3, ignoreRowAndColumn](Vector2i l, float & fOut)
		{
			if (l.x >= ignoreRowAndColumn.x)
			{
				l.x += 1;
			}
			if (l.y >= ignoreRowAndColumn.y)
			{
				l.y += 1;
			}

			fOut = copy3_3[l.y][l.x];
		});
	}
	
	float& operator[](Vector2i l) { return values[l.y][l.x]; }
	const float& operator[](Vector2i l) const { return values[l.y][l.x]; }
	
	void SetValues(float fillValue) { SetValues([fillValue](Vector2i l, float & fOut) { fOut = fillValue; }); }
	void SetValues(Matrix2f & toCopy) { SetValues([toCopy](Vector2i l, float & fOut) { fOut = toCopy[l]; }); }
	
	template<typename Func>
	void SetValues(Func evaluator)
	{
		int x, y;
		Vector2i l;
		for (x = 0; x < 2; ++x)
		{
			l.x = x;
			for (y = 0; y < 2; ++y)
			{
				l.y = y;
				evaluator(l, values[l.y][l.x]);
			}
		}
	}

	float Determinant(void) const
	{
		return (El(0, 0) * El(1, 1)) - (El(1, 0) * El(0, 1));
	}

private:

	float values[2][2];
};
class Matrix3f
{
public:

	Matrix3f(void) { SetValues([](Vector2i l, float & fOut) { fOut = 0.0f; }); }
	Matrix3f(const float copy4_4[][4], Vector2i ignoreRowAndColumn)
	{
		SetValues([&copy4_4, ignoreRowAndColumn](Vector2i l, float & fOut)
		{
			if (l.x >= ignoreRowAndColumn.x)
			{
				l.x += 1;
			}
			if (l.y >= ignoreRowAndColumn.y)
			{
				l.y += 1;
			}

			fOut = copy4_4[l.y][l.x];
		});
	}

	float& operator[](Vector2i l) { return values[l.y][l.x]; }
	const float& operator[](Vector2i l) const { return values[l.y][l.x]; }

	void SetValues(float fillValue) { SetValues([fillValue](Vector2i l, float & fOut) { fOut = fillValue; }); }
	void SetValues(const Matrix3f & toCopy) { SetValues([toCopy](Vector2i l, float & fOut) { fOut = toCopy[l]; }); }
	
	template<typename Func>
	void SetValues(Func evaluator)
	{
		int x, y;
		Vector2i l;
		for (x = 0; x < 3; ++x)
		{
			l.x = x;
			for (y = 0; y < 3; ++y)
			{
				l.y = y;
				evaluator(l, values[y][x]);
			}
		}
	}

	float Determinant(void) const
	{
		//Use a recursive algorithm by splitting this matrix into smaller 2x2 matrices.
		
		//For this algorithm, we can freely choose which row/column to use for computing the determinant.
		//Here, we chose the top row.

		bool negative = false;
		Vector2i l(0, 0);
		float determinant = 0.0f, value;

		for (int x = 0; x < 3; ++x)
		{
			l.x = x;
			value = operator[](l);

			if (value != 0.0f)
			{
				determinant += value * (negative ? -1 : 1) * Matrix2f(values, l).Determinant();
			}

			negative = !negative;
		}

		return determinant;
	}

private:

	float values[3][3];
};

#pragma endregion


Matrix4f Matrix4f::Multiply(Matrix4f const& lhs, Matrix4f const& rhs)
{
	Matrix4f ret;
	int x, y;

	for (x = 0; x < 4; ++x)
	{
		for (y = 0; y < 4; ++y)
		{
			ret[Vector2i(x, y)] = (lhs[Vector2i(0, y)] * rhs[Vector2i(x, 0)]) +
								  (lhs[Vector2i(1, y)] * rhs[Vector2i(x, 1)]) +
								  (lhs[Vector2i(2, y)] * rhs[Vector2i(x, 2)]) +
								  (lhs[Vector2i(3, y)] * rhs[Vector2i(x, 3)]);
		}
	}
	return ret;
}

Vector4f Matrix4f::Multiply(Matrix4f const& lhs, Vector4f const& rhs)
{
	Vector4f ret;
	
	ret.x = (lhs[Vector2i(0, 0)] * rhs.x) + (lhs[Vector2i(1, 0)] * rhs.y) + (lhs[Vector2i(2, 0)] * rhs.z) + (lhs[Vector2i(3, 0)] * rhs.w);
	ret.y = (lhs[Vector2i(0, 1)] * rhs.x) + (lhs[Vector2i(1, 1)] * rhs.y) + (lhs[Vector2i(2, 1)] * rhs.z) + (lhs[Vector2i(3, 1)] * rhs.w);
	ret.z = (lhs[Vector2i(0, 2)] * rhs.x) + (lhs[Vector2i(1, 2)] * rhs.y) + (lhs[Vector2i(2, 2)] * rhs.z) + (lhs[Vector2i(3, 2)] * rhs.w);
	ret.w = (lhs[Vector2i(0, 3)] * rhs.x) + (lhs[Vector2i(1, 3)] * rhs.y) + (lhs[Vector2i(2, 3)] * rhs.z) + (lhs[Vector2i(3, 3)] * rhs.w);

	return ret;
}

void Matrix4f::SetAsScale(Vector3f scaleDimensions)
{
	float x = scaleDimensions.x,
		y = scaleDimensions.y,
		z = scaleDimensions.z;

	SetAsIdentity();
	(*this)[Vector2i(0, 0)] = x;
	(*this)[Vector2i(1, 1)] = y;
	(*this)[Vector2i(2, 2)] = z;
}
void Matrix4f::SetAsRotateX(float radians)
{
	float sinTheta = sinf(radians),
		  cosTheta = cosf(radians);
	
	SetAsIdentity();

	(*this)[Vector2i(1, 1)] = cosTheta;
	(*this)[Vector2i(2, 1)] = -sinTheta;
	(*this)[Vector2i(1, 2)] = sinTheta;
	(*this)[Vector2i(2, 2)] = cosTheta;
}
void Matrix4f::SetAsRotateY(float radians)
{
	float sinTheta = sinf(radians),
		cosTheta = cosf(radians);

	SetAsIdentity();

	(*this)[Vector2i(0, 0)] = cosTheta;
	(*this)[Vector2i(2, 0)] = -sinTheta;
	(*this)[Vector2i(0, 2)] = sinTheta;
	(*this)[Vector2i(2, 2)] = cosTheta;
}
void Matrix4f::SetAsRotateZ(float radians)
{
	float sinTheta = sinf(radians),
		cosTheta = cosf(radians);

	SetAsIdentity();

	(*this)[Vector2i(0, 0)] = cosTheta;
	(*this)[Vector2i(0, 1)] = sinTheta;
	(*this)[Vector2i(1, 0)] = -sinTheta;
	(*this)[Vector2i(1, 1)] = cosTheta;
}
void Matrix4f::SetAsTranslation(Vector3f pos)
{
	SetAsIdentity();

	(*this)[Vector2i(3, 0)] = pos.x;
	(*this)[Vector2i(3, 1)] = pos.y;
	(*this)[Vector2i(3, 2)] = pos.z;
}
void Matrix4f::SetAsRotation(Vector3f Target, Vector3f Up, bool alreadyNormalized)
{
	Vector3f n = Target;
	Vector3f u = Up;
	if (!alreadyNormalized)
	{
		n.Normalize();
		u.Normalize();
	}

	u = u.Cross(n);

	Vector3f v = n.Cross(u);

	Vector3f U = u.Normalized(),
			 V = v.Normalized(),
			 N = n;

	(*this)[Vector2i(0, 0)] = U.x;   (*this)[Vector2i(1, 0)] = U.y;   (*this)[Vector2i(2, 0)] = U.z;   (*this)[Vector2i(3, 0)] = 0.0f;
	(*this)[Vector2i(0, 1)] = V.x;   (*this)[Vector2i(1, 1)] = V.y;   (*this)[Vector2i(2, 1)] = V.z;   (*this)[Vector2i(3, 1)] = 0.0f;
	(*this)[Vector2i(0, 2)] = N.x;   (*this)[Vector2i(1, 2)] = N.y;   (*this)[Vector2i(2, 2)] = N.z;   (*this)[Vector2i(3, 2)] = 0.0f;
	(*this)[Vector2i(0, 3)] = 0.0f;  (*this)[Vector2i(1, 3)] = 0.0f;  (*this)[Vector2i(2, 3)] = 0.0f;  (*this)[Vector2i(3, 3)] = 1.0f;
}
void Matrix4f::SetAsPerspProj(const ProjectionInfo& p)
{
	const float ar         = p.Width / p.Height;
	const float zRange     = (p.zNear - p.zFar);
	const float tanHalfFOV = tanf(p.FOV * 0.5f);

	El(0, 0) = 1.0f/(tanHalfFOV * ar); El(1, 0) = 0.0f;			     El(2, 0) = 0.0f;						 El(3, 0) = 0.0;
	El(0, 1) = 0.0f;                   El(1, 1) = 1.0f/(tanHalfFOV); El(2, 1) = 0.0f;						 El(3, 1) = 0.0;
	El(0, 2) = 0.0f;                   El(1, 2) = 0.0f;			     El(2, 2) = -(p.zNear + p.zFar)/zRange;  El(3, 2) = 2.0f * p.zFar * p.zNear / zRange;
	El(0, 3) = 0.0f;                   El(1, 3) = 0.0f;			     El(2, 3) = 1.0f;					     El(3, 3) = 0.0;    
}
void Matrix4f::SetAsOrthoProj(const ProjectionInfo & p)
{
	//TODO: Test.

	float width = p.Width,
		  height = p.Height,
		  depth = p.zFar - p.zNear;
	
	El(0, 0) = 2 / width;	El(1, 0) = 0;			El(2, 0) = 0;			El(3, 0) = 0;
	El(0, 1) = 0;			El(1, 1) = 2 / height;	El(2, 1) = 0;			El(3, 1) = 0;
	El(0, 2) = 0;			El(1, 2) = 0;			El(2, 2) = 2 / depth;	El(3, 2) = 0;
	El(0, 3) = 0;			El(1, 3) = 0;			El(2, 3) = 0;			El(3, 3) = 1;
}
void Matrix4f::SetAsWVP(const Matrix4f & projM, const Matrix4f & camM, const Matrix4f & worldM)
{
	Matrix4f finalM = Matrix4f::Multiply(projM, camM, worldM);
	SetValues(&finalM);
}

float Matrix4f::Determinant(void) const
{
	//Use a recursive algorithm for finding it by splitting the matrix into smaller matrices.

	//For this algorithm, we can freely choose which row/column to use for computing the determinant.
	//The bottom row is the most likely to have a lot of zeros, so we use it to simplify computing.

	bool negative = true;
	Vector2i l(0, 3);
	float determinant = 0.0f, value;
	Matrix3f temp;

	for (int x = 0; x < 4; ++x)
	{
		l.x = x;
		value = operator[](l);

		//This is kind of an expensive operation, so don't bother doing it if it will come out to 0 anyway.
		if (value != 0.0f)
		{
			temp = Matrix3f(values, l);
			determinant += value * (negative ? -1 : 1) * temp.Determinant();
		}

		negative = !negative;
	}

	return determinant;
}
Matrix4f Matrix4f::Inverse(void) const
{
	Matrix4f ret;

	// Compute the reciprocal determinant
	float det = Determinant();
	if(det == 0.0f) 
	{
		// Matrix not invertible. Setting all elements to nan is not really
		// correct in a mathematical sense but it is easy to debug for the
		// programmer.
		const float nan = std::numeric_limits<float>::quiet_NaN();
		ret.SetValues(nan);
		return ret;
	}

	float invdet = 1.0f / det;

	//Yay for hard-coding!

	ret[Vector2i(0, 0)] = invdet * ((El(1, 1) * El(2, 2) * El(3, 3)) +
									(El(2, 1) * El(3, 2) * El(1, 3)) +
									(El(3, 1) * El(1, 2) * El(2, 3)) -
									(El(1, 1) * El(3, 2) * El(2, 3)) -
									(El(2, 1) * El(1, 2) * El(3, 3)) -
									(El(3, 1) * El(2, 2) * El(1, 3)));
	ret[Vector2i(1, 0)] = invdet * ((El(1, 0) * El(3, 2) * El(2, 3)) +
								    (El(2, 0) * El(1, 2) * El(3, 3)) +
									(El(3, 0) * El(2, 2) * El(1, 3)) - 
									(El(1, 0) * El(2, 2) * El(3, 3)) - 
									(El(2, 0) * El(3, 2) * El(1, 3)) - 
									(El(3, 0) * El(1, 2) * El(2, 3)));
	ret[Vector2i(2, 0)] = invdet * ((El(1, 0) * El(2, 1) * El(3, 3)) + 
									(El(2, 0) * El(3, 1) * El(1, 3)) + 
									(El(3, 0) * El(1, 1) * El(2, 3)) - 
									(El(0, 1) * El(3, 1) * El(2, 3)) - 
									(El(2, 0) * El(1, 1) * El(3, 3)) - 
									(El(3, 0) * El(2, 1) * El(1, 3)));
	ret[Vector2i(3, 0)] = invdet * ((El(1, 0) * El(3, 1) * El(2, 2)) + 
									(El(2, 0) * El(1, 1) * El(3, 2)) + 
									(El(3, 0) * El(2, 1) * El(1, 2)) - 
									(El(1, 0) * El(2, 1) * El(3, 2)) - 
									(El(2, 0) * El(3, 1) * El(1, 2)) - 
									(El(3, 0) * El(1, 1) * El(2, 2)));
	ret[Vector2i(0, 1)] = invdet * ((El(0, 1) * El(3, 2) * El(2, 3)) + 
									(El(2, 1) * El(0, 2) * El(3, 3)) + 
									(El(3, 1) * El(2, 2) * El(0, 3)) - 
									(El(0, 1) * El(2, 2) * El(3, 3)) - 
									(El(2, 1) * El(3, 2) * El(0, 3)) - 
									(El(3, 1) * El(0, 2) * El(2, 3)));
	ret[Vector2i(1, 1)] = invdet * ((El(0, 0) * El(2, 2) * El(3, 3)) + 
									(El(2, 0) * El(3, 2) * El(0, 3)) + 
									(El(3, 0) * El(0, 2) * El(2, 3)) - 
									(El(0, 0) * El(3, 2) * El(2, 3)) - 
									(El(2, 0) * El(0, 2) * El(3, 3)) - 
									(El(3, 0) * El(2, 2) * El(0, 3)));
	ret[Vector2i(2, 1)] = invdet * ((El(0, 0) * El(3, 1) * El(2, 3)) + 
									(El(2, 0) * El(0, 1) * El(3, 3)) + 
									(El(3, 0) * El(2, 1) * El(0, 3)) - 
									(El(0, 0) * El(2, 1) * El(3, 3)) - 
									(El(2, 0) * El(3, 1) * El(0, 3)) - 
									(El(3, 0) * El(0, 1) * El(2, 3)));
	ret[Vector2i(3, 1)] = invdet * ((El(0, 0) * El(2, 1) * El(3, 2)) + 
									(El(2, 0) * El(3, 1) * El(0, 2)) + 
									(El(3, 0) * El(0, 1) * El(2, 2)) - 
									(El(0, 0) * El(3, 1) * El(2, 2)) - 
									(El(2, 0) * El(0, 1) * El(3, 2)) - 
									(El(3, 0) * El(2, 1) * El(0, 2)));
	ret[Vector2i(0, 2)] = invdet * ((El(0, 1) * El(1, 2) * El(3, 3)) + 
									(El(1, 1) * El(3, 2) * El(0, 3)) + 
									(El(3, 1) * El(0, 2) * El(1, 3)) - 
									(El(0, 1) * El(3, 2) * El(1, 3)) - 
									(El(1, 1) * El(0, 2) * El(3, 3)) - 
									(El(3, 1) * El(1, 2) * El(0, 3)));
	ret[Vector2i(1, 2)] = invdet * ((El(0, 0) * El(3, 2) * El(1, 3)) + 
									(El(1, 0) * El(0, 2) * El(3, 3)) + 
									(El(3, 0) * El(1, 2) * El(0, 3)) - 
									(El(0, 0) * El(1, 2) * El(3, 3)) - 
									(El(1, 0) * El(3, 2) * El(0, 3)) - 
									(El(3, 0) * El(0, 2) * El(1, 3)));
	ret[Vector2i(2, 2)] = invdet * ((El(0, 0) * El(1, 1) * El(3, 3)) + 
									(El(1, 0) * El(3, 1) * El(0, 3)) + 
									(El(3, 0) * El(0, 1) * El(1, 3)) -
									(El(0, 0) * El(3, 1) * El(1, 3)) - 
									(El(1, 0) * El(0, 1) * El(3, 3)) - 
									(El(3, 0) * El(1, 1) * El(0, 3)));
	ret[Vector2i(3, 2)] = invdet * ((El(0, 0) * El(3, 1) * El(1, 2)) + 
									(El(1, 0) * El(0, 1) * El(3, 2)) + 
									(El(3, 0) * El(1, 1) * El(0, 2)) - 
									(El(0, 0) * El(1, 1) * El(3, 2)) - 
									(El(1, 0) * El(3, 1) * El(0, 2)) - 
									(El(3, 0) * El(0, 1) * El(1, 2)));
	ret[Vector2i(0, 3)] = invdet * ((El(0, 1) * El(2, 2) * El(1, 3)) + 
									(El(1, 1) * El(0, 2) * El(2, 3)) + 
									(El(2, 1) * El(1, 2) * El(0, 3)) - 
									(El(0, 1) * El(1, 2) * El(2, 3)) - 
									(El(1, 1) * El(2, 2) * El(0, 3)) - 
									(El(2, 1) * El(0, 2) * El(1, 3)));
	ret[Vector2i(1, 3)] = invdet * ((El(0, 0) * El(1, 2) * El(2, 3)) + 
									(El(1, 0) * El(2, 2) * El(0, 3)) + 
									(El(2, 0) * El(0, 2) * El(1, 3)) - 
									(El(0, 0) * El(2, 2) * El(1, 3)) - 
									(El(1, 0) * El(0, 2) * El(2, 3)) - 
									(El(2, 0) * El(1, 2) * El(0, 3)));
	ret[Vector2i(2, 3)] = invdet * ((El(0, 0) * El(2, 1) * El(1, 3)) + 
									(El(1, 0) * El(0, 1) * El(2, 3)) + 
									(El(2, 0) * El(1, 1) * El(0, 3)) - 
									(El(0, 0) * El(1, 1) * El(2, 3)) - 
									(El(1, 0) * El(2, 1) * El(0, 3)) - 
									(El(2, 0) * El(0, 1) * El(1, 3)));
	ret[Vector2i(3, 3)] = invdet * ((El(0, 0) * El(1, 1) * El(2, 2)) + 
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

	ret.SetValuesF([&thisM](Vector2i l, float & fOut)
	{
		float f = thisM[Vector2i(l.y, l.x)];
		fOut = f;
	});

	return ret;
}