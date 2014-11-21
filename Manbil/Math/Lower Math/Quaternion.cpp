#include "Quaternion.h"

#include <math.h>
#include "BasicMath.h"



Quaternion Quaternion::Slerp(Quaternion one, Quaternion two, float zeroToOne, bool areBothNormalized)
{
	if (!areBothNormalized)
	{
		return Slerp(one.Normalized(), two.Normalized(), zeroToOne, true);
	}

	float dot = one.Dot(two);

	//If the quaternions are very close to each other, use a simple lerp.
	const float DOT_THRESHOLD = 0.9995f;
	if (dot > DOT_THRESHOLD)
	{
		Quaternion result = one + ((two - one) * zeroToOne);
		return result.Normalized();
	}

	dot = BasicMath::Clamp(dot, -1.0f, 1.0f);
	float theta = acosf(dot) * zeroToOne;

	Quaternion finalQ = one - (two * dot);
	finalQ.Normalize();

	return one * cosf(theta) + (finalQ * sinf(theta));
}
Quaternion Quaternion::Nlerp(Quaternion one, Quaternion two, float zeroToOne)
{
	float dot = one.Dot(two);
	float zeroToOneInv = 1.0f - zeroToOne;

	two = (dot >= 0.0f ? two : -two);
	
	return Quaternion((zeroToOneInv * one.x) + (zeroToOne * two.x),
					  (zeroToOneInv * one.y) + (zeroToOne * two.y),
					  (zeroToOneInv * one.z) + (zeroToOne * two.z),
					  (zeroToOneInv * one.w) + (zeroToOne * two.w)).Normalized();
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
    float k_cos_theta = from.Dot(to),
          k = sqrtf(from.LengthSquared() * to.LengthSquared());

    if (BasicMath::Abs(k_cos_theta + k) < 0.0005f)
    {
        Vector3f other = (BasicMath::Abs(from.Dot(Vector3f(1.0f, 0.0, 0.0f))) < 1.0f) ?
                            Vector3f(1.0f, 0.0f, 0.0f) :
                            Vector3f(0.0f, 1.0f, 0.0f);
        Vector3f axis = from.Cross(other).Normalized();

        x = axis.x;
        y = axis.y;
        z = axis.z;
        w = ToRadian(180.0f);
    }

    Vector3f axis = from.Cross(to);
    x = axis.x;
    y = axis.y;
    z = axis.z;
    w = k_cos_theta + k;
    Normalize();
}

void Quaternion::Normalize(void)
{
	float length = Length();

	x /= length;
	y /= length;
	z /= length;
	w /= length;
}
Quaternion Quaternion::Normalized(void) const
{
	Quaternion ret(*this);
	ret.Normalize();
	return ret;
}

Quaternion Quaternion::Multiply(const Quaternion & l, const Quaternion & r)
{
	const float w2 = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z),
				x2 = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y),
				y2 = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z),
				z2 = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

	return Quaternion(x2, y2, z2, w2);
}
Quaternion Quaternion::Multiply(const Quaternion & l, const Vector3f & r)
{
	return Multiply(l, Quaternion(r.x, r.y, r.z, 0.0f));
	const float w2 = -(l.x * r.x) - (l.y * r.y) - (l.z * r.z),
				x2 =  (l.w * r.x) + (l.y * r.z) - (l.z * r.y),
				y2 =  (l.w * r.y) + (l.z * r.x) - (l.x * r.z),
				z2 =  (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

	return Quaternion(x2, y2, z2, w2);
}

void Quaternion::ToMatrix(Matrix4f & out, bool amINormalized) const
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

	out.SetValues(0.0f);

	Vector2i l;

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

void Quaternion::Rotate(Vector3f & v) const
{
	Quaternion conjugate = CalcConjugate();
	Quaternion finalW = Multiply(Multiply(*this, v), conjugate);

	v.x = finalW.x;
	v.y = finalW.y;
	v.z = finalW.z;

	//v.Normalize();
}

void Quaternion::Round(int decimals)
{
	x = BasicMath::Round(x, decimals);
	y = BasicMath::Round(y, decimals);
	z = BasicMath::Round(z, decimals);
	w = BasicMath::Round(w, decimals);
}