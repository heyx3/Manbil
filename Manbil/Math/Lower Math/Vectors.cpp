#include "Vectors.h"


#pragma region Vector2b

Vector2b& Vector2b::operator+=(const Vector2b& other)
{
    if (255 - x < other.x)
        x = 255;
    else x += other.x;
    if (255 - y < other.y)
        y = 255;
    else y += other.y;
    return *this;
}
Vector2b& Vector2b::operator-=(const Vector2b& other)
{
    if (x < other.x)
        x = 0;
    else x -= other.x;
    if (y < other.y)
        y = 0;
    else y -= other.y;
    return *this;
}

#pragma endregion

#pragma region Vector3b

Vector3b& Vector3b::operator+=(const Vector3b& other)
{
    if (255 - x < other.x)
        x = 255;
    else x += other.x;
    if (255 - y < other.y)
        y = 255;
    else y += other.y;
    if (255 - z < other.z)
        z = 255;
    else z += other.z;
    return *this;
}
Vector3b& Vector3b::operator-=(const Vector3b& other)
{
    if (x < other.x)
        x = 0;
    else x -= other.x;
    if (y < other.y)
        y = 0;
    else y -= other.y;
    if (z < other.z)
        z = 0;
    else z -= other.z;
    return *this;
}

#pragma endregion

#pragma region Vector4b

Vector4b& Vector4b::operator +=(const Vector4b& other)
{
    if (255 - x < other.x)
        x = 255;
    else x += other.x;
    if (255 - y < other.y)
        y = 255;
    else y += other.y;
    if (255 - z < other.z)
        z = 255;
    else z += other.z;
    if (255 - w < other.w)
        w = 255;
    else w += other.w;
    return *this;
}
Vector4b& Vector4b::operator -=(const Vector4b& other)
{
    if (x < other.x)
        x = 0;
    else x -= other.x;
    if (y < other.y)
        y = 0;
    else y -= other.y;
    if (z < other.z)
        z = 0;
    else z -= other.z;
    if (w < other.w)
        w = 0;
    else w -= other.w;
    return *this;
}

#pragma endregion


#pragma region Vector2u

Vector2u Vector2u::Clamp(unsigned int min, unsigned int max) const
{
    return Vector2u(BasicMath::Clamp(x, min, max),
                    BasicMath::Clamp(y, min, max));
}
Vector2u Vector2u::Clamp(Vector2u min, Vector2u max) const
{
    return Vector2u(BasicMath::Clamp(x, min.x, max.x),
                    BasicMath::Clamp(y, min.y, max.y));
}

unsigned int Vector2u::DistanceSquared(Vector2u other) const
{
    unsigned int f1 = x - other.x,
                 f2 = y - other.y;
    return (f1 * f1) + (f2 * f2);
}
unsigned int Vector2u::ManhattanDistance(Vector2u other) const
{
    unsigned int dist = 0;

    if (x < other.x)
        dist += (other.x - x);
    else dist += (x - other.x);

    if (y < other.y)
        dist += (other.y - y);
    else dist += (y - other.y);

    return dist;
}

#pragma endregion

#pragma region Vector3u

Vector3u Vector3u::Clamp(unsigned int min, unsigned int max) const
{
    return Vector3u(BasicMath::Clamp(x, min, max),
                    BasicMath::Clamp(y, min, max),
                    BasicMath::Clamp(z, min, max));
}
Vector3u Vector3u::Clamp(Vector3u min, Vector3u max) const
{
    return Vector3u(BasicMath::Clamp(x, min.x, max.x),
                    BasicMath::Clamp(y, min.y, max.y),
                    BasicMath::Clamp(z, min.z, max.z));
}

unsigned int Vector3u::DistanceSquared(Vector3u other) const
{
    unsigned int f1 = x - other.x,
                 f2 = y - other.y,
                 f3 = z - other.z;
    return (f1 * f2) + (f2 * f2) + (f3 * f3);
}
unsigned int Vector3u::ManhattanDistance(Vector3u other) const
{
    unsigned int dist = 0;

    if (x < other.x)
        dist += (other.x - x);
    else dist += (x - other.x);

    if (y < other.y)
        dist += (other.y - y);
    else dist += (y - other.y);

    if (z < other.z)
        dist += (other.z - z);
    else dist += (z - other.z);

    return dist;
}

#pragma endregion

#pragma region Vector4u

Vector4u Vector4u::Clamp(unsigned int min, unsigned int max) const
{
    return Vector4u(BasicMath::Clamp(x, min, max),
                    BasicMath::Clamp(y, min, max),
                    BasicMath::Clamp(z, min, max),
                    BasicMath::Clamp(w, min, max));
}
Vector4u Vector4u::Clamp(Vector4u min, Vector4u max) const
{
    return Vector4u(BasicMath::Clamp(x, min.x, max.x),
                    BasicMath::Clamp(y, min.y, max.y),
                    BasicMath::Clamp(z, min.z, max.z),
                    BasicMath::Clamp(w, min.w, max.w));
}

unsigned int Vector4u::DistanceSquared(const Vector4u& other) const
{
    unsigned int f1 = x - other.x,
                 f2 = y - other.y,
                 f3 = z - other.z,
                 f4 = w - other.w;
    return (f1 * f1) + (f2 * f2) + (f3 * f3) + (f4 * f4);
}
unsigned int Vector4u::ManhattanDistance(const Vector4u& other) const
{
    unsigned int dist = 0;

    if (x < other.x)
        dist += (other.x - x);
    else dist += (x - other.x);

    if (y < other.y)
        dist += (other.y - y);
    else dist += (y - other.y);

    if (z < other.z)
        dist += (other.z - z);
    else dist += (z - other.z);

    if (w < other.w)
        dist += (other.w - w);
    else dist += (w - other.w);

    return dist;
}

#pragma endregion


#pragma region Vector2f

void Vector2f::Rotate(float radians)
{
	float cosTheta = cosf(radians),
		  sinTheta = sinf(radians);

    float oldX = x;
	x = (oldX * cosTheta) - (y * sinTheta);
	y = (y * cosTheta) + (oldX * sinTheta);
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