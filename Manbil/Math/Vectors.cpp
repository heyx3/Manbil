#include "Vectors.h"


#pragma region Vector2f

void Vector2f::Rotate(float radians)
{
	float cosTheta = cosf(radians),
		  sinTheta = sinf(radians);

	x = (x * cosTheta) - (y * sinTheta);
	y = (y * cosTheta) + (x * sinTheta);
}
Vector2f Vector2f::Rotated(float radians)
{
	Vector2f ret(*this);
	ret.Rotate(radians);
	return ret;
}

void Vector2f::Normalize()
{
	float lenInv = 1.0f / Length();

	x *= lenInv;
	y *= lenInv;
}
Vector2f Vector2f::Normalized() const
{
	float lenInv = 1.0f / Length();

	return Vector2f(x * lenInv, y * lenInv);
}

void Vector2f::FastNormalize()
{
	float lenInv = FastInvLength();

	x *= lenInv;
	y *= lenInv;
}
Vector2f Vector2f::FastNormalized() const
{
	float len = Length();

	return Vector2f(x / len, y / len);
}

#pragma endregion

#pragma region Vector3f

void Vector3f::Normalize(void)
{
	const float lenInv = 1.0f / Length();

	x *= lenInv;
	y *= lenInv;
	z *= lenInv;
}
Vector3f Vector3f::Normalized(void) const
{
	const float lenInv = 1.0f / Length();

	return Vector3f(x * lenInv, y * lenInv, z * lenInv);
}

void Vector3f::FastNormalize(void)
{
	const float lenInv = FastInvLength();

	x *= lenInv;
	y *= lenInv;
	z *= lenInv;
}
Vector3f Vector3f::FastNormalized(void) const
{
	const float lenInv = FastInvLength();

	return Vector3f(x * lenInv, y * lenInv, z * lenInv);
}

Vector3f Vector3f::Cross(Vector3f v) const
{
	const float _x = (y * v.z) - (z * v.y);
	const float _y = (z * v.x) - (x * v.z);
	const float _z = (x * v.y) - (y * v.x);

	return Vector3f(_x, _y, _z);
}

#pragma endregion

#pragma region Vector4f

void Vector4f::Normalize(void)
{
	const float lenInv = 1.0f / Length();

	x *= lenInv;
	y *= lenInv;
	z *= lenInv;
	w *= lenInv;
}
Vector4f Vector4f::Normalized(void) const
{
	const float lenInv = 1.0f / Length();

	return Vector4f(x * lenInv, y * lenInv, z * lenInv, w * lenInv);
}

void Vector4f::FastNormalize(void)
{
	const float lenInv = FastInvLength();

	x *= lenInv;
	y *= lenInv;
	z *= lenInv;
	w *= lenInv;
}
Vector4f Vector4f::FastNormalized(void) const
{
	const float lenInv = FastInvLength();

	return Vector4f(x * lenInv, y * lenInv, z * lenInv, w * lenInv);
}

#pragma endregion