#pragma once

#include "../LowerMath.hpp"


//TODO: Copy this class into a new "TransformObjectTree" class that is basically a TransformObject with children and a parent. It should cache its transform matrix, and along with the current "GetXMatrix" functions, it should provide overloads that return a const reference to the cached matrices.

//Represents an object with a position, orientation, and scale.
//X axis is left/right, Y axis is forward/back, and Z axis is up/down.
//Euler angle rotations are done along the Z axis (upwards -- yaw),
//    then the rotated Y axis (forwards -- roll), then the rotated X axis (sideways -- pitch).
class TransformObject
{
public:
	
    static Vector3f Forward(void) { return Vector3f(0.0f, 1.0f, 0.0f); }
    static Vector3f Rightward(void) { return Vector3f(1.0f, 0.0f, 0.0f); }
    static Vector3f Upward(void) { return Vector3f(0.0f, 0.0f, 1.0f); }


    TransformObject(void);
	TransformObject(Vector3f position, Vector3f eulerAngles = Vector3f(),
                    Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f));
    TransformObject(Vector3f position, Quaternion rot = Quaternion(),
                    Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f));


	Vector3f GetPosition(void) const { return pos; }
	Vector3f GetRotationAngles(void) const { return rot.GetEulerAngles(); }
    Quaternion GetRotation(void) const { return rot; }
	Vector3f GetScale(void) const { return scale; }


    //Gets the forward vector.
    Vector3f GetForward(void) const { return forward; }
    //Gets the upward vector (guaranteed to be perpendicular to the forward vector).
    Vector3f GetUpward(void) const { return GetRightward().Cross(GetForward()); }
    //Calculates the rightward vector (by crossing the forward and up vectors).
    Vector3f GetRightward(void) const;


	void SetPosition(Vector3f value) { pos = value; }
	void SetRotation(Vector3f eulerAngleAmounts);
    void SetRotation(const Quaternion& newRot) { rot = newRot; CalculateNewDirVectors(); }
	void SetScale(Vector3f value) { scale = value; }
	void SetScale(float value) { scale = Vector3f(value, value, value); }

	void IncrementPosition(Vector3f value) { pos += value; }
    //Rotates by the given euler angles in world space.
    void RotateAbsolute(Vector3f eulers) { SetRotation(Quaternion(rot, Quaternion(eulers))); }
    //Rotates by the given euler angles in object space.
    void RotateRelative(Vector3f eulers) { SetRotation(Quaternion(Quaternion(eulers), rot)); }
    //Applies the given rotation to this object.
    void Rotate(Quaternion rotation) { SetRotation(Quaternion(rot, rotation)); }
	
	void GetTranslationMatrix(Matrix4f& outM) const { outM.SetAsTranslation(pos); }
    void GetRotationMatrix(Matrix4f& outM) const { outM.SetAsRotation(rot); }
	void GetScaleMatrix(Matrix4f& outM) const { outM.SetAsScale(scale); }
	void GetWorldTransform(Matrix4f& outM) const;


private:

	Vector3f pos, scale;
    Quaternion rot;

    Vector3f forward, up;


    void CalculateNewDirVectors(void);
};