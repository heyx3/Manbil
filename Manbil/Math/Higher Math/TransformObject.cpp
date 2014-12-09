#include "TransformObject.h"


TransformObject::TransformObject(void)
{
	SetPosition(Vector3f());
	SetRotation(Vector3f());
	SetScale(1.0f);
}
TransformObject::TransformObject(Vector3f position, Vector3f eulerRotationAngles, Vector3f scale)
{
	SetPosition(position);
	SetRotation(eulerRotationAngles);
	SetScale(scale);
}

void TransformObject::GetRotationMatrix(Matrix4f & outM) const
{
	Matrix4f xRot, yRot, zRot;
	xRot.SetAsRotateX(eulerRotation.x);
	yRot.SetAsRotateY(eulerRotation.y);
	zRot.SetAsRotateZ(eulerRotation.z);

	Matrix4f temp = Matrix4f::Multiply(xRot, yRot, zRot);
	outM.Set(temp);
}
void TransformObject::GetWorldTransform(Matrix4f & outM) const
{
	Matrix4f posM, rotM, scaleM;
	GetTranslationMatrix(posM);
	GetRotationMatrix(rotM);
	GetScaleMatrix(scaleM);

	Matrix4f ret = Matrix4f::Multiply(posM, rotM, scaleM);

	outM.Set(ret);
}

void TransformObject::CalculateNewDirVectors(void)
{
    Matrix4f rot;
    GetRotationMatrix(rot);
    
    forward = rot.Apply(Forward()).Normalized();
    up = Upward();
}