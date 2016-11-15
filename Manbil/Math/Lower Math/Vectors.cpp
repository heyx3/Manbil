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
    return Vector2u(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max));
}
Vector2u Vector2u::Clamp(Vector2u min, Vector2u max) const
{
    return Vector2u(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y));
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

    //These are unsigned ints, so we have to be careful to prevent going below 0.
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
    return Vector3u(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max),
                    Mathf::Clamp(z, min, max));
}
Vector3u Vector3u::Clamp(Vector3u min, Vector3u max) const
{
    return Vector3u(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y),
                    Mathf::Clamp(z, min.z, max.z));
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
    
    //These are unsigned ints, so we have to be careful to prevent going below 0.
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
    return Vector4u(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max),
                    Mathf::Clamp(z, min, max),
                    Mathf::Clamp(w, min, max));
}
Vector4u Vector4u::Clamp(Vector4u min, Vector4u max) const
{
    return Vector4u(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y),
                    Mathf::Clamp(z, min.z, max.z),
                    Mathf::Clamp(w, min.w, max.w));
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
    
    //These are unsigned ints, so we have to be careful to prevent going below 0.

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


#pragma region Vector2i

Vector2i Vector2i::Clamp(int min, int max) const
{
    return Vector2i(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max));
}
Vector2i Vector2i::Clamp(Vector2i min, Vector2i max) const
{
    return Vector2i(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y));
}

int Vector2i::DistanceSquared(Vector2i other) const
{
    int f1 = x - other.x,
        f2 = y - other.y;
    return (f1 * f1) + (f2 * f2);
}

int Vector2i::ManhattanDistance(Vector2i other) const
{
    return Mathf::Abs(x - other.x) +
           Mathf::Abs(y - other.y);
}

#pragma endregion

#pragma region Vector3i

Vector3i Vector3i::Clamp(int min, int max) const
{
    return Vector3i(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max),
                    Mathf::Clamp(z, min, max));
}
Vector3i Vector3i::Clamp(Vector3i min, Vector3i max) const
{
    return Vector3i(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y),
                    Mathf::Clamp(z, min.z, max.z));
}

int Vector3i::DistanceSquared(Vector3i other) const
{
    int f1 = x - other.x,
        f2 = y - other.y,
        f3 = z - other.z;
    return (f1 * f1) + (f2 * f2) + (f3 * f3);
}

int Vector3i::ManhattanDistance(Vector3i other) const
{
    return Mathf::Abs(x - other.x) +
           Mathf::Abs(y - other.y) +
           Mathf::Abs(z - other.z);
}

#pragma endregion

#pragma region Vector4i

Vector4i Vector4i::Clamp(int min, int max) const
{
    return Vector4i(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max),
                    Mathf::Clamp(z, min, max),
                    Mathf::Clamp(w, min, max));
}
Vector4i Vector4i::Clamp(Vector4i min, Vector4i max) const
{
    return Vector4i(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y),
                    Mathf::Clamp(z, min.z, max.z),
                    Mathf::Clamp(w, min.w, max.w));
}

int Vector4i::DistanceSquared(Vector4i other) const
{
    int f1 = x - other.x,
        f2 = y - other.y,
        f3 = z - other.z,
        f4 = w - other.w;
    return (f1 * f1) + (f2 * f2) + (f3 * f3) + (f4 * f4);
}

int Vector4i::ManhattanDistance(Vector4i other) const
{
    return Mathf::Abs(x - other.x) +
           Mathf::Abs(y - other.y) +
           Mathf::Abs(z - other.z) +
           Mathf::Abs(w - other.w);
}

#pragma endregion


#pragma region Vector2f

Vector2f Vector2f::Lerp(Vector2f start, Vector2f end, float t)
{
    return Vector2f(Mathf::Lerp(start.x, end.x, t),
                    Mathf::Lerp(start.y, end.y, t));
}Vector2f Vector2f::Lerp(Vector2f start, Vector2f end, Vector2f t)
{
    return Vector2f(Mathf::Lerp(start.x, end.x, t.x),
                    Mathf::Lerp(start.y, end.y, t.y));
}

void Vector2f::Rotate(float radians)
{
	float cosTheta = cosf(radians),
		  sinTheta = sinf(radians);

    float oldX = x;
	x = (oldX * cosTheta) - (y * sinTheta);
	y = (y * cosTheta) + (oldX * sinTheta);
}
Vector2f Vector2f::Rotated(float radians) const
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
    float lenInv = FastInvLength();
	return Vector2f(x * lenInv, y * lenInv);
}

Vector2f Vector2f::Clamp(float min, float max) const
{
    return Vector2f(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max));
}
Vector2f Vector2f::Clamp(Vector2f min, Vector2f max) const
{
    return Vector2f(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y));
}

float Vector2f::DistanceSquared(Vector2f other) const
{
    float f1 = x - other.x,
          f2 = y - other.y;
    return (f1 * f1) + (f2 * f2);
}
float Vector2f::ManhattanDistance(Vector2f other) const
{
    return Mathf::Abs(x - other.x) +
           Mathf::Abs(y - other.y);
}

int Vector2f::GetHashCode(void) const
{
    static_assert(sizeof(int) == sizeof(float),
                  "'int' and 'float' must be the same size for this hash code to work!");
    return Vector2i(*(int*)(&x), *(int*)(&y)).GetHashCode();
}

#pragma endregion

#pragma region Vector3f

Vector3f Vector3f::Lerp(Vector3f start, Vector3f end, float t)
{
    return Vector3f(Mathf::Lerp(start.x, end.x, t),
                    Mathf::Lerp(start.y, end.y, t),
                    Mathf::Lerp(start.z, end.z, t));
}Vector3f Vector3f::Lerp(Vector3f start, Vector3f end, Vector3f t)
{
    return Vector3f(Mathf::Lerp(start.x, end.x, t.x),
                    Mathf::Lerp(start.y, end.y, t.y),
                    Mathf::Lerp(start.z, end.z, t.z));
}

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
	float lenInv = FastInvLength();

	x *= lenInv;
	y *= lenInv;
	z *= lenInv;
}
Vector3f Vector3f::FastNormalized(void) const
{
    float lenInv = FastInvLength();

	return Vector3f(x * lenInv, y * lenInv, z * lenInv);
}

Vector3f Vector3f::Cross(Vector3f v) const
{
	return Vector3f((y * v.z) - (z * v.y),
                    (z * v.x) - (x * v.z),
                    (x * v.y) - (y * v.x));
}

Vector3f Vector3f::Clamp(float min, float max) const
{
    return Vector3f(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max),
                    Mathf::Clamp(z, min, max));
}
Vector3f Vector3f::Clamp(Vector3f min, Vector3f max) const
{
    return Vector3f(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y),
                    Mathf::Clamp(z, min.z, max.z));
}

float Vector3f::DistanceSquared(Vector3f other) const
{
    float f1 = x - other.x,
          f2 = y - other.y,
          f3 = z - other.z;
    return (f1 * f1) + (f2 * f2) + (f3 * f3);
}
float Vector3f::ManhattanDistance(Vector3f other) const
{
    return Mathf::Abs(x - other.x) +
           Mathf::Abs(y - other.y) +
           Mathf::Abs(z - other.z);
}

int Vector3f::GetHashCode(void) const
{
    static_assert(sizeof(int) == sizeof(float),
                  "'int' and 'float' must be the same size for this hash code to work!");
    return Vector3i(*(int*)(&x), *(int*)(&y), *(int*)(&z)).GetHashCode();
}

#pragma endregion

#pragma region Vector4f

Vector4f Vector4f::Lerp(Vector4f start, Vector4f end, float t)
{
    return Vector4f(Mathf::Lerp(start.x, end.x, t),
                    Mathf::Lerp(start.y, end.y, t),
                    Mathf::Lerp(start.z, end.z, t),
                    Mathf::Lerp(start.w, end.w, t));
}Vector4f Vector4f::Lerp(Vector4f start, Vector4f end, Vector4f t)
{
    return Vector4f(Mathf::Lerp(start.x, end.x, t.x),
                    Mathf::Lerp(start.y, end.y, t.y),
                    Mathf::Lerp(start.z, end.z, t.z),
                    Mathf::Lerp(start.w, end.w, t.w));
}
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

Vector4f Vector4f::Clamp(float min, float max) const
{
    return Vector4f(Mathf::Clamp(x, min, max),
                    Mathf::Clamp(y, min, max),
                    Mathf::Clamp(z, min, max),
                    Mathf::Clamp(w, min, max));
}
Vector4f Vector4f::Clamp(Vector4f min, Vector4f max) const
{
    return Vector4f(Mathf::Clamp(x, min.x, max.x),
                    Mathf::Clamp(y, min.y, max.y),
                    Mathf::Clamp(z, min.z, max.z),
                    Mathf::Clamp(w, min.w, max.w));
}

float Vector4f::DistanceSquared(Vector4f other) const
{
    float f1 = x - other.x,
          f2 = y - other.y,
          f3 = z - other.z,
          f4 = w - other.w;
    return (f1 * f1) + (f2 * f2) + (f3 * f3) + (f4 * f4);
}
float Vector4f::ManhattanDistance(Vector4f other) const
{
    return Mathf::Abs(x - other.x) +
           Mathf::Abs(y - other.y) +
           Mathf::Abs(z - other.z) +
           Mathf::Abs(w - other.w);
}

int Vector4f::GetHashCode(void) const
{
    static_assert(sizeof(int) == sizeof(float),
                  "'int' and 'float' must be the same size for this hash code to work!");
    return Vector4i(*(int*)(&x), *(int*)(&y), *(int*)(&z), *(int*)(&w)).GetHashCode();
}

#pragma endregion