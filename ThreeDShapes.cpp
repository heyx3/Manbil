#include "ThreeDShapes.h"

//TODO: Implement and test.

bool Cube::TouchingSphere(const Sphere & sphere) const
{
	Vector3f s1 = Bounds.GetTopLeftFront(),
			 s2 = Bounds.GetBottomRightBack(),
			 sph = sphere.GetCenter();
	float distSqr = BasicMath::Square(sphere.Radius);

	if (sph.x < s1.x) distSqr -= BasicMath::Square(sph.x - s1.x);
	else if (sph.x > s2.x) distSqr -= BasicMath::Square(sph.x - s2.x);
	if (sph.y < s1.y) distSqr -= BasicMath::Square(sph.y - s1.y);
	else if (sph.y > s2.y) distSqr -= BasicMath::Square(sph.y - s2.y);
	if (sph.z < s1.z) distSqr -= BasicMath::Square(sph.z - s1.z);
	else if (sph.z > s2.z) distSqr -= BasicMath::Square(sph.z - s2.z);

	return distSqr > 0.0f;
}
bool Cube::TouchingCapsule(const Capsule & capsule) const
{
	return false;
}

Cube::RayTraceResult Cube::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
	//Try each face for a ray hit. Iterate through each dimension (YZ faces, XZ faces, XY faces).

	typedef GeometricMath::PointOnLineAtValueResult<Vector3f> PointOnFaces;

	Vector3f center = Bounds.GetCenter();
	Interval dInts[3] = { Bounds.GetXInterval(), Bounds.GetYInterval(), Bounds.GetZInterval() };
	bool bHits[3] = { false, false, false };
	PointOnFaces hits[3] = { PointOnFaces(Vector3f(), 0.0f), PointOnFaces(Vector3f(), 0.0f), PointOnFaces(Vector3f(), 0.0f) };
	bool positives[3];
	int ind1, ind2;
	//Loop through each dimension -- X, Y, and Z.
	for (int i = 0; i < 3; ++i)
	{
		//Get the other two dimensions.
		ind1 = (i + 1) % 3;
		ind2 = (i + 2) % 3;

		//If the ray has a chance of hitting the face, check for a hit.
		if (rayDir[i] != 0.0f)
		{
			//Start with the closest face.
			if (rayStart[i] < center[i])
			{
				hits[i] = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, i, dInts[i].GetStart());
				if (hits[i].t >= 0.0f && dInts[ind1].Touches(hits[i].Point[ind1]) && dInts[ind2].Touches(hits[i].Point[ind2]))
				{
					bHits[i] = true;
					positives[i] = false;
				}
				else
				{
					hits[i] = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, i, dInts[i].GetEnd());
					if (hits[i].t >= 0.0f && dInts[ind1].Touches(hits[i].Point[ind1]) && dInts[ind2].Touches(hits[i].Point[ind2]))
					{
						bHits[i] = true;
						positives[i] = true;
					}
				}
			}
			else
			{
				hits[i] = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, i, dInts[i].GetEnd());
				if (hits[i].t >= 0.0f && dInts[ind1].Touches(hits[i].Point[ind1]) && dInts[ind2].Touches(hits[i].Point[ind2]))
				{
					bHits[i] = true;
					positives[i] = true;
				}
				else
				{
					hits[i] = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, i, dInts[i].GetStart());
					if (hits[i].t >= 0.0f && dInts[ind1].Touches(hits[i].Point[ind1]) && dInts[ind2].Touches(hits[i].Point[ind2]))
					{
						bHits[i] = true;
						positives[i] = false;
					}
				}
			}
		}
	}

	//Get the closest ray hit.
	Vector3f closest;
	int closestDim;
	float distSqr;
	float tempDistSqr;
	bool foundFirst = false;
	for (int i = 0; i < 3; ++i)
	{
		if (bHits[i])
		{
			if (!foundFirst)
			{
				foundFirst = true;
				closest = hits[i].Point;
				distSqr = closest.DistanceSquared(rayStart);
				closestDim = i;
			}
			else
			{
				tempDistSqr = rayStart.DistanceSquared(hits[i].Point);
				if (tempDistSqr < distSqr)
				{
					closest = hits[i].Point;
					distSqr = tempDistSqr;
					closestDim = i;
				}
			}
		}
	}

	//If there were no hits, return a null result.
	if (!foundFirst) return RayTraceResult();


	//Otherwise, get the correct surface normal and return the ray hit.

	Vector3f normal;
	normal[closestDim] = (positives[closestDim] ? 1.0f : -1.0f);

	return RayTraceResult(hits[closestDim].Point, normal);
}

bool Sphere::TouchingCube(const Cube & cube) const
{
	Vector3f s1 = cube.GetBounds().GetTopLeftFront(),
			 s2 = cube.GetBounds().GetBottomRightBack(),
			 sph = GetCenter();
	float distSqr = BasicMath::Square(Radius);

	if (sph.x < s1.x) distSqr -= BasicMath::Square(sph.x - s1.x);
	else if (sph.x > s2.x) distSqr -= BasicMath::Square(sph.x - s2.x);
	if (sph.y < s1.y) distSqr -= BasicMath::Square(sph.y - s1.y);
	else if (sph.y > s2.y) distSqr -= BasicMath::Square(sph.y - s2.y);
	if (sph.z < s1.z) distSqr -= BasicMath::Square(sph.z - s1.z);
	else if (sph.z > s2.z) distSqr -= BasicMath::Square(sph.z - s2.z);

	return distSqr > 0.0f;
}
bool Sphere::TouchingCapsule(const Capsule & capsule) const
{
	Vector3f capPoint = GeometricMath::ClosestToLine(capsule.GetEndpoint1(), capsule.GetEndpoint2(), GetCenter(), false);

	return capPoint.DistanceSquared(GetCenter()) <= (BasicMath::Square(Radius) + BasicMath::Square(capsule.Radius));
}

Sphere::RayTraceResult Sphere::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
	Vector3f l = rayDir,
			 cent = GetCenter(),
			 o = rayStart;
	float r = Radius;

	Vector3f oc = o - cent;

	float a = l.Dot(l),
		  b = (oc * 2.0f).Dot(l),
		  c = (oc.Dot(oc)) - BasicMath::Square(r);

	float discriminant = BasicMath::Square(b) - (4.0f * a * c);

	//No roots.
	if (discriminant < 0.0f)
	{
		return RayTraceResult();
	}
	else
	{
		float denom = 1.0f / (2.0f * a);
		float t;
		
		//One root.
		if (discriminant == 0.0f)
		{
			t = -b * denom;
		}
		//Two roots.
		else
		{
			float sqt = sqrtf(discriminant);
			float t1 = -(b + sqt) * denom,
				  t2 = (sqt - b) * denom;
			if (t1 >= 0.0f && t1 < t2)
			{
				t = t1;
			}
			else
			{
				t = t2;
			}
		}

		if (t < 0.0f) return RayTraceResult();

		Vector3f p = (rayStart + (rayDir * t));
		return RayTraceResult(p, (p - cent).Normalized());
	}
}

bool Capsule::TouchingCube(const Cube & cube) const
{
	return false;
}
bool Capsule::TouchingSphere(const Sphere & sphere) const
{
	Vector3f capPoint = GeometricMath::ClosestToLine(l1, l2, sphere.GetCenter(), false);
	
	return capPoint.DistanceSquared(sphere.GetCenter()) <= (BasicMath::Square(sphere.Radius) + BasicMath::Square(Radius));
}
bool Capsule::TouchingCapsule(const Capsule & capsule) const
{
	GeometricMath::ClosestValues<Vector3f> cvs = GeometricMath::ClosestToIntersection(l1, l2, capsule.l1, capsule.l2, false);

	return cvs.OnFirstLine.DistanceSquared(cvs.OnSecondLine) <= (BasicMath::Square(Radius) + BasicMath::Square(capsule.Radius));
}

Capsule::RayTraceResult Capsule::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
	//No capsule ray hits yet.
	return RayTraceResult();
}