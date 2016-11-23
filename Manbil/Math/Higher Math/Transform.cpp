#include "Transform.h"



Transform::Transform(void)
    : pos(Vector3f()), scale(Vector3f(1.0f, 1.0f, 1.0f))
{
    SetRotation(Quaternion());
}
Transform::Transform(Vector3f position, Vector3f eulerRotationAngles, Vector3f _scale)
    : pos(position), scale(_scale)
{
	SetRotation(eulerRotationAngles);
}
Transform::Transform(Vector3f position, Quaternion _rot, Vector3f _scale)
    : pos(position), scale(_scale)
{
    SetRotation(_rot);
}

Vector3f Transform::GetRightward(void) const
{
    Vector3f side = forward.Cross(up);
    return (side.x < 0.0f) ? -side : side;
}

void Transform::SetRotation(Vector3f eulerAngleAmounts)
{
    rot = Quaternion(eulerAngleAmounts);
    CalculateNewDirVectors();
}

void Transform::GetWorldTransform(Matrix4f & outM) const
{
    //TODO: Figure out the math for making one matrix instead of multiplying the pos, rot, and scale matrices.

	Matrix4f posM, rotM, scaleM;
	GetTranslationMatrix(posM);
	GetRotationMatrix(rotM);
	GetScaleMatrix(scaleM);

	Matrix4f ret = Matrix4f::Multiply(posM, rotM, scaleM);

	outM.Set(ret);
}

void Transform::CalculateNewDirVectors(void)
{
    forward = rot.Rotated(Forward());
    up = rot.Rotated(Upward());
}