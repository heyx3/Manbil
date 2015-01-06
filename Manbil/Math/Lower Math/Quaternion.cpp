#include "Quaternion.h"

#include <math.h>
#include "Mathf.h"



Quaternion Quaternion::Slerp(Quaternion one, Quaternion two, float zeroToOne, bool areBothNormalized)
{
	if (!areBothNormalized)
	{
        one.Normalize();
        two.Normalize();
	}

	float dot = one.Dot(two);

	dot = Mathf::Clamp(dot, -1.0f, 1.0f);
	float theta = acosf(dot) * zeroToOne;

	Quaternion finalQ = two - (one * dot);
	finalQ.Normalize();

	return (one * cosf(theta)) + (finalQ * sinf(theta));
}
Quaternion Quaternion::Nlerp(Quaternion one, Quaternion two, float t)
{
    float t_1 = 1.0f - t;

    Vector4f result;
    if (one.Dot(two) < 0.0f)
        result = (*(Vector4f*)(&one) * t_1) + (*(Vector4f*)(&two) * -t);
    else result = (*(Vector4f*)(&one) * t_1) + (*(Vector4f*)(&two) * t);

    result.Normalize();
    return *(Quaternion*)(&result);
}
Quaternion Quaternion::NlerpFast(Quaternion one, Quaternion two, float t)
{
    float t_1 = 1.0f - t;

    Vector4f result;
    if (one.Dot(two) < 0.0f)
        result = (*(Vector4f*)(&one) * t_1) + (*(Vector4f*)(&two) * -t);
    else result = (*(Vector4f*)(&one) * t_1) + (*(Vector4f*)(&two) * t);

    result.FastNormalize();
    return *(Quaternion*)(&result);
}


Quaternion::Quaternion(Vector3f axisOfRotation, float rotInRadians)
{
	const float sinHalfAngle = sin(rotInRadians * 0.5f),
		        cosHalfAngle = cos(rotInRadians * 0.5f);

	x = axisOfRotation.x * sinHalfAngle;
	y = axisOfRotation.y * sinHalfAngle;
	z = axisOfRotation.z * sinHalfAngle;
	w = cosHalfAngle;
}
Quaternion::Quaternion(Vector3f from, Vector3f to)
{
    float dotted = from.Dot(to);

    if (1.0f - dotted < 0.0001f)
    {
        *this = Quaternion();
    }
    else if (1.0f + dotted < 0.0001f)
    {
        //Get an arbitrary perpendicular axis to rotate around.
        Vector3f axis = (Mathf::Abs(from.Dot(Vector3f(1.0f, 0.0f, 0.0f))) < 1.0f) ?
                            Vector3f(1.0f, 0.0f, 0.0f) :
                            Vector3f(0.0f, 1.0f, 0.0f);
        axis = axis.Cross(from).Normalized();

        *this = Quaternion(axis, 3.1415926536f);
    }
    else
    {
        //The value of xyz is the cross product of the given vectors.
        Vector3f norm = from.Cross(to);
        memcpy(&x, &norm, sizeof(float) * 3);

        //The value of w is based on the angle between the two vectors.
        w = (1.0f + dotted);

        Normalize();
    }
}
Quaternion::Quaternion(Vector3f eulerAngles)
{
    eulerAngles *= 0.5f;

    Vector3f c(cosf(eulerAngles.x), cosf(eulerAngles.y), cosf(eulerAngles.z)),
             s(sinf(eulerAngles.x), sinf(eulerAngles.y), sinf(eulerAngles.z));

    w = (c.z * c.y * c.x) + (s.z * s.y * s.x);
    x = (c.z * c.y * s.x) - (s.z * s.y * c.x);
    y = (c.z * s.y * c.x) + (s.z * c.y * s.x);
    z = (s.z * c.y * c.x) - (c.z * s.y * s.x);
}


Quaternion Quaternion::Normalized(void) const
{
    Vector4f normed = ((Vector4f*)this)->Normalized();
    return *(Quaternion*)(&normed);
}
Quaternion Quaternion::FastNormalized(void) const
{
    Vector4f normed = ((Vector4f*)this)->FastNormalized();
    return *(Quaternion*)(&normed);
}


Quaternion Quaternion::Multiply(const Quaternion& l, const Quaternion& r)
{
	const float w2 = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z),
				x2 = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y),
				y2 = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z),
				z2 = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

	return Quaternion(x2, y2, z2, w2);
}
Quaternion Quaternion::Multiply(const Quaternion& l, const Vector3f& r)
{
	return Multiply(l, Quaternion(r.x, r.y, r.z, 0.0f));
}


void Quaternion::ToMatrix(Matrix4f& out, bool amINormalized) const
{
	Quaternion q = (amINormalized ? *this : Normalized());
	float xSqr = q.x * q.x,
		  ySqr = q.y * q.y,
		  zSqr = q.z * q.z,
		  xy2 = 2 * q.x * q.y,
		  xz2 = 2 * q.x * q.z,
		  xw2 = 2 * q.x * q.w,
		  yz2 = 2 * q.y * q.z,
		  yw2 = 2 * q.y * q.w,
		  zw2 = 2 * q.z * q.w;

	out.Set(0.0f);

	Vector2u l(0, 0);

	out[l] = 1 - (2 * ySqr) - (2 * zSqr);
	l.x = 1;
	out[l] = xy2 - zw2;
	l.x = 2;
	out[l] = yz2 + xw2;


	l.y = 1;
	l.x = 0;

	out[l] = xy2 + zw2;
	l.x = 1;
	out[l] = 1 - (2 * xSqr) - (2 * zSqr);
	l.x = 2;
	out[l] = yz2 + xw2;


	l.y = 2;
	l.x = 0;

	out[l] = xz2 - yw2;
	l.x = 1;
	out[l] = yz2 - xw2;
	l.x = 2;
	out[l] = 1 - (2 * xSqr) - (2 * ySqr);


	l.y = 3;
	l.x = 0;

	out[l] = 1;
}
Vector4f Quaternion::GetAxisAngle(void) const
{
    float halfAngle = acosf(w);
    return Vector4f(*(Vector3f*)&x * (1.0f / sinf(halfAngle)),
                    2.0f * halfAngle);
}
Vector3f Quaternion::GetEulerAngles(void) const
{
    float determinant = 2.0f * ((x * z) - (w * y));
    if (determinant == -1.0f)
    {
        return Vector3f(atan2f((x * y) - (w * z),
                               (x * z) + (w * y)),
                        (3.1415926535898f * 0.5f),
                        0.0f);
    }
    else if (determinant == 1.0f)
    {
        return Vector3f(atan2f((x * y) - (w * z),
                               (x * z) + (w * y)),
                        (-3.1415926535898f * 0.5f),
                        0.0f);
    }
    else
    {
        return Vector3f(atan2f((y * z) + (w * x),
                               0.5f - ((x * x) + (y * y))),
                        asinf(-determinant),
                        atan2f((x * y) + (w * z),
                               0.5f - ((y * y) + (z * z))));
    }


    //Old version of this function:
    return Vector3f(atan2f(2.0f * ((w * x) + (y * z)),
                           1.0f - (2.0f * ((x * x) + (y * y)))),
                    asinf(2.0f * ((w * y) - (z * x))),
                    atan2f(2.0f * ((w * z) + (x * y)),
                           1.0f - (2.0f * ((y * y) + (z * z)))));
}


void Quaternion::Rotate(Vector3f& v) const
{
	Quaternion conjugate = operator-();
	Quaternion finalW = Multiply(Multiply(*this, v), conjugate);

	v.x = finalW.x;
	v.y = finalW.y;
	v.z = finalW.z;
}