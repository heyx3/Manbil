#include "TransformObject.h"



TransformObject::TransformObject(void)
    : pos(Vector3f()), scale(Vector3f(1.0f, 1.0f, 1.0f))
{
    SetRotation(Quaternion());
}
TransformObject::TransformObject(Vector3f position, Vector3f eulerRotationAngles, Vector3f _scale)
    : pos(position), scale(_scale)
{
	SetRotation(eulerRotationAngles);
}
TransformObject::TransformObject(Vector3f position, Quaternion _rot, Vector3f _scale)
    : pos(position), scale(_scale)
{
    SetRotation(_rot);
}

Vector3f TransformObject::GetRightward(void) const
{
    Vector3f side = forward.Cross(up);
    return (side.x < 0.0f) ? -side : side;
}

void TransformObject::SetRotation(Vector3f eulerAngleAmounts)
{
    rot = Quaternion(eulerAngleAmounts);
    CalculateNewDirVectors();
}

void TransformObject::GetWorldTransform(Matrix4f & outM) const
{
    //TODO: Figure out the math for making one matrix instead of multiplying the pos, rot, and scale matrices.

	Matrix4f posM, rotM, scaleM;
	GetTranslationMatrix(posM);
	GetRotationMatrix(rotM);
	GetScaleMatrix(scaleM);

	Matrix4f ret = Matrix4f::Multiply(posM, rotM, scaleM);

	outM.Set(ret);
}

void TransformObject::CalculateNewDirVectors(void)
{
    forward = rot.Rotated(Forward());
    up = rot.Rotated(Upward());
}