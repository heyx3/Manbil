#pragma once

#include "../LowerMath.hpp"

//Represents an object with a position, orientation, and scale.
//X axis is left/right, Y axis is forward/back, and Z axis is up/down.
class TransformObject
{
public:
	
    static Vector3f Forward(void) { return Vector3f(0.0f, 1.0f, 0.0f); }
    static Vector3f Rightward(void) { return Vector3f(1.0f, 0.0f, 0.0f); }
    static Vector3f Upward(void) { return Vector3f(0.0f, 0.0f, 1.0f); }


	TransformObject(void);
	TransformObject(Vector3f position, Vector3f eulerRotationAngles = Vector3f(), Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f));

	Vector3f GetPosition(void) const { return pos; }
	Vector3f GetRotationAngles(void) const { return eulerRotation; }
	Vector3f GetScale(void) const { return scale; }


    Vector3f GetForward(void) const { return forward; }
    Vector3f GetRightward(void) const { Vector3f side = forward.Cross(up); return (side.x < 0.0f) ? -side : side; }

    //Gets the upward vector (not necessarily perpendicular to the forward and right vectors).
    Vector3f GetUpward(void) const { return up; }
    //Gets the upward vector that is perpendicular to the forward and rightward vectors.
    Vector3f GetPerpUpward(void) const { return GetRightward().Cross(GetForward()); }


	void SetPosition(Vector3f value) { pos = value; }
	void SetRotation(Vector3f eulerAngleAmounts) { eulerRotation = eulerAngleAmounts; CalculateNewDirVectors(); }
	void SetScale(Vector3f value) { scale = value; }
	void SetScale(float value) { scale = Vector3f(value, value, value); }

	void IncrementPosition(Vector3f value) { pos += value; }
    void Rotate(Vector3f eulerAngleAmounts) { eulerRotation += eulerAngleAmounts; CalculateNewDirVectors(); }
	
	void GetTranslationMatrix(Matrix4f & outM) const { outM.SetAsTranslation(pos); }
	void GetRotationMatrix(Matrix4f & outM) const;
	void GetScaleMatrix(Matrix4f & outM) const { outM.SetAsScale(scale); }
	void GetWorldTransform(Matrix4f & outM) const;

private:

	Vector3f pos, scale, eulerRotation,
             forward, up;

    void CalculateNewDirVectors(void);
};