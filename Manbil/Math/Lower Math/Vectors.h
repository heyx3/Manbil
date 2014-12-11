#pragma once

#include "BasicMath.h"


//Define these vectors in a namespace because SFML uses some of the same names for its vectors.
namespace ManbilVectors
{
	#pragma region Vector2b, Vector3b, Vector4b

    //Unsigned bytes. All operators clamp to 0-255 instead of overflowing.
	class Vector2b
	{
	public:
		unsigned char x, y;
        Vector2b(void) : x(0), y(0) { }
		Vector2b(unsigned char _x, unsigned char _y) : x(_x), y(_y) { }
        Vector2b(float _x, float _y)
            : x((unsigned char)(_x * 255.0f)), y((unsigned char)(_y * 255.0f)) { }
        //These byte vectors are generally used to represent colors,
        //   so all operators will clamp instead of wrapping around.
        Vector2b& operator+=(const Vector2b& other);
        //These byte vectors are generally used to represent colors,
        //   so all operators will clamp instead of wrapping around.
        Vector2b& operator-=(const Vector2b& other);
    };

    //Unsigned bytes. All operators clamp to 0-255 instead of overflowing.
	class Vector3b
	{
	public:
        unsigned char x, y, z;
        Vector3b(void) : x(0), y(0), z(0) { }
		Vector3b(unsigned char _x, unsigned char _y, unsigned char _z)
            : x(_x), y(_y), z(_z) { }
        Vector3b(float _x, float _y = 0.0f, float _z = 0.0f)
            : x((unsigned char)(_x * 255.0f)),
              y((unsigned char)(_y * 255.0f)),
              z((unsigned char)(_z * 255.0f)) { }
        //These byte vectors are generally used to represent colors,
        //   so all operators will clamp instead of wrapping around.
        Vector3b& operator+=(const Vector3b& other);
        //These byte vectors are generally used to represent colors,
        //   so all operators will clamp instead of wrapping around.
        Vector3b& operator-=(const Vector3b& other);
    };

    //Unsigned bytes. All operators clamp to 0-255 instead of overflowing.
	class Vector4b
	{
	public:
        unsigned char x, y, z, w;
        Vector4b(void) : x(0), y(0), z(0), w(0) { }
		Vector4b(unsigned char _x, unsigned char _y, unsigned char _z, unsigned char _w)
            : x(_x), y(_y), z(_z), w(_w) { }
        Vector4b(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
            : x((unsigned char)(_x * 255.0f)), y((unsigned char)(_y * 255.0f)),
              z((unsigned char)(_z * 255.0f)), w((unsigned char)(_w * 255.0f)) { }
        //These byte vectors are generally used to represent colors,
        //   so all operators will clamp instead of wrapping around.
        Vector4b& operator+=(const Vector4b& other);
        //These byte vectors are generally used to represent colors,
        //   so all operators will clamp instead of wrapping around.
        Vector4b& operator-=(const Vector4b& other);
	};

	#pragma endregion


    #pragma region Vector2u

    class Vector2u
    {
    public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector2u& v) const { return v.GetHashCode(); }


        unsigned int x;
        unsigned int y;

        Vector2u(void) : Vector2u(0, 0) { }
        Vector2u(unsigned int X, unsigned int Y) : x(X), y(Y) { }
        Vector2u(const Vector2u& other) { x = other.x; y = other.y; }

        Vector2u& operator=(const Vector2u& other) { x = other.x; y = other.y; return *this; }


        //Access the X or Y component with 0 or 1, respectively.
        const unsigned int& operator[](unsigned int index) const { return (&x)[index]; }
        //Access the X or Y component with 0 or 1, respectively.
        unsigned int& operator[](unsigned int index) { return (&x)[index]; }


        Vector2u& operator+=(const Vector2u& other) { x += other.x; y += other.y; return *this; }
        Vector2u& operator-=(const Vector2u& other) { x -= other.x; y -= other.y; return *this; }
        Vector2u& operator*=(unsigned int i) { x *= i; y *= i; return *this; }
        Vector2u& operator/=(unsigned int i) { x /= i; y /= i; return *this; }

        Vector2u operator+(const Vector2u& other) const { return Vector2u(x + other.x, y + other.y); }
        Vector2u operator-(const Vector2u& other) const { return Vector2u(x - other.x, y - other.y); }
        Vector2u operator*(unsigned int i) const { return Vector2u(x * i, y * i); }
        Vector2u operator/(unsigned int i) const { return Vector2u(x / i, y / i); }


        bool operator==(const Vector2u& v) const { return (x == v.x) && (y == v.y); }
        bool operator!=(const Vector2u& v) const { return (x != v.x) || (y != v.y); }


        Vector2u LessX(void) const { assert(x > 0); return Vector2u(x - 1, y); }
        Vector2u MoreX(void) const { return Vector2u(x + 1, y); }
        Vector2u LessY(void) const { assert(y > 0); return Vector2u(x, y - 1); }
        Vector2u MoreY(void) const { return Vector2u(x, y + 1); }


        //Scales this Vector2u's x, and y components by the given Vector2u's x and y components.
        void MultiplyComponents(Vector2u scale) { x *= scale.x; y *= scale.y; }
        //Scales this Vector2u's x, and y components by the given Vector2u's x and y components.
        Vector2u ComponentProduct(Vector2u scale) const { return Vector2u(x * scale.x, y * scale.y); }

        //Clamps each component of this vector to be inside the given range of values.
        Vector2u Clamp(unsigned int min, unsigned int max) const;
        //Clamps each component of this vector to be inside the given range of values.
        Vector2u Clamp(Vector2u min, Vector2u max) const;


        //Computes the dot product of this vector and another one.
        unsigned int Dot(Vector2u& v) const { return (x * v.x) + (y * v.y); }


        unsigned int LengthSquared(void) const { return (x * x) + (y * y); }
        float Length(void) const { return sqrtf((float)LengthSquared()); }


        unsigned int DistanceSquared(Vector2u other) const;
        float Distance(Vector2u other) const { return sqrtf((float)DistanceSquared(other)); }
        unsigned int ManhattanDistance(Vector2u other) const;


        unsigned int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663); }
    };

    #pragma endregion

    #pragma region Vector3u

    class Vector3u
    {
    public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector3u& v) const { return v.GetHashCode(); }


        unsigned int x, y, z;


        Vector3u(void) : Vector3u(0, 0, 0) { }
        Vector3u(unsigned int _x, unsigned int _y, unsigned int _z) : x(_x), y(_y), z(_z) { }
        Vector3u(Vector2u copy, unsigned int zValue) : x(copy.x), y(copy.y), z(zValue) { }
        Vector3u(const Vector3u& copy) : x(copy.x), y(copy.y), z(copy.z) { }

        inline Vector3u& operator=(const Vector3u& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }


        //Access the X, Y, or Z component with 0, 1, or 2, respectively.
        const unsigned int& operator[](unsigned int index) const { return (&x)[index]; }
        //Access the X, Y, or Z component with 0, 1, or 2, respectively.
        unsigned int& operator[](unsigned int index) { return (&x)[index]; }


        Vector3u& operator+=(const Vector3u& r) { x += r.x; y += r.y; z += r.z; return *this; }
        Vector3u& operator-=(const Vector3u& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }
        Vector3u& operator*=(unsigned int f) { x *= f; y *= f; z *= f; return *this; }
        Vector3u& operator/=(unsigned int f) { x /= f; y /= f; z /= f; return *this; }

        Vector3u operator+(const Vector3u& v) const { return Vector3u(x + v.x, y + v.y, z + v.z); }
        Vector3u operator-(const Vector3u& v) const { return Vector3u(x - v.x, y - v.y, z - v.z); }
        Vector3u operator*(unsigned int scale) const { return Vector3u(x + scale, y + scale, z + scale); }
        Vector3u operator/(unsigned int val) const { return Vector3u(x / val, y / val, z / val); }


        bool operator==(Vector3u v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
        bool operator!=(Vector3u v) const { return (x != v.x) || (y != v.y) || (z != v.z); }


        Vector3u LessX(void) const { assert(x > 0); return Vector3u(x - 1, y, z); }
        Vector3u LessY(void) const { assert(y > 0); return Vector3u(x, y - 1, z); }
        Vector3u LessZ(void) const { assert(z > 0); return Vector3u(x, y, z - 1); }
        Vector3u MoreX(void) const { return Vector3u(x + 1, y, z); }
        Vector3u MoreY(void) const { return Vector3u(x, y + 1, z); }
        Vector3u MoreZ(void) const { return Vector3u(x, y, z + 1); }


        //Clamps each component of this vector to be inside the given range of values.
        Vector3u Clamp(unsigned int min, unsigned int max) const;
        //Clamps each component of this vector to be inside the given range of values.
        Vector3u Clamp(Vector3u min, Vector3u max) const;

        
        //Computes the dot product of this vector and another one.
        unsigned int Dot(Vector3u v) const { return (x * v.x) + (y * v.y) + (z * v.z); }


        //Scales this Vector3u's x, y, and z components by the given Vector3u's x, y, and z components.
        void MultiplyComponents(Vector3u scale) { x *= scale.x; y *= scale.y; z *= scale.z; }
        //Scales this Vector3u's x, y, and z components by the given Vector3u's x, y, and z components.
        Vector3u ComponentProduct(Vector3u scle) const { return Vector3u(x * scle.x, y * scle.y, z * scle.z); }


        unsigned int LengthSquared(void) const { return (x * x) + (y * y) + (z * z); }
        float Length(void) const { return sqrtf((float)LengthSquared()); }


        unsigned int DistanceSquared(Vector3u other) const;
        float Distance(Vector3u other) const { return sqrtf((float)DistanceSquared(other)); }
        unsigned int ManhattanDistance(Vector3u other) const;


        unsigned int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791); }
    };

    #pragma endregion

    #pragma region Vector4u

    class Vector4u
    {
    public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector4u& v) const { return v.GetHashCode(); }


        unsigned int x;
        unsigned int y;
        unsigned int z;
        unsigned int w;


        Vector4u(void) : Vector4u(0, 0, 0, 0) { }
        Vector4u(unsigned int _x, unsigned int _y, unsigned int _z, unsigned int _w)
            : x(_x), y(_y), z(_z), w(_w) { }
        Vector4u(Vector3u v3, unsigned int _w) : x(v3.x), y(v3.y), z(v3.z), w(_w) { }
        Vector4u(const Vector4u& cpy) { x = cpy.x; y = cpy.y; z = cpy.z; w = cpy.w; }

        Vector4u& operator=(Vector4u& cpy) { x = cpy.x; y = cpy.y; z = cpy.z; w = cpy.w; }


        //Access the X, Y, Z, or W component with 0, 1, 2, or 3, respectively.
        const unsigned int& operator[](unsigned int index) const { return (&x)[index]; }
        //Access the X, Y, Z, or W component with 0, 1, 2, or 3, respectively.
        unsigned int& operator[](unsigned int index) { return (&x)[index]; }


        Vector4u& operator+=(const Vector4u& r) { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
        Vector4u& operator-=(const Vector4u& r) { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
        Vector4u& operator*=(unsigned int f) { x *= f; y *= f; z *= f; w *= f; return *this; }
        Vector4u& operator/=(unsigned int f) { x /= f; y /= f; z /= f; w /= f; return *this; }

        Vector4u operator+(const Vector4u& v) const { return Vector4u(x + v.x, y + v.y, z + v.z, w + v.w); }
        Vector4u operator-(const Vector4u& v) const { return Vector4u(x - v.x, y - v.y, z - v.z, w - v.w); }
        Vector4u operator*(unsigned int scle) const { return Vector4u(x * scle, y * scle, z * scle, w * scle); }
        Vector4u operator/(unsigned int denom) const { return Vector4u(x / denom, y / denom, z / denom, w / denom); }


        bool operator==(const Vector4u& v) const { return (x == v.x) || (y == v.y) || (z == v.z) || (w == v.w); }
        bool operator!=(const Vector4u& v) const { return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w); }


        //Scales this Vector4u's components by the given Vector4u's components.
        void MultiplyComponents(Vector4u scale) { x *= scale.x; y *= scale.y; z *= scale.z; w *= scale.w; }
        //Scales this Vector4u's components by the given Vector4u's components.
        inline Vector4u ComponentProduct(Vector4u scale) const
        {
            return Vector4u(x * scale.x, y * scale.y, z * scale.z, w * scale.w);
        }


        //Clamps each component of this vector to be inside the given range of values.
        Vector4u Clamp(unsigned int min, unsigned int max) const;
        //Clamps each component of this vector to be inside the given range of values.
        Vector4u Clamp(Vector4u min, Vector4u max) const;


        unsigned int Dot(Vector4u v) const { return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w); }


        unsigned int LengthSquared(void) const { return (x * x) + (y * y) + (z * z) + (w * w); }
        float Length(void) const { return sqrtf((float)LengthSquared()); }


        float Distance(const Vector4u& other) const { return sqrtf((float)DistanceSquared(other)); }
        unsigned int DistanceSquared(const Vector4u& other) const;
        unsigned int ManhattanDistance(const Vector4u& other) const;


        unsigned int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791) ^ (w * 4256233); }
    };

    #pragma endregion


	#pragma region Vector2i

	class Vector2i
	{
	public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        inline unsigned int operator()(const Vector2i& v) const
        {
            int i = v.GetHashCode();
            return *((unsigned int*)(&i));
        }


		int x;
		int y;


        Vector2i(void) : Vector2i(0, 0) { }
		Vector2i(int X, int Y) : x(X), y(Y) { }
        Vector2i(const Vector2i& cpy) : x(cpy.x), y(cpy.y) { }

        inline Vector2i& operator=(const Vector2i& cpy)
        {
            x = cpy.x;
            y = cpy.y;

            return *this;
        }


        //Access the X or Y component with 0 or 1, respectively.
        const int& operator[](int index) const { return (&x)[index]; }
        //Access the X or Y component with 0 or 1, respectively.
        int& operator[](int index) { return (&x)[index]; }

        
        Vector2i& operator+=(const Vector2i& other) { x += other.x; y += other.y; return *this; }
        Vector2i& operator-=(const Vector2i& other) { x -= other.x; y -= other.y; return *this; }
		Vector2i& operator*=(int i) { x *= i; y *= i; return *this; }
		Vector2i& operator/=(int i) { x /= i; y /= i; return *this; }

        Vector2i operator+(const Vector2i& other) const { return Vector2i(x + other.x, y + other.y); }
        Vector2i operator-(const Vector2i& other) const { return Vector2i(x - other.x, y - other.y); }
		Vector2i operator*(int i) const { return Vector2i(x * i, y * i); }
		Vector2i operator/(int i) const { return Vector2i(x / i, y / i); }
	
        Vector2i operator-(void) const { return Vector2i(-x, -y); }


        bool operator==(const Vector2i& v) const { return (x == v.x) && (y == v.y); }
        bool operator!=(const Vector2i& v) const { return (x != v.x) || (y != v.y); }


		Vector2i LessX(void) const { return Vector2i(x - 1, y); }
		Vector2i MoreX(void) const { return Vector2i(x + 1, y); }
		Vector2i LessY(void) const { return Vector2i(x, y - 1); }
		Vector2i MoreY(void) const { return Vector2i(x, y + 1); }

        Vector2i FlipX(void) const { return Vector2i(-x, y); }
        Vector2i FlipY(void) const { return Vector2i(x, -y); }


        //Scales this Vector2i's x, and y components by the given Vector2i's x and y components.
        void MultiplyComponents(Vector2i scale) { x *= scale.x; y *= scale.y; }
        //Scales this Vector2i's x, and y components by the given Vector2i's x and y components.
        Vector2i ComponentProduct(Vector2i scale) const { return Vector2i(x * scale.x, y * scale.y); }


        //Gets the dot product of this vector and another one.
        int Dot(Vector2i other) const { return (x * other.x) + (y * other.y); }


        //Clamps each component to be inside the specified range.
        Vector2i Clamp(int min, int max) const;
        //Clamps each component to be inside the specified range.
        Vector2i Clamp(Vector2i min, Vector2i max) const;

        //Gets the absolute value of this vector's components.
        Vector2u Abs(void) const { return Vector2u(BasicMath::Abs(x), BasicMath::Abs(y)); }


        int LengthSquared(void) const { return (x * x) + (y * y); }
        float Length(void) const { return sqrtf((float)LengthSquared()); }

        int DistanceSquared(Vector2i other) const;
		float Distance(Vector2i other) const { return sqrtf((float)DistanceSquared(other)); }
		int ManhattanDistance(Vector2i other) const;


		int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663); }
	};

	#pragma endregion

	#pragma region Vector3i

	class Vector3i
	{
	public:

        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        inline unsigned int operator()(const Vector3i& v) const
        {
            int i = v.GetHashCode();
            return *((unsigned int*)(&i));
        }


        int x, y, z;


        Vector3i(void) : Vector3i(0, 0, 0) { }
        Vector3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) { }
		Vector3i(Vector2i copy, int zValue) : x(copy.x), y(copy.y), z(zValue) { }
        Vector3i(const Vector3i& copy) : x(copy.x), y(copy.y), z(copy.z) { }

        inline Vector3i& operator=(const Vector3i& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;

            return *this;
        }


        //Access the X, Y, or Z component with 0, 1, or 2, respectively.
        const int& operator[](int index) const { return (&x)[index]; }
        //Access the X, Y, or Z component with 0, 1, or 2, respectively.
        int& operator[](int index) { return (&x)[index]; }


        Vector3i& operator+=(const Vector3i& r) { x += r.x; y += r.y; z += r.z; return *this; }
        Vector3i& operator-=(const Vector3i& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }
        Vector3i& operator*=(int f) { x *= f; y *= f; z *= f; return *this; }
        Vector3i& operator/=(int f) { x /= f; y /= f; z /= f; return *this; }

        Vector3i operator+(const Vector3i& v) const { return Vector3i(x + v.x, y + v.y, z + v.z); }
        Vector3i operator-(const Vector3i& v) const { return Vector3i(x - v.x, y - v.y, z - v.z); }
        Vector3i operator*(int scale) const { return Vector3i(x * scale, y * scale, z * scale); }
        Vector3i operator/(int denom) const { return Vector3i(x / denom, y / denom, z / denom); }

        Vector3i operator-(void) const { return Vector3i(-x, -y, -z); }


        bool operator==(const Vector3i& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
        bool operator!=(const Vector3i& v) const { return (x != v.x) || (y != v.y) || (z != v.z); }


        Vector3i LessX(void) const { return Vector3i(x - 1, y, z); }
        Vector3i LessY(void) const { return Vector3i(x, y - 1, z); }
        Vector3i LessZ(void) const { return Vector3i(x, y, z - 1); }
        Vector3i MoreX(void) const { return Vector3i(x + 1, y, z); }
        Vector3i MoreY(void) const { return Vector3i(x, y + 1, z); }
        Vector3i MoreZ(void) const { return Vector3i(x, y, z + 1); }

        Vector3i FlipX(void) const { return Vector3i(-x, y, z); }
        Vector3i FlipY(void) const { return Vector3i(x, -y, z); }
        Vector3i FlipZ(void) const { return Vector3i(x, y, -z); }

        Vector2i XY(void) const { return Vector2i(x, y); }
        Vector2i XZ(void) const { return Vector2i(x, z); }
        Vector2i YZ(void) const { return Vector2i(y, z); }


        //Scales this Vector3i's x, y, and z components by the given Vector3i's x, y, and z components.
        void MultiplyComponents(Vector3i scale) { x *= scale.x; y *= scale.y; z *= scale.z; }
        //Scales this Vector3i's x, y, and z components by the given Vector3i's x, y, and z components.
        inline Vector3i ComponentProduct(Vector3i scale) const
        {
            return Vector3i(x * scale.x, y * scale.y, z * scale.z);
        }


        //Clamps each component to be inside the specified range.
        Vector3i Clamp(int min, int max) const;
        //Clamps each component to be inside the specified range.
        Vector3i Clamp(Vector3i min, Vector3i max) const;


        //Gets the absolute value of this vector's components.
        Vector3u Abs(void) const { return Vector3u(BasicMath::Abs(x), BasicMath::Abs(y), BasicMath::Abs(z)); }


        //Gets the dot product of this vector and the given one.
		int Dot(Vector3i other) const { return (x * other.x) + (y * other.y) + (z * other.z); }


        float Length(void) const { return sqrtf((float)LengthSquared()); }
		int LengthSquared(void) const { return (x * x) + (y * y) + (z * z); }

        float Distance(Vector3i other) const { return sqrtf((float)DistanceSquared(other)); }
		int DistanceSquared(Vector3i other) const;
        int ManhattanDistance(Vector3i other) const;


		int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791); }
	};

	#pragma endregion

    #pragma region Vector4i

    class Vector4i
    {
    public:
        
        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector4i& v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        int x;
        int y;
        int z;
        int w;


        Vector4i(void) : Vector4i(0, 0, 0, 0) { }
        Vector4i(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w) { }
        Vector4i(Vector3i v3, int _w) : x(v3.x), y(v3.y), z(v3.z), w(_w) { }
        Vector4i(const Vector4i& cpy) : x(cpy.x), y(cpy.y), z(cpy.z), w(cpy.w) { }

        inline Vector4i& operator=(const Vector4i& cpy)
        {
            x = cpy.x;
            y = cpy.y;
            z = cpy.z;
            w = cpy.w;

            return *this;
        }


        //Access the X, Y, Z, or W component with 0, 1, 2, or 3, respectively.
        const int& operator[](int index) const { return (&x)[index]; }
        //Access the X, Y, Z, or W component with 0, 1, 2, or 3, respectively.
        int& operator[](int index) { return (&x)[index]; }


        Vector4i& operator+=(const Vector4i& r) { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
        Vector4i& operator-=(const Vector4i& r) { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
        Vector4i& operator*=(int f) { x *= f; y *= f; z *= f; w *= f; return *this; }
        Vector4i& operator/=(int f) { x /= f; y /= f; z /= f; w /= f; return *this; }

        Vector4i operator+(const Vector4i& v) const { return Vector4i(x + v.x, y + v.y, z + v.z, w + v.w); }
        Vector4i operator-(const Vector4i& v) const { return Vector4i(x - v.x, y - v.y, z - v.z, w - v.w); }
        Vector4i operator*(int scale) const { return Vector4i(x * scale, y * scale, z * scale, w * scale); }
        Vector4i operator/(int denom) const { return Vector4i(x / denom, y / denom, z / denom, w / denom); }

        Vector4i operator-(void) const { return Vector4i(-x, -y, -z, -w); }


        bool operator==(const Vector4i& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
        bool operator!=(const Vector4i& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }


        //Scales this Vector4i's components by the given Vector4i's components.
        void MultiplyComponents(Vector4i scale) { x *= scale.x; y *= scale.y; z *= scale.z; w *= scale.w; }
        //Scales this Vector4i's components by the given Vector4i's components.
        inline Vector4i ComponentProduct(Vector4i scale) const
        {
            return Vector4i(x * scale.x, y * scale.y, z * scale.z, w * scale.w);
        }


        //Clamps each component to be inside the specified range.
        Vector4i Clamp(int min, int max) const;
        //Clamps each component to be inside the specified range.
        Vector4i Clamp(Vector4i min, Vector4i max) const;


        //Gets the absolute value of this vector's components.
        inline Vector4u Abs(void) const
        {
            return Vector4u(BasicMath::Abs(x), BasicMath::Abs(y),
                            BasicMath::Abs(z), BasicMath::Abs(w));
        }


        //Gets the dot product of this vector and the given one.
        int Dot(Vector4i v) const { return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w); }


        int LengthSquared(void) const { return (x * x) + (y * y) + (z * z) + (w * w); }
        float Length(void) const { return sqrtf((float)LengthSquared()); }

        int DistanceSquared(Vector4i other) const;
        float Distance(Vector4i other) const { return sqrtf((float)DistanceSquared(other)); }
        int ManhattanDistance(Vector4i other) const;


        int GetHashCode(void) const { return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791) ^ (w * 4256233); }
    };

    #pragma endregion


	#pragma region Vector2f

	class Vector2f
	{
	public:

        //Performs a "lerp" on each component of the vector.
        static Vector2f Lerp(Vector2f min, Vector2f max, float t);
        //Performs a "lerp" on each component of the vector.
        static Vector2f Lerp(Vector2f min, Vector2f max, Vector2f t);


        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector2f& v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        float x;
		float y;


        Vector2f(void) : Vector2f(0.0f, 0.0f) { }
        Vector2f(float _x, float _y) : x(_x), y(_y) { }
        Vector2f(const Vector2f& cpy) : x(cpy.x), y(cpy.y) { }

        Vector2f& operator=(const Vector2f& cpy)
        {
            x = cpy.x;
            y = cpy.y;

            return *this;
        }


        //Access the X and Y components using 0 and 1, respectively.
        const float& operator[](int index) const { return (&x)[index]; }
        //Access the X and Y components using 0 and 1, respectively.
        float& operator[](int index) { return (&x)[index]; }


        Vector2f& operator+=(const Vector2f& other) { x += other.x; y += other.y; return *this; }
		Vector2f& operator-=(const Vector2f& other) { x -= other.x; y -= other.y; return *this; }
		Vector2f& operator*=(float f) { x *= f; y *= f; return *this; }
		Vector2f& operator/=(float f) { x /= f; y /= f; return *this; }

		Vector2f operator+(const Vector2f& other) const { return Vector2f(x + other.x, y + other.y); }
		Vector2f operator-(const Vector2f& other) const { return Vector2f(x - other.x, y - other.y); }
		Vector2f operator*(float f) const { return Vector2f(x * f, y * f); }
		Vector2f operator/(float f) const { return Vector2f(x / f, y / f); }

		Vector2f operator-(void) const { return Vector2f(-x, -y); }
	

        bool operator==(const Vector2f& other) const { return (x == other.x) && (y == other.y); }
        bool operator!=(const Vector2f& other) const { return (x != other.x) || (y != other.y); }


        Vector2f FlipX(void) const { return Vector2f(-x, y); }
        Vector2f FlipY(void) const { return Vector2f(x, -y); }


        //Scales this Vector2f's x and y components by the given Vector2f's x and y components.
        void MultiplyComponents(Vector2f scale) { x *= scale.x; y *= scale.y; }
        //Scales this Vector2f's x and y components by the given Vector2f's x and y components.
        Vector2f ComponentProduct(Vector2f scale) const { return Vector2f(x * scale.x, y * scale.y); }


        //Clamps each component to be inside the specified range.
        Vector2f Clamp(float min, float max) const;
        //Clamps each component to be inside the specified range.
        Vector2f Clamp(Vector2f min, Vector2f max) const;


        //'Ceil's this vector's components (towards positive infinity).
        Vector2i Ceil(void) const { return Vector2i((int)ceil(x), (int)ceil(y)); }
        //Floors this vector's components (towards negative infinity).
        Vector2i Floored(void) const { return Vector2i((int)floor(x), (int)floor(y)); }
        //Gets the absolute value of all components.
        Vector2f Abs(void) const { return Vector2f(BasicMath::Abs(x), BasicMath::Abs(y)); }

        //Rotates this vector by the given amount of radians.
        void Rotate(float radians);
        //Rotates this vector by the given amount of radians.
        Vector2f Rotated(float radians) const;

        //Normalize this vector.
        void Normalize(void);
        //Normalize this vector.
        Vector2f Normalized(void) const;

        //Normalize this vector using a good and fast approximation.
        void FastNormalize(void);
        //Normalize this vector using a good and fast approimation.
        Vector2f FastNormalized(void) const;


		float Dot(Vector2f v) const { return (x * v.x) + (y * v.y); }
		float AngleBetween(Vector2f v) const { return acosf(Dot(v) / (Length() * v.Length())); }


        //Quickly computes "1.0 / this->Length()" using a good approximation.
		float FastInvLength(void) const { return BasicMath::FastInvSqrt1(LengthSquared()); }

		float LengthSquared(void) const { return (x * x) + (y * y); }
		float Length(void) const { return sqrtf(LengthSquared()); }

		float Distance(Vector2f other) const { return sqrtf(DistanceSquared(other)); }
		float DistanceSquared(Vector2f other) const;
		float ManhattanDistance(Vector2f other) const;

	
        int GetHashCode(void) const;
	};

	#pragma endregion

	#pragma region Vector3f

	class Vector3f
	{
	public:

        //Performs a "lerp" on each component of the vector.
        static Vector3f Lerp(Vector3f min, Vector3f max, float t);
        //Performs a "lerp" on each component of the vector.
        static Vector3f Lerp(Vector3f min, Vector3f max, Vector3f t);


        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector3f& v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        float x;
		float y;
		float z;


        Vector3f(void) : Vector3f(0.0f, 0.0f, 0.0f) { }
        Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
        Vector3f(Vector2f copy, float zValue) : x(copy.x), y(copy.y), z(zValue) { }
        Vector3f(const Vector3f& cpy) : x(cpy.x), y(cpy.y), z(cpy.z) { }

        inline Vector3f& operator=(const Vector3f& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;

            return *this;
        }


        Vector3f& operator+=(const Vector3f& r) { x += r.x; y += r.y; z += r.z; return *this; }
		Vector3f& operator-=(const Vector3f& r) { x -= r.x; y -= r.y; z -= r.z; return *this; }
		Vector3f& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
		Vector3f& operator/=(float f) { x /= f; y /= f; z /= f; return *this; }

		Vector3f operator+(const Vector3f& v) const { return Vector3f(x + v.x, y + v.y, z + v.z); }
		Vector3f operator-(const Vector3f& v) const { return Vector3f(x - v.x, y - v.y, z - v.z); }
		Vector3f operator*(float scale) const { return Vector3f(x * scale, y * scale, z * scale); }
        Vector3f operator/(float denom) const { return Vector3f(x / denom, y / denom, z / denom); }
	
		Vector3f operator-(void) const { return Vector3f(-x, -y, -z); }


        //Access the X, Y, and Z components using 0, 1, and 2, respectively.
        const float& operator[](int index) const { return (&x)[index]; }
        //Access the X, Y, and Z components using 0, 1, and 2, respectively.
		float& operator[](int index) { return (&x)[index]; }
	

        bool operator==(const Vector3f& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
        bool operator!=(const Vector3f& v) const { return (x != v.x) || (y != v.y) || (z != v.z); }


        Vector3f FlipX(void) const { return Vector3f(-x, y, z); }
        Vector3f FlipY(void) const { return Vector3f(x, -y, z); }
        Vector3f FlipZ(void) const { return Vector3f(x, y, -z); }

        Vector2f XY(void) const { return Vector2f(x, y); }
        Vector2f XZ(void) const { return Vector2f(x, z); }
        Vector2f YZ(void) const { return Vector2f(y, z); }


		//Scales this Vector3f's x, y, and z components by the given Vector3f's x, y, and z components.
		void MultiplyComponents(Vector3f scale) { x *= scale.x; y *= scale.y; z *= scale.z; }
		//Scales this Vector3f's x, y, and z components by the given Vector3f's x, y, and z components.
        inline Vector3f ComponentProduct(Vector3f scale) const
        {
            return Vector3f(x * scale.x, y * scale.y, z * scale.z);
        }


        //Clamps each component to be inside the specified range.
        Vector3f Clamp(float min, float max) const;
        //Clamps each component to be inside the specified range.
        Vector3f Clamp(Vector3f min, Vector3f max) const;


        //'Ceil's this vector's components (towards positive infinity).
        Vector3i Ceil(void) const { return Vector3i((int)ceil(x), (int)ceil(y), (int)ceil(z)); }
        //Floors this vector's components (towards negative infinity).
        Vector3i Floored(void) const { return Vector3i((int)floor(x), (int)floor(y), (int)floor(z)); }
        //Gets the absolute value of all components.
        Vector3f Abs(void) const { return Vector3f(BasicMath::Abs(x), BasicMath::Abs(y), BasicMath::Abs(z)); }


        //Normalize this vector.
        void Normalize(void);
        //Normalize this vector.
        Vector3f Normalized(void) const;

        //Normalize this vector using a good and fast approximation.
        void FastNormalize(void);
        //Normalize this vector using a good and fast approximation.
        Vector3f FastNormalized(void) const;


		float Dot(Vector3f v) const { return (x * v.x) + (y * v.y) + (z * v.z); }
		float AngleBetween(Vector3f v) const { return acosf(Dot(v) / (Length() * v.Length())); }


        //Quickly computes "1.0 / this->Length()" using a good approximation.
		float FastInvLength(void) const { return BasicMath::FastInvSqrt1(LengthSquared()); }

		float Length(void) const { return sqrtf(LengthSquared()); }
		float LengthSquared(void) const { return (x * x) + (y * y) + (z * z); }

		float Distance(Vector3f other) const { return sqrtf(DistanceSquared(other)); }
		float DistanceSquared(Vector3f other) const;
        float ManhattanDistance(Vector3f other) const;


		Vector3f Cross(Vector3f v) const;
        Vector3f Reflect(Vector3f normal) const { return (*this) + (normal * 2.0 * (-normal).Dot(*this)); }


        int GetHashCode(void) const;
	};

	#pragma endregion

	#pragma region Vector4f

	class Vector4f
	{
	public:
		
        //Performs a "lerp" on each component of the vector.
        static Vector4f Lerp(Vector4f min, Vector4f max, float t);
        //Performs a "lerp" on each component of the vector.
        static Vector4f Lerp(Vector4f min, Vector4f max, Vector4f t);


        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        //Gets the hash value for a vector instance.
        //Enables this class to be used for std collections that use hashes.
        unsigned int operator()(const Vector4f& v) const { int i = v.GetHashCode(); return *((unsigned int*)(&i)); }


        float x;
		float y;
		float z;
		float w;
        

        Vector4f(void) : Vector4f(0.0f, 0.0f, 0.0f, 0.0f) { }
        Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
        Vector4f(Vector3f v3, float _w) : x(v3.x), y(v3.y), z(v3.z), w(_w) { }
        Vector4f(const Vector4f& cpy) : x(cpy.x), y(cpy.y), z(cpy.z), w(cpy.w) { }
        
        inline Vector4f& operator=(const Vector4f& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;

            return *this;
        }


        //Access the X, Y, Z, and W components using 0, 1, 2, and 3, respectively.
        const float& operator[](int index) const { return (&x)[index]; }
        //Access the X, Y, Z, and W components using 0, 1, 2, and 3, respectively.
        float& operator[](int index) { return (&x)[index]; }


        Vector4f& operator+=(const Vector4f& r) { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
		Vector4f& operator-=(const Vector4f& r) { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }
		Vector4f& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
		Vector4f& operator/=(float f) { x /= f; y /= f; z /= f; w /= f; return *this; }

		Vector4f operator+(const Vector4f& v) const { return Vector4f(x + v.x, y + v.y, z + v.z, w + v.w); }
		Vector4f operator-(const Vector4f& v) const { return Vector4f(x - v.x, y - v.y, z - v.z, w - v.w); }
		Vector4f operator*(float scale) const { return Vector4f(x * scale, y * scale, z * scale, w * scale); }
        Vector4f operator/(float denom) const { return Vector4f(x / denom, y / denom, z / denom, w / denom); }
	
		Vector4f operator-(void) const { return Vector4f(-x, -y, -z, -w); }
	

        bool operator==(const Vector4f& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
        bool operator!=(const Vector4f& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }


        Vector4f FlipX(void) const { return Vector4f(-x, y, z, w); }
        Vector4f FlipY(void) const { return Vector4f(x, -y, z, w); }
        Vector4f FlipZ(void) const { return Vector4f(x, y, -z, w); }
        Vector4f FlipW(void) const { return Vector4f(x, y, z, -w); }


        //Scales this Vector4f's components by the given Vector4f's components.
        void MultiplyComponents(Vector4f scale) { x *= scale.x; y *= scale.y; z *= scale.z; w *= scale.w; }
        //Scales this Vector4f's components by the given Vector4f's components.
        inline Vector4f ComponentProduct(Vector4f scale) const
        {
            return Vector4f(x * scale.x, y * scale.y, z * scale.z, w * scale.w);
        }


        Vector4f Clamp(float min, float max) const;
        Vector4f Clamp(Vector4f min, Vector4f max) const;


        //'Ceil's this vector's components (towards positive infinity).
        inline Vector4i Ceil(void) const
        {
            return Vector4i((int)ceil(x), (int)ceil(y), (int)ceil(z), (int)ceil(w));
        }
        //Floors this vector's components (towards negative infinity).
        inline Vector4i Floored(void) const
        {
            return Vector4i((int)floor(x), (int)floor(y), (int)floor(z), (int)floor(w));
        }
        //Gets the absolute value of this vector's components.
        inline Vector4f Abs(void) const
        {
            return Vector4f(BasicMath::Abs(x), BasicMath::Abs(y), BasicMath::Abs(z), BasicMath::Abs(w));
        }


        //Normalize this vector.
        void Normalize(void);
        //Normalize this vector.
        Vector4f Normalized(void) const;

        //Normalize this vector using a good and fast approximation.
        void FastNormalize(void);
        //Normalize this vector using a good and fast approximation.
        Vector4f FastNormalized(void) const;


		float Dot(Vector4f v) const { return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w); }
		float AngleBetween(Vector4f v) const { return acosf(Dot(v) / (Length() * v.Length())); }


        //Quickly computes "1.0 / this->Length()" using a good approximation.
		float FastInvLength(void) const { return BasicMath::FastInvSqrt1(LengthSquared()); }

		float Length(void) const { return sqrtf(LengthSquared()); }
		float LengthSquared(void) const { return (x * x) + (y * y) + (z * z) + (w * w); }

		float Distance(Vector4f other) const { return sqrtf(DistanceSquared(other)); }
		float DistanceSquared(Vector4f other) const;
		float ManhattanDistance(Vector4f other) const;


        int GetHashCode(void) const;
	};

	#pragma endregion



    //Functions to cast between different types of vectors.

    inline Vector2u ToV2u(Vector2i inV)
    {
        assert(inV.x >= 0 && inV.y >= 0);
        return Vector2u((unsigned int)inV.x, (unsigned int)inV.y);
    }
    inline Vector3u ToV3u(Vector3i inV)
    {
        assert(inV.x >= 0 && inV.y >= 0 && inV.z >= 0);
        return Vector3u((unsigned int)inV.x, (unsigned int)inV.y, (unsigned int)inV.z);
    }
    inline Vector4u ToV4u(Vector4i inV)
    {
        assert(inV.x >= 0 && inV.y >= 0 && inV.z >= 0 && inV.w >= 0);
        return Vector4u((unsigned int)inV.x, (unsigned int)inV.y,
                        (unsigned int)inV.z, (unsigned int)inV.w);
    }

    inline Vector2u ToV2u(Vector2f inV)
    {
        assert(inV.x >= 0.0f && inV.y >= 0.0f);
        return Vector2u((unsigned int)inV.x, (unsigned int)inV.y);
    }
    inline Vector3u ToV3u(Vector3f inV)
    {
        assert(inV.x >= 0.0f && inV.y >= 0.0f && inV.z >= 0.0f);
        return Vector3u((unsigned int)inV.x, (unsigned int)inV.y, (unsigned int)inV.z);
    }
    inline Vector4u ToV4u(Vector4f inV)
    {
        assert(inV.x >= 0.0f && inV.y >= 0.0f && inV.z >= 0.0f && inV.w >= 0.0f);
        return Vector4u((unsigned int)inV.x, (unsigned int)inV.y,
                        (unsigned int)inV.z, (unsigned int)inV.w);
    }

    inline Vector2i ToV2i(Vector2u inV) { return Vector2i((int)inV.x, (int)inV.y); }
    inline Vector3i ToV3i(Vector3u inV) { return Vector3i((int)inV.x, (int)inV.y, (int)inV.z); }
    inline Vector4i ToV3i(Vector4u inV) { return Vector4i((int)inV.x, (int)inV.y, (int)inV.z, (int)inV.w); }

    inline Vector2i ToV2i(Vector2f inV) { return Vector2i((int)inV.x, (int)inV.y); }
    inline Vector3i ToV3i(Vector3f inV) { return Vector3i((int)inV.x, (int)inV.y, (int)inV.z); }
    inline Vector4i ToV3i(Vector4f inV) { return Vector4i((int)inV.x, (int)inV.y, (int)inV.z, (int)inV.w); }


    inline Vector2f ToV2f(Vector2i inV) { return Vector2f((float)inV.x, (float)inV.y); }
    inline Vector3f ToV3f(Vector3i inV) { return Vector3f((float)inV.x, (float)inV.y, (float)inV.z); }
    inline Vector4f ToV4f(Vector4i inV) { return Vector4f((float)inV.x, (float)inV.y, (float)inV.z, (float)inV.w); }

    inline Vector2f ToV2f(Vector2u inV) { return Vector2f((float)inV.x, (float)inV.y); }
    inline Vector3f ToV3f(Vector3u inV) { return Vector3f((float)inV.x, (float)inV.y, (float)inV.z); }
    inline Vector4f ToV4f(Vector4u inV) { return Vector4f((float)inV.x, (float)inV.y, (float)inV.z, (float)inV.w); }
}

//Hide the namespace so that it only needs to be specified if a file is also using SFML stuff.
using namespace ManbilVectors;