#pragma once

#include "Vectors.h"
#include "Matrix4f.h"

class Quaternion
{
public:
	
	//Interpolates between two quaternions smoothly. Is not commutative, but keeps a constant velocity and finds the shortest path.
	static Quaternion Slerp(Quaternion one, Quaternion two, float zeroToOne, bool areBothNormalized);
	//Interpolates between two quaternions smoothly. Does not have a constant velocity, but it is commutative, finds the shortest path,  and is relatively efficient.
	static Quaternion Nlerp(Quaternion one, Quaternion two, float zeroToOne);

	float x, y, z, w;

    //Creates a quaternion that represents a rotation of 0 rad along the Z axis.
	Quaternion(void): x(0.0f), y(0.0f), z(1.0f), w(0.0f) { }
    //Creates a quaternion with the given components.
	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
    //Creates a quaternion that represents a rotation of the given amount around the given axis.
	Quaternion(Vector3f axisOfRotation, float rotInRadians);
    //Creates a quaternion that represents two other rotations: the first rotation followed by the second rotation.
    Quaternion(Quaternion firstRotation, Quaternion secondRotation)
        : Quaternion(Multiply(secondRotation, firstRotation)) { }
    //Creates a quaternion that rotates from the first given vector to the second given vector.
    //TODO: Test. If it works, use it to make a "tangent space to world space" material node. 
    Quaternion(Vector3f from, Vector3f to);

	void Normalize(void);
	Quaternion Normalized(void) const;

	float Length(void) const { return sqrtf(LengthSquared()); }
	float LengthSquared(void) const { return Vector4f(x, y, z, w).LengthSquared(); }

	float Dot(Quaternion other) const { return (w * other.w) + (x * other.x) + (y * other.y) + (z * other.z); }

	Quaternion CalcConjugate() const { Quaternion ret(-x, -y, -z, w); return ret; }

	Quaternion operator-(void) const { return Quaternion(-x, -y, -z, -w); }

	Quaternion operator+(const Quaternion & q) const { return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w); }
	Quaternion operator-(const Quaternion & q) const { return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w); }
	Quaternion operator*(float scale) const { return Quaternion(x * scale, y * scale, z * scale, w * scale); }
	Quaternion operator/(float invScale) const { return Quaternion(x / invScale, y / invScale, z / invScale, w / invScale); }

	static Quaternion Multiply(const Quaternion & l, const Vector3f & r);
	static Quaternion Multiply(const Quaternion & l, const Quaternion & r);

	void Round(int decimals);

	void ToMatrix(Matrix4f & out, bool amINormalized = false) const;

    //Applies this Quaternion's rotation to the given vector.
    void Rotate(Vector3f & v) const;
    //Applies this Quaternion's rotation to the given vector.
    Vector3f Rotated(Vector3f v) const { Rotate(v); return v; }
};