#include "Camera.h"

Camera::Camera(Vector3f _pos, Vector3f _forward, Vector3f _up, bool lockUp)
    : pos(_pos), forward(_forward), up(_up), ClosestDotVariance(0.01f), LockUp(lockUp),
      MinOrthoBounds(-50.0f, -50.0f, 0.0f), MaxOrthoBounds(50.0f, 50.0f, 1000.0f)
{
	IncrementPosition(Vector3f());
    forward.Normalize();
    up.Normalize();
	SetRotation(forward, up);
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
        //The camera's up vector is locked, so make sure the forward vector doesn't get too close.
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

void Camera::SetRotation(Vector3f newForward, Vector3f newUp)
{
	forward = newForward;
	up = newUp;
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
	mRot.SetAsOrientation(forward, up, true);

	outM.Set(Matrix4f::Multiply(mRot, mPos));
}
void Camera::GetPerspectiveProjection(Matrix4f& outM) const
{
    outM.SetAsPerspProj(PerspectiveInfo.FOV, PerspectiveInfo.Width, PerspectiveInfo.Height,
                        PerspectiveInfo.zNear, PerspectiveInfo.zFar);
}
void Camera::GetOrthoProjection(Matrix4f& outM) const
{
    outM.SetAsOrthoProj(MinOrthoBounds + pos, MaxOrthoBounds + pos);
}