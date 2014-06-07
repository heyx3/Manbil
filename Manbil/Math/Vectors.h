#pragma once

#include "BasicMath.h"

namespace MyVectors
{
	#pragma region Byte vectors

	class Vector2b
	{
	public:
		unsigned char x, y;
		Vector2b(unsigned char _x = 0, unsigned char _y = 0) : x(_x), y(_y) { }
		Vector2b(float _x, float _y = 0.0f) : x((unsigned char)(_x * 255.0f)), y((unsigned char)(_y * 255.0f)) { }
	};
	class Vector3b
	{
	public:
		unsigned char x, y, z;
		Vector3b(unsigned char _x = 0, unsigned char _y = 0, unsigned char _z = 0) : x(_x), y(_y), z(_z) { }
		Vector3b(float _x, float _y = 0.0f, float _z = 0.0f) : x((unsigned char)(_x * 255.0f)), y((unsigned char)(_y * 255.0f)), z((unsigned char)(_z * 255.0f)) { }
	};
	class Vector4b
	{
	public:
		unsigned char x, y, z, w;
		Vector4b(unsigned char _x = 0, unsigned char _y = 0, unsigned char _z = 0, unsigned char _w = 0) : x(_x), y(_y), z(_z), w(_w) { }
		Vector4b(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f) : x((unsigned char)(_x * 255.0f)), y((unsigned char)(_y * 255.0f)), z((unsigned char)(_z * 255.0f)), w((unsigned char)(_w * 255.0f)) { }
	};

	#pragma endregion


	#pragma region Vector2i

	class Vector2i
	{
	public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector2i & v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


		int x;
		int y;

		Vector2i(int X = 0, int Y = 0) : x(X), y(Y) { }

        Vector2i(Vector2i&& other)
            : x(0), y(0)
        {
            x = other.x;
            y = other.y;

            other.x = 0;
            other.y = 0;
        }
        Vector2i & operator=(Vector2i&& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;

                other.x = 0;
                other.y = 0;
            }

            return *this;
        }

        
		Vector2i LessX(void) const { return Vector2i(x - 1, y); }
		Vector2i MoreX(void) const { return Vector2i(x + 1, y); }
		Vector2i LessY(void) const { return Vector2i(x, y - 1); }
		Vector2i MoreY(void) const { return Vector2i(x, y + 1); }
		
		Vector2i LessXY(void) const { return Vector2i(x - 1, y - 1); }
		Vector2i MoreXLessY(void) const { return Vector2i(x + 1, y - 1); }
		Vector2i LessXMoreY(void) const { return Vector2i(x - 1, y + 1); }
		Vector2i MoreXY(void) const { return Vector2i(x + 1, y + 1); }

        Vector2i& operator+=(const Vector2i& other) { x += other.x; y += other.y; return *this; }
        Vector2i& operator-=(const Vector2i& other) { x -= other.x; y -= other.y; return *this; }
		Vector2i& operator*=(int i) { x *= i; y *= i; return *this; }
		Vector2i& operator/=(int i) { x /= i; y /= i; return *this; }

        Vector2i operator+(const Vector2i& other) const { return Vector2i(x + other.x, y + other.y); }
        Vector2i operator-(const Vector2i& other) const { return Vector2i(x - other.x, y - other.y); }
		Vector2i operator*(int i) const { return Vector2i(x * i, y * i); }
		Vector2i operator/(int i) const { return Vector2i(x / i, y / i); }
	
        Vector2i operator-(void) const { return Vector2i(-x, -y); }

        const int & operator[](int index) const { return (&x)[index]; }
        int & operator[](int index) { return (&x)[index]; }

        bool operator==(const Vector2i& rhs) const { return Equals(rhs); }
        bool operator!=(const Vector2i& other) const { return x != other.x || y != other.y; }
		bool Equals(Vector2i v) const { return x == v.x && y == v.y; }

        Vector2i Clamp(int min, int max) const
        {
            return Vector2i(BasicMath::Max(min, BasicMath::Min(max, x)),
                            BasicMath::Max(min, BasicMath::Min(max, y)));
        }

        float Length(void) const { return sqrtf((float)LengthSquared()); }
		int LengthSquared(void) const { return (x * x) + (y * y); }

        //Scales this Vector2i's x, and y components by the given Vector2i's x and y components.
        void MultiplyComponents(Vector2i scale) { x *= scale.x; y *= scale.y; }
        //Scales this Vector2i's x, and y components by the given Vector2i's x and y components.
        Vector2i ComponentProduct(Vector2i scale) const { Vector2i v(x, y); v.MultiplyComponents(scale); return v; }

		float Distance(Vector2i other) const { return sqrtf((float)DistanceSquared(other)); }
		int DistanceSquared(Vector2i other) const { int f1 = x - other.x, f2 = y - other.y; return (f1 * f1) + (f2 * f2); }
		int ManhattanDistance(Vector2i other) const { return BasicMath::Abs(x - other.x) + BasicMath::Abs(y - other.y); }

		int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663); }
	};

	#pragma endregion

	#pragma region Vector3i

	class Vector3i
	{
	public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector3i & v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        int x, y, z;

        Vector3i(int _x = 0, int _y = 0, int _z = 0) : x(_x), y(_y), z(_z) { }
		Vector3i(Vector2i copy, int zValue) : x(copy.x), y(copy.y), z(zValue) { }
        Vector3i(const Vector3i & copy) : x(copy.x), y(copy.y), z(copy.z) { }

        Vector3i(Vector3i&& other) : x(0), y(0), z(0)
        {
            x = other.x;
            y = other.y;
            z = other.z;

            other.x = 0;
            other.y = 0;
            other.z = 0;
        }
        Vector3i & operator=(Vector3i&& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;
                z = other.z;

                other.x = 0;
                other.y = 0;
                other.z = 0;
            }

            return *this;
        }


        Vector3i LessX(void) const { return Vector3i(x - 1, y, z); }
        Vector3i LessY(void) const { return Vector3i(x, y - 1, z); }
        Vector3i LessZ(void) const { return Vector3i(x, y, z - 1); }
        Vector3i MoreX(void) const { return Vector3i(x + 1, y, z); }
        Vector3i MoreY(void) const { return Vector3i(x, y + 1, z); }
        Vector3i MoreZ(void) const { return Vector3i(x, y, z + 1); }


        Vector3i& operator+=(const Vector3i& r) { x += r.x; y += r.y; z += r.z; return *this; }
		Vector3i& operator-=(const Vector3i& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }
		Vector3i& operator*=(int f) { x *= f; y *= f; z *= f; return *this; }
		Vector3i& operator/=(int f) { x /= f; y /= f; z /= f; return *this; }

		Vector3i operator+(const Vector3i& other) const { return Vector3i(x + other.x, y + other.y, z + other.z); }
		Vector3i operator-(const Vector3i& other) const { return Vector3i(x - other.x, y - other.y, z - other.z); }
		Vector3i operator*(int scale) const { return Vector3i(x * scale, y * scale, z * scale); }
		Vector3i operator/(int invScale) const { return Vector3i(x / invScale, y / invScale, z / invScale); }
	
        Vector3i operator-(void) const { return Vector3i(-x, -y, -z); }

        const int & operator[](int index) const { return (&x)[index]; }
        int & operator[](int index) { return (&x)[index]; }
	
        bool operator==(const Vector3i& other) const { return Equals(other); }
        bool operator!=(const Vector3i& other) const { return x != other.x || y != other.y || z != other.z; }
		bool Equals(Vector3i v) const { return x == v.x && y == v.y && z == v.z; }

        Vector3i Clamp(int min, int max) const
        {
            return Vector3i(BasicMath::Max(min, BasicMath::Min(max, x)),
                            BasicMath::Max(min, BasicMath::Min(max, y)),
                            BasicMath::Max(min, BasicMath::Min(max, z)));
        }

		int Dot(Vector3i other) const { return (x * other.x) + (y * other.y) + (z * other.z); }
		float AngleBetween(Vector3i other) const { return acosf(Dot(other) / (Length() * other.Length())); }

        //Scales this Vector3i's x, y, and z components by the given Vector3i's x, y, and z components.
        void MultiplyComponents(Vector3i scale) { x *= scale.x; y *= scale.y; z *= scale.z; }
        //Scales this Vector3i's x, y, and z components by the given Vector3i's x, y, and z components.
        Vector3i ComponentProduct(Vector3i scale) const { Vector3i v(x, y, z); v.MultiplyComponents(scale); return v; }

        float Length(void) const { return sqrtf((float)LengthSquared()); }
		int LengthSquared(void) const { return (x * x) + (y * y) + (z * z); }

        float Distance(Vector3i other) const { return sqrtf((float)DistanceSquared(other)); }
		int DistanceSquared(Vector3i other) const { int f1 = x - other.x, f2 = y - other.y, f3 = z - other.z; return (f1 * f2) + (f2 * f2) + (f3 * f3); }

		int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791); }
	};

	#pragma endregion

    #pragma region Vector4i

    class Vector4i
    {
    public:
        
        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector4i & v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        int x;
        int y;
        int z;
        int w;

        Vector4i(int _x = 0, int _y = 0, int _z = 0, int _w = 0) : x(_x), y(_y), z(_z), w(_w) { }
        Vector4i(Vector3i v3, int _w) : x(v3.x), y(v3.y), z(v3.z), w(_w) { }

        Vector4i(Vector4i&& other)
            : x(0), y(0), z(0), w(0)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;

            other.x = 0;
            other.y = 0;
            other.z = 0;
            other.w = 0;
        }
        Vector4i & operator=(Vector4i&& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;
                z = other.z;
                w = other.w;

                other.x = 0;
                other.y = 0;
                other.z = 0;
                other.w = 0;
            }

            return *this;
        }


        Vector4i& operator+=(const Vector4i& r) { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
        Vector4i& operator-=(const Vector4i& r) { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
        Vector4i& operator*=(int f) { x *= f; y *= f; z *= f; w *= f; return *this; }
        Vector4i& operator/=(int f) { x /= f; y /= f; z /= f; w /= f; return *this; }

        Vector4i operator+(const Vector4i& other) const { return Vector4i(x + other.x, y + other.y, z + other.z, w + other.w); }
        Vector4i operator-(const Vector4i& other) const { return Vector4i(x - other.x, y - other.y, z - other.z, w - other.w); }
        Vector4i operator*(int scale) const { return Vector4i(x * scale, y * scale, z * scale, w * scale); }
        Vector4i operator/(int invScale) const { return Vector4i(x / invScale, y / invScale, z / invScale, w / invScale); }

        Vector4i operator-(void) const { return Vector4i(-x, -y, -z, -w); }

        bool operator==(const Vector4i& other) const { return Equals(other); }
        bool operator!=(const Vector4i& other) const { return x != other.x || y != other.y || z != other.z || w != other.w; }
        bool Equals(Vector4i v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }

        const int & operator[](int index) const { return (&x)[index]; }
        int & operator[](int index) { return (&x)[index]; }

        Vector4i Clamp(int min, int max) const
        {
            return Vector4i(BasicMath::Max(min, BasicMath::Min(max, x)),
                            BasicMath::Max(min, BasicMath::Min(max, y)),
                            BasicMath::Max(min, BasicMath::Min(max, z)),
                            BasicMath::Max(min, BasicMath::Min(max, w)));
        }

        int Dot(Vector4i other) const { return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w); }
        float AngleBetween(Vector4i other) const { return acosf(Dot(other) / (Length() * other.Length())); }

        float Length(void) const { return sqrtf((float)LengthSquared()); }
        int LengthSquared(void) const { return (x * x) + (y * y) + (z * z) + (w * w); }
        float FastInvLength(void) const { return BasicMath::FastInvSqrt1((float)LengthSquared()); }

        float Distance(Vector4i other) const { return sqrtf((float)DistanceSquared(other)); }
        int DistanceSquared(Vector4i other) const { int f1 = x - other.x, f2 = y - other.y, f3 = z - other.z, f4 = w - other.w; return (f1 * f1) + (f2 * f2) + (f3 * f3) + (f4 * f4); }
        int ManhattanDistance(Vector4i other) const { return BasicMath::Abs(x - other.x) + BasicMath::Abs(y - other.y) + BasicMath::Abs(z - other.z) + BasicMath::Abs(w - other.w); }
        float FastInvDistance(Vector4i other) const { return BasicMath::FastInvSqrt1((float)DistanceSquared(other)); }

        //Scales this Vector4i's x, y, z, and w components by the given Vector4i's x, y, z, and w components.
        void MultiplyComponents(Vector4i scale) { x *= scale.x; y *= scale.y; z *= scale.z; w *= scale.w; }
        //Scales this Vector4i's x, y, z, and w components by the given Vector4i's x, y, z, and w components.
        Vector4i ComponentProduct(Vector4i scale) const { Vector4i v(x, y, z, w); v.MultiplyComponents(scale); return v; }

        void Normalize(void);
        Vector4i Normalized(void) const;

        void FastNormalize(void);
        Vector4i FastNormalized(void) const;

        int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791) ^ (w * 4256233); }
    };

    #pragma endregion


	#pragma region Vector2f

	class Vector2f
	{
	public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector2f & v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        float x;
		float y;

        Vector2f(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) { }
        Vector2f(Vector2i copy) : x((float)copy.x), y((float)copy.y) { }

        Vector2f(Vector2f&& other)
            : x(0.0f), y(0.0f)
        {
            x = other.x;
            y = other.y;

            other.x = 0.0f;
            other.y = 0.0f;
        }
        Vector2f & operator=(Vector2f&& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;

                other.x = 0.0f;
                other.y = 0.0f;
            }

            return *this;
        }


        Vector2f& operator+=(const Vector2f& other) { x += other.x; y += other.y; return *this; }
		Vector2f& operator-=(const Vector2f& other) { x -= other.x; y -= other.y; return *this; }
		Vector2f& operator*=(float f) { x *= f; y *= f; return *this; }
		Vector2f& operator/=(float f) { x /= f; y /= f; return *this; }

		Vector2f operator+(const Vector2f& other) const { return Vector2f(x + other.x, y + other.y); }
		Vector2f operator-(const Vector2f& other) const { return Vector2f(x - other.x, y - other.y); }
		Vector2f operator*(float f) const { return Vector2f(x * f, y * f); }
		Vector2f operator/(float f) const { return Vector2f(x / f, y / f); }

		Vector2f operator-(void) const { return Vector2f(-x, -y); }
	
		const float & operator[](int index) const { return (&x)[index]; }
		float & operator[](int index) { return (&x)[index]; }

        bool operator==(const Vector2f & other) const { return Equals(other); }
        bool operator!=(const Vector2f& other) const { return x != other.x || y != other.y; }
		bool Equals(Vector2f v) const { return x == v.x && y == v.y; }

        Vector2f Clamp(float min, float max) const
        {
            return Vector2f(BasicMath::Max(min, BasicMath::Min(max, x)),
                            BasicMath::Max(min, BasicMath::Min(max, y)));
        }

        //Casts this vector's components to integers, dropping any fractional values.
        Vector2i CastToInt(void) const { return Vector2i((int)x, (int)y); }
        //Rounds this vector's components to integers.
        Vector2i RoundToInt(void) const { return Vector2i(BasicMath::RoundToInt(x), BasicMath::RoundToInt(y)); }
        //Floors this vector's components (towards negative infinity).
        Vector2i Floored(void) const { return Vector2i((int)floor(x), (int)floor(y)); }
        //'Ceil's this vector's components (towards positive infinity).
        Vector2i Ceil(void) const { return Vector2i((int)ceil(x), (int)ceil(y)); }

		float Dot(Vector2f other) const { return (x * other.x) + (y * other.y); }
		float AngleBetween(Vector2f other) const { return acosf(Dot(other) / (Length() * other.Length())); }

		float Length(void) const { return sqrtf(LengthSquared()); }
		float LengthSquared(void) const { return (x * x) + (y * y); }
		float FastInvLength(void) const { return BasicMath::FastInvSqrt1(LengthSquared()); }

		float Distance(Vector2f other) const { return sqrtf(DistanceSquared(other)); }
		float DistanceSquared(Vector2f other) const { float f1 = x - other.x, f2 = y - other.y; return (f1 * f1) + (f2 * f2); }
		float ManhattanDistance(Vector2f other) const { return BasicMath::Abs(x - other.x) + BasicMath::Abs(y - other.y); }
		float FastInvDistance(Vector2f other) const { return BasicMath::FastInvSqrt1(DistanceSquared(other)); }

        //Scales this Vector2f's x and y components by the given Vector2f's x and y components.
        void MultiplyComponents(Vector2f scale) { x *= scale.x; y *= scale.y; }
        //Scales this Vector2f's x and y components by the given Vector2f's x and y components.
        Vector2f ComponentProduct(Vector2f scale) const { Vector2f v(x, y); v.MultiplyComponents(scale); return v; }

		void Round(int decimals) { x = BasicMath::Round(x, decimals); y = BasicMath::Round(y, decimals); }
		Vector2f Rounded(int decimals) const { return Vector2f(BasicMath::Round(x, decimals), BasicMath::Round(y, decimals)); }

		void Rotate(float radians);
		Vector2f Rotated(float radians);

		void Normalize(void);
		Vector2f Normalized(void) const;

		void FastNormalize(void);
		Vector2f FastNormalized(void) const;
	
		int GetHashCode(void) const { return ((int)(x * 100) * 73856093) ^ ((int)(y * 1000) * 19349663); }
	};

	#pragma endregion

	#pragma region Vector3f

	class Vector3f
	{
	public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector3f & v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        float x;
		float y;
		float z;

        Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) { }
        Vector3f(Vector2f copy, float zValue) : x(copy.x), y(copy.y), z(zValue) { }

        Vector3f(Vector3f&& other)
            : x(0.0f), y(0.0f), z(0.0f)
        {
            x = other.x;
            y = other.y;
            z = other.z;

            other.x = 0.0f;
            other.y = 0.0f;
            other.z = 0.0f;
        }
        Vector3f & operator=(Vector3f&& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;
                z = other.z;

                other.x = 0.0f;
                other.y = 0.0f;
                other.z = 0.0f;
            }

            return *this;
        }


        Vector3f& operator+=(const Vector3f& r) { x += r.x; y += r.y; z += r.z; return *this; }
		Vector3f& operator-=(const Vector3f& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }
		Vector3f& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
		Vector3f& operator/=(float f) { x /= f; y /= f; z /= f; return *this; }

		Vector3f operator+(const Vector3f& other) const { return Vector3f(x + other.x, y + other.y, z + other.z); }
		Vector3f operator-(const Vector3f& other) const { return Vector3f(x - other.x, y - other.y, z - other.z); }
		Vector3f operator*(float scale) const { return Vector3f(x * scale, y * scale, z * scale); }
		Vector3f operator/(float invScale) const { return Vector3f(x / invScale, y / invScale, z / invScale); }
	
		Vector3f operator-(void) const { return Vector3f(-x, -y, -z); }
	
		const float & operator[](int index) const { return (&x)[index]; }
		float & operator[](int index) { return (&x)[index]; }
	
        bool operator==(const Vector3f& other) const { return Equals(other); }
        bool operator!=(const Vector3f& other) const { return x != other.x || y != other.y || z != other.z; }
		bool Equals(Vector3f v) const { return x == v.x && y == v.y && z == v.z; }

        Vector3f Clamp(float min, float max)
        {
            return Vector3f(BasicMath::Max(min, BasicMath::Min(max, x)),
                            BasicMath::Max(min, BasicMath::Min(max, y)),
                            BasicMath::Max(min, BasicMath::Min(max, z)));
        }

        //Casts this vector's components to integers, dropping any fractional values.
        Vector3i CastToInt(void) const { return Vector3i((int)x, (int)y, (int)z); }
        //Rounds this vector's components to integers.
        Vector3i RoundToInt(void) const { return Vector3i(BasicMath::RoundToInt(x), BasicMath::RoundToInt(y), BasicMath::RoundToInt(z)); }
        //Floors this vector's components (towards negative infinity).
        Vector3i Floored(void) const { return Vector3i((int)floor(x), (int)floor(y), (int)floor(z)); }
        //'Ceil's this vector's components (towards positive infinity).
        Vector3i Ceil(void) const { return Vector3i((int)ceil(x), (int)ceil(y), (int)ceil(z)); }

		float Dot(Vector3f other) const { return (x * other.x) + (y * other.y) + (z * other.z); }
		float AngleBetween(Vector3f other) const { return acosf(Dot(other) / (Length() * other.Length())); }

		float Length(void) const { return sqrtf(LengthSquared()); }
		float LengthSquared(void) const { return (x * x) + (y * y) + (z * z); }
		float FastInvLength(void) const { return BasicMath::FastInvSqrt1(LengthSquared()); }

		float Distance(Vector3f other) const { return sqrtf(DistanceSquared(other)); }
		float DistanceSquared(Vector3f other) const { float f1 = (x - other.x), f2 = (y - other.y), f3 = z - other.z; return (f1 * f1) + (f2 * f2) + (f3 * f3); }
		float ManhattanDistance(Vector3f other) const { return BasicMath::Abs(x - other.x) + BasicMath::Abs(y - other.y) + BasicMath::Abs(z - other.z); }
		float FastInvDistance(Vector3f other) const { return BasicMath::FastInvSqrt1(DistanceSquared(other)); }

		void Round(int decimals) { x = BasicMath::Round(x, decimals); y = BasicMath::Round(y, decimals); z = BasicMath::Round(z, decimals); }
		Vector3f Rounded(int decimals) const { return Vector3f(BasicMath::Round(x, decimals), BasicMath::Round(y, decimals), BasicMath::Round(z, decimals)); }

		//Scales this Vector3f's x, y, and z components by the given Vector3f's x, y, and z components.
		void MultiplyComponents(Vector3f scale) { x *= scale.x; y *= scale.y; z *= scale.z; }
		//Scales this Vector3f's x, y, and z components by the given Vector3f's x, y, and z components.
		Vector3f ComponentProduct(Vector3f scale) const { Vector3f v(x, y, z); v.MultiplyComponents(scale); return v; }

		void Normalize(void);
		Vector3f Normalized(void) const;

		void FastNormalize(void);
		Vector3f FastNormalized(void) const;

		Vector3f Cross(Vector3f v) const;

		Vector3f Reflect(Vector3f normal) const { return (*this) + (normal * 2.0 * (-normal).Dot(*this)); }

		int GetHashCode(void) const { return ((int)(x * 100) * 73856093) ^ ((int)(y * 1000) * 19349663) ^ ((int)(z * 10) * 83492791); }
	};

	#pragma endregion

	#pragma region Vector4f

	class Vector4f
	{
	public:
		
        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector4f & v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        float x;
		float y;
		float z;
		float w;

        Vector4f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f) : x(_x), y(_y), z(_z), w(_w) { }
        Vector4f(Vector3f v3, float _w) : x(v3.x), y(v3.y), z(v3.z), w(_w) { }

        Vector4f(Vector4f&& other)
            : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;

            other.x = 0.0f;
            other.y = 0.0f;
            other.z = 0.0f;
            other.w = 0.0f;
        }
        Vector4f & operator=(Vector4f&& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;
                z = other.z;
                w = other.w;

                other.x = 0.0f;
                other.y = 0.0f;
                other.z = 0.0f;
                other.w = 0.0f;
            }

            return *this;
        }


        Vector4f& operator+=(const Vector4f& r) { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
		Vector4f& operator-=(const Vector4f& r) { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
		Vector4f& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
		Vector4f& operator/=(float f) { x /= f; y /= f; z /= f; w /= f; return *this; }

		Vector4f operator+(const Vector4f& other) const { return Vector4f(x + other.x, y + other.y, z + other.z, w + other.w); }
		Vector4f operator-(const Vector4f& other) const { return Vector4f(x - other.x, y - other.y, z - other.z, w - other.w); }
		Vector4f operator*(float scale) const { return Vector4f(x * scale, y * scale, z * scale, w * scale); }
		Vector4f operator/(float invScale) const { return Vector4f(x / invScale, y / invScale, z / invScale, w / invScale); }
	
		Vector4f operator-(void) const { return Vector4f(-x, -y, -z, -w); }
	
        bool operator==(const Vector4f& other) const { return Equals(other); }
        bool operator!=(const Vector4f& other) const { return x != other.x || y != other.y || z != other.z || w != other.w; }
		bool Equals(Vector4f v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }

		const float & operator[](int index) const { return (&x)[index]; }
		float & operator[](int index) { return (&x)[index]; }

        Vector4f Clamp(float min, float max)
        {
            return Vector4f(BasicMath::Max(min, BasicMath::Min(max, x)),
                            BasicMath::Max(min, BasicMath::Min(max, y)),
                            BasicMath::Max(min, BasicMath::Min(max, z)), 
                            BasicMath::Max(min, BasicMath::Min(max, w)));
        }

        //Casts this vector's components to integers, dropping any fractional values.
        Vector4i CastToInt(void) const { return Vector4i((int)x, (int)y, (int)z, (int)w); }
        //Rounds this vector's components to integers.
        Vector4i RoundToInt(void) const { return Vector4i(BasicMath::RoundToInt(x), BasicMath::RoundToInt(y), BasicMath::RoundToInt(z), BasicMath::RoundToInt(w)); }
        //Floors this vector's components (towards negative infinity).
        Vector4i Floored(void) const { return Vector4i((int)floor(x), (int)floor(y), (int)floor(z), (int)floor(w)); }
        //'Ceil's this vector's components (towards positive infinity).
        Vector4i Ceil(void) const { return Vector4i((int)ceil(x), (int)ceil(y), (int)ceil(z), (int)ceil(w)); }

		float Dot(Vector4f other) const { return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w); }
		float AngleBetween(Vector4f other) const { return acosf(Dot(other) / (Length() * other.Length())); }

		float Length(void) const { return sqrtf(LengthSquared()); }
		float LengthSquared(void) const { return (x * x) + (y * y) + (z * z) + (w * w); }
		float FastInvLength(void) const { return BasicMath::FastInvSqrt1(LengthSquared()); }

		float Distance(Vector4f other) const { return sqrtf(DistanceSquared(other)); }
		float DistanceSquared(Vector4f other) const { float f1 = x - other.x, f2 = y - other.y, f3 = z - other.z, f4 = w - other.w; return (f1 * f1) + (f2 * f2) + (f3 * f3) + (f4 * f4); }
		float ManhattanDistance(Vector4f other) const { return BasicMath::Abs(x - other.x) + BasicMath::Abs(y - other.y) + BasicMath::Abs(z - other.z) + BasicMath::Abs(w - other.w); }
		float FastInvDistance(Vector4f other) const { return BasicMath::FastInvSqrt1(DistanceSquared(other)); }

        //Scales this Vector4f's x, y, z, and w components by the given Vector4f's x, y, z, and w components.
        void MultiplyComponents(Vector4f scale) { x *= scale.x; y *= scale.y; z *= scale.z; w *= scale.w; }
        //Scales this Vector4f's x, y, z, and w components by the given Vector4f's x, y, z, and w components.
        Vector4f ComponentProduct(Vector4f scale) const { Vector4f v(x, y, z, w); v.MultiplyComponents(scale); return v; }

		void Round(int decimals) { x = BasicMath::Round(x, decimals); y = BasicMath::Round(y, decimals); z = BasicMath::Round(z, decimals); w = BasicMath::Round(w, decimals); }
		Vector4f Rounded(int decimals) const { return Vector4f(BasicMath::Round(x, decimals), BasicMath::Round(y, decimals), BasicMath::Round(z, decimals), BasicMath::Round(w, decimals)); }

		void Normalize(void);
		Vector4f Normalized(void) const;

		void FastNormalize(void);
		Vector4f FastNormalized(void) const;

		int GetHashCode(void) const { return ((int)(x * 100) * 73856093) ^ ((int)(y * 1000) * 19349663) ^ ((int)(z * 10) * 83492791) ^ ((int)(w * 10000) * 4256233); }
	};

	#pragma endregion
}

using namespace MyVectors;