#include "Camera.h"

Camera::Camera(Vector3f _pos, Vector3f _forward, Vector3f _up)
	: pos(_pos), forward(_forward), up(_up), closestDotVariance(0.01f),
      MinOrthoBounds(-50.0f, -50.0f, 0.0f), MaxOrthoBounds(50.0f, 50.0f, 1000.0f)
{
	IncrementPosition(Vector3f());
	SetRotation(forward, up, false);
}


void Camera::AddPitch(float radians)
{
	Vector3f oldForward = forward;

	Quaternion rotation(GetSideways(), radians);
	rotation.Rotate(forward);

	forward.Normalize();

	float dot = forward.Dot(up);
	float variance = abs(abs(dot) - 1);

	if (variance < closestDotVariance)
	{
		forward = oldForward;
	}
}
void Camera::AddYaw(float radians)
{
	Quaternion rotation(up, radians);

	rotation.Rotate(forward);
}
void Camera::AddRoll(float radians)
{
	up = forward.Cross(GetSideways());

	Quaternion rotation(forward, radians);
	rotation.Rotate(up);

	up.Normalize();
}

void Camera::SetRotation(Vector3f newForward, Vector3f newUp, bool alreadyNormalized)
{
	if (alreadyNormalized)
	{
		forward = newForward;
		up = newUp;
	}
	else
	{
		forward = newForward.Normalized();
		up = newUp.Normalized();
	}
}
void Camera::Rotate(Quaternion rotation)
{
    rotation.Rotate(forward);
    rotation.Rotate(up);
}

void Camera::GetViewTransform(Matrix4f & outM) const
{
	Matrix4f mPos, mRot, mVP;
	
	mPos.SetAsTranslation(-pos);
	mRot.SetAsRotation(forward, up, true);
	mVP = Matrix4f::Multiply(mRot, mPos);

	outM.SetValues(&mVP);
}
void Camera::GetOrthoProjection(Matrix4f & outM) const
{
    outM.SetAsOrthoProj(MinOrthoBounds, MaxOrthoBounds);
}