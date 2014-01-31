#pragma once

#include "../LowerMath.hpp"

//Represents an object with a position, orientation, and scale.
class TransformObject
{
public:
	
	TransformObject(void);
	TransformObject(Vector3f position, Vector3f eulerRotationAngles = Vector3f(), Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f));

	Vector3f GetPosition(void) const { return pos; }
	Vector3f GetRotationAngles(void) const { return eulerRotation; }
	Vector3f GetScale(void) const { return scale; }

	void SetPosition(Vector3f value) { pos = value; }
	void SetRotation(Vector3f eulerAngleAmounts) { eulerRotation = eulerAngleAmounts; }
	void SetScale(Vector3f value) { scale = value; }
	void SetScale(float value) { scale = Vector3f(value, value, value); }

	void IncrementPosition(Vector3f value) { pos += value; }
	void Rotate(Vector3f eulerAngleAmounts) { eulerRotation += eulerAngleAmounts; }
	
	void GetTranslationMatrix(Matrix4f & outM) const { outM.SetAsTranslation(pos); }
	void GetRotationMatrix(Matrix4f & outM) const;
	void GetScaleMatrix(Matrix4f & outM) const { outM.SetAsScale(scale); }
	void GetWorldTransform(Matrix4f & outM) const;

private:

	Vector3f pos, scale, eulerRotation;
};