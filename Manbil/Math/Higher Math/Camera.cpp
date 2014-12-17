#include "Camera.h"

Camera::Camera(Vector3f _pos, Vector3f _forward, Vector3f _up, bool lockUp)
    : pos(_pos), forward(_forward), up(_up), ClosestDotVariance(0.01f), LockUp(lockUp),
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

    if (!LockUp)
    {
        rotation.Rotate(up);
        up.Normalize();
    }

    else
    {
        float dot = forward.Dot(up);
        float variance = abs(abs(dot) - 1);

        if (variance < ClosestDotVariance)
        {
            forward = oldForward;
        }
    }
}
void Camera::AddYaw(float radians)
{
	Quaternion rotation(up, radians);

	rotation.Rotate(forward);
    forward.Normalize();
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
    forward.Normalize();
    rotation.Rotate(up);
    up.Normalize();
}

void Camera::GetViewTransform(Matrix4f& outM) const
{
	Matrix4f mPos, mRot;
	
	mPos.SetAsTranslation(-pos);
	mRot.SetAsRotation(forward, up, true);

	outM.Set(Matrix4f::Multiply(mRot, mPos));
}
void Camera::GetPerspectiveTransform(Matrix4f& outM) const
{
    outM.SetAsPerspProj(Info.FOV, Info.Width, Info.Height, Info.zNear, Info.zFar);
}
void Camera::GetOrthoProjection(Matrix4f& outM) const
{
    outM.SetAsOrthoProj(MinOrthoBounds + pos, MaxOrthoBounds + pos);
}