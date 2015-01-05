#pragma once

#include "Vectors.h"
#include "Matrix4f.h"


//Represents a rotation.
class Quaternion
{
public:

    //Quaternions have several different ways they can be interpolated.
    //The various lerp algorithms have three different kinds of properties that are useful:
    //   - Commutativity: does lerp(start, end, t) == lerp(end, start, 1.0f - t)?
    //   - Constant velocity: does the lerp have a smooth, linear progression from start to end?
    //   - Shortest path: does the lerp find the shortest-possible path from start to end?

	//Interpolates between two quaternions smoothly. Is more expensive and not commutative, but
    //    it keeps a constant velocity and finds the shortest path.
    //Takes in whether both quaternions need to be normalized.
    static Quaternion Slerp(Quaternion one, Quaternion two, float zeroToOne, bool areBothNormalized);
    //Interpolates between two quaternions smoothly. Does not have a constant velocity, but
    //    it is commutative, finds the shortest path,  and is relatively efficient.
    static Quaternion Nlerp(Quaternion one, Quaternion two, float zeroToOne);
    //A more efficient, slightly less-accurate version of NLerp.
    static Quaternion NlerpFast(Quaternion one, Quaternion two, float zeroToOne);


    //Yields a new quaternion whose rotation is equal to "rhs"'s rotation followed by "lhs"'s rotation.
    static Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
    //Part of the process for rotating a vector by a quaternion.
    static Quaternion Multiply(const Quaternion& lhs, const Vector3f& rhs);


    //As quaternions are basically Vector4f's with special operations,
    //    a lot of these methods involve reinterpreting a quaternion as a Vector4f
    //    and calling some Vector4f methods on it. This is done instead of just rewriting the math here
    //    because in the future, the vector math classes can be optimized with things like SIMD ops,
    //    and now this Quaternion class can take advantage of that without having to rewrite that stuff
    //    here.
    
    //The components of this Quaternion.
	float x, y, z, w;


    //Creates a quaternion that represents no rotation.
	Quaternion(void): x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }
    //Creates a quaternion with the given components.
	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
    //Creates a quaternion that represents a rotation of the given amount around the given axis.
    //If the given axis isn't normalized, the magnitude of any vector this quaternion rotates is undefined.
	Quaternion(Vector3f axisOfRotation, float rotInRadians);
    //Creates a quaternion that represents two rotations in sequence:
    //    the first given rotation followed by the second given rotation.
    Quaternion(const Quaternion& firstRotation, const Quaternion& secondRotation)
        : Quaternion(Multiply(secondRotation, firstRotation)) { }
    //Creates a quaternion that rotates from the first given direction to the second given direction.
    //If either given direction isn't normalized, the quaternion will not work correctly.
    Quaternion(Vector3f from, Vector3f to);
    //Creates a quaternion representing a rotation by the given Euler angles.
    //The euler angles represent a rotation along the Z axis, then the rotated Y axis,
    //    then the rotated X axis.
    explicit Quaternion(Vector3f eulerAngles);


    //Gets the inverse of this quaternion.
    Quaternion operator-(void) const { return Quaternion(-x, -y, -z, w); }

    Quaternion operator+(const Quaternion& q) const { return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w); }
    Quaternion operator-(const Quaternion& q) const { return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w); }
    Quaternion operator*(float scale) const { return Quaternion(x * scale, y * scale, z * scale, w * scale); }
    Quaternion operator/(float invScale) const { return Quaternion(x / invScale, y / invScale, z / invScale, w / invScale); }


    //Applies this Quaternion's rotation to the given vector.
    void Rotate(Vector3f& v) const;
    //Applies this Quaternion's rotation to the given vector.
    Vector3f Rotated(Vector3f v) const { Rotate(v); return v; }


    //Gets the dot product of this quaternion with the given one.
    float Dot(Quaternion other) const { return ((Vector4f*)this)->Dot(*(Vector4f*)&other); }


    //Normalizes this quaternion's values.
    void Normalize(void) { ((Vector4f*)this)->Normalize(); }
    //Normalizes this quaternion's values.
	Quaternion Normalized(void) const;

    //Normalizes this quaternion's values using a fast approximation.
    void FastNormalize(void) { ((Vector4f*)this)->FastNormalize(); }
    //Normalizes this quaternion's values using a fast approximation
    Quaternion FastNormalized(void) const;

    
    //Sets the given matrix to represent the same rotation as this quaternion.
	void ToMatrix(Matrix4f& out, bool amINormalized = false) const;
    //Gets the axis this quaternion rotates points around and the angle it rotates them by.
    Vector4f GetAxisAngle(void) const;
    //Gets the euler angles (in radians) of the rotation this quaternion represents.
    //Note that these may be different than the euler angles passed into this Quaternion's constructor;
    //There is more than one way to represent a given rotation with euler angles.
    //The euler angles represent a rotation along the Z axis, then the rotated Y axis,
    //    then the rotated X axis.
    Vector3f GetEulerAngles(void) const;
};

static_assert(sizeof(Quaternion) == sizeof(Vector4f),
              "Quaternion and Vector4f should always be the same size");