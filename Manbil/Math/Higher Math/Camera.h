#pragma once

#include "../LowerMath.hpp"
#include "ProjectionInfo.h"


//Basic camera functionality: UVN rotation, transform calculation, etc.
//Has two modes, indicated by the "LockUp" flag:
//   - If "LockUp" is true, the camera's Up vector will not be affected by pitching,
//        and the Forward vector can only get so close to it (specified by "ClosestDotVariance").
//        Rolling the camera or rotating it with a Quaternion will still change the Up vector like normal.
//   - If "LockUp" is false, the camera's Up vector is not locked in any way.
class Camera
{
public:

    //If true, keeps the upward vector along the positive Z axis regardless of rotation.
    bool LockUp;
    //If "LockUp" is true, this field indicates the closest the camera can get to the up/down vector.
    //Should be between 0 (can look all the way up/down) and 1 (can't pitch the camera at all).
    float ClosestDotVariance;

    //The information about this camera's 3D perspective (field of view, z-near/far, and aspect ratio).
	ProjectionInfo PerspectiveInfo;
    //The information about this camera's 2D perspective (visible region, relative to the camera).
    Vector3f MinOrthoBounds, MaxOrthoBounds;


	Camera(Vector3f pos = Vector3f(0.0f, 0.0f, 0.0f),
           Vector3f forward = Vector3f(1.0f, 0.0f, 0.0f),
           Vector3f up = Vector3f(0.0f, 0.0f, 1.0f),
           bool lockUp = true);


	Vector3f GetPosition(void) const { return pos; }
	Vector3f GetForward(void) const { return forward; }
	Vector3f GetUpward(void) const { return up; }
	Vector3f GetSideways(void) const { return up.Cross(forward).Normalized(); }

	void SetPosition(Vector3f newPos) { pos = newPos; }
	void IncrementPosition(Vector3f amount) { SetPosition(pos + amount); }

	void SetRotation(Vector3f newForward, Vector3f newUp);
    void Rotate(Quaternion rotation);
	void AddPitch(float radians);
	void AddYaw(float radians);
	void AddRoll(float radians);

	void GetViewTransform(Matrix4f& outM) const;
    void GetPerspectiveProjection(Matrix4f& outM) const;
    void GetOrthoProjection(Matrix4f& outM) const;
	

private:

	Vector3f pos, forward, up;
};