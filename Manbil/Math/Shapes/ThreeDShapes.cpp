#include "ThreeDShapes.h"

#include <assert.h>


bool Shape::TouchingPolygon(const PolygonSolid & poly) const
{
    const Shape * shpe = this;
    return std::any_of(poly.GetTriangles().begin(), poly.GetTriangles().end(),
                       [shpe](const Triangle & tri) { return shpe->TouchingTriangle(tri); });
}


Vector3f Cube::FarthestPointInDirection(Vector3f dirNormalized) const
{
    Vector3f absDirNormalized(BasicMath::Abs(dirNormalized.x) / Bounds.GetXSize(),
                              BasicMath::Abs(dirNormalized.y) / Bounds.GetYSize(),
                              BasicMath::Abs(dirNormalized.z) / Bounds.GetZSize());

    //On X face.
    if (absDirNormalized.x >= absDirNormalized.y && absDirNormalized.x >= absDirNormalized.z)
    {
        return GeometricMath::GetPointOnLineAtValue(GetCenter(), dirNormalized, 0,
                                                    Bounds.GetCenterX() +
                                                    (BasicMath::Sign(dirNormalized.x) * 0.5f *
                                                     Bounds.GetXSize())).Point;
    }
    //On Y face.
    else if (absDirNormalized.y >= absDirNormalized.x && absDirNormalized.y >= absDirNormalized.z)
    {
        return GeometricMath::GetPointOnLineAtValue(GetCenter(), dirNormalized, 1,
                                                    Bounds.GetCenterY() +
                                                    (BasicMath::Sign(dirNormalized.y) * 0.5f *
                                                     Bounds.GetYSize())).Point;
    }
    //On Z face.
    else
    {
        assert(absDirNormalized.z > absDirNormalized.x && absDirNormalized.z > absDirNormalized.y);
        return GeometricMath::GetPointOnLineAtValue(GetCenter(), dirNormalized, 2,
                                                    Bounds.GetCenterZ() +
                                                    (BasicMath::Sign(dirNormalized.z) * 0.5f *
                                                     Bounds.GetZSize())).Point;
    }
}

bool Cube::TouchingSphere(const Sphere & sphere) const
{
    //Get the closest point in/on the cube to the sphere's center and see if it's inside the sphere.
    Vector3f clampedToCube = Vector3f(BasicMath::Clamp(sphere.GetCenter().x,
                                                       Bounds.GetXMin(), Bounds.GetXMax()),
                                      BasicMath::Clamp(sphere.GetCenter().y,
                                                       Bounds.GetYMin(), Bounds.GetYMax()),
                                      BasicMath::Clamp(sphere.GetCenter().z,
                                                       Bounds.GetZMin(), Bounds.GetZMax()));
    return sphere.IsPointInside(clampedToCube);
}
bool Cube::TouchingCapsule(const Capsule & capsule) const
{
    //GJK algorithm. http://vec3.ca/gjk/implementation/
    //TODO: Abstract out to GeometricMath, or even a standalone class.

    if (GetCenter() == capsule.GetCenter()) return true;

    const Cube * cbe = this;
    const Capsule & caps = capsule;
    auto supportFunc = [cbe, caps](Vector3f dir) -> Vector3f
    {
        return cbe->FarthestPointInDirection(dir) - caps.FarthestPointInDirection(-dir);
    };
    auto crossABA = [](Vector3f a, Vector3f b) -> Vector3f { return a.Cross(b).Cross(a); };
    Vector3f b, c, d,
        v;
    int n;

    auto DoSimplex = [cbe, caps, &b, &c, &d, &v, &n, crossABA](Vector3f a) -> bool
    {
        if (n == 0)
        {
            b = a;
            v = -a;
            n = 1;
        }
        else if (n == 1)
        {
            v = crossABA(b - a, -a);

            c = b;
            b = a;
            n = 2;
        }
        else if (n == 2)
        {
            Vector3f ao = -a,
                ab = b - a,
                ac = c - a;

            Vector3f abc = ab.Cross(ac),
                abp = ab.Cross(abc);

            if (abp.Dot(ao) > 0.0f)
            {
                c = b;
                b = a;
                v = crossABA(ab, ao);
            }
            else
            {
                Vector3f acp = abc.Cross(ac);
                if (acp.Dot(ao) > 0.0f)
                {
                    b = a;
                    v = crossABA(ac, ao);
                }
                else
                {
                    n = 3;

                    if (abc.Dot(ao) > 0.0f)
                    {
                        d = c;
                        c = b;
                        b = a;

                        v = abc;
                    }
                    else
                    {
                        d = b;
                        b = a;

                        v = -abc;
                    }
                }
            }
        }
        else if (n == 3)
        {
            Vector3f ao = -a,
                ab = b - a,
                ac = c - a;
            Vector3f abc = ab.Cross(ac);
            Vector3f ad, acd, adb;

            if (abc.Dot(ao) > 0.0f)
            {
                goto checkFace;
            }

            ad = d - a;
            acd = ac.Cross(ad);
            if (acd.Dot(ao) > 0.0f)
            {
                b = c;
                c = d;

                ab = ac;
                ac = ad;
                abc = acd;

                goto checkFace;
            }

            adb = ad.Cross(ab);
            if (adb.Dot(ao) > 0.0f)
            {
                c = b;
                b = d;

                ac = ab;
                ab = ad;

                abc = adb;

                goto checkFace;
            }

            return true;

        checkFace:

            Vector3f abp = ab.Cross(abc);

            if (abp.Dot(ao) > 0)
            {
                c = b;
                b = a;

                v = crossABA(ab, ao);

                n = 2;
            }
            else
            {
                Vector3f acp = abc.Cross(ac);
                if (acp.Dot(ao) > 0.0f)
                {
                    b = a;

                    v = crossABA(ac, ao);

                    n = 2;
                }
                else
                {
                    d = c;
                    c = b;
                    b = a;

                    v = abc;

                    n = 3;
                }
            }
        }

        return false;
    };

    v = Vector3f(1.0f, 0.0f, 0.0f);
    n = 0;

    //Normally you should loop infinitely, but thanks to floating point error,
    //    we might get caught in an infinte loop.
    for (unsigned int i = 0; i < 10; ++i)
    {
        Vector3f a = supportFunc(v);

        if (a.Dot(v) < 0.0f) return false;

        if (DoSimplex(a)) return true;

        v.Normalize();
    }

    //If the loop never managed to exit, it's very likely that this was an intersection.
    return true;












    //Use GJK algorithm.
    /*
    const Cube * cbe = this;
    const Capsule & caps = capsule;
    auto supportFunc = [cbe, caps](Vector3f dir) -> Vector3f
    {
    return cbe->FarthestPointInDirection(dir) - caps.FarthestPointInDirection(-dir);
    };

    auto DoSimplex = [cbe, caps](Vector3f * simplex, int * nVertices, Vector3f * dir) -> bool
    {
    if (*nVertices == 2)
    {
    Vector3f a = simplex[1],
    b = simplex[0];
    Vector3f a_origin = -a,
    a_b = b - a;
    if (a_origin.Dot(a_b) > 0.0f)
    {
    simplex[0] = a;
    simplex[1] = b;
    *nVertices = 2;
    *dir = a_b.Cross(a_origin).Cross(a_b);
    }
    else
    {
    simplex[0] = a;
    *nVertices = 1;
    *dir = a_origin;
    }
    }
    else if (*nVertices == 3)
    {
    Vector3f a = simplex[2],
    b = simplex[0],
    c = simplex[1];
    Vector3f a_origin = -a,
    a_b = b - a,
    a_c = c - a,
    abc = a_b.Cross(a_c),
    a_b_cross = a_b.Cross(abc),
    a_c_cross = abc.Cross(a_c);
    if (a_c_cross.Dot(a_origin) > 0.0f)
    {
    if (a_c.Dot(a_origin) > 0.0f)
    {
    simplex[0] = a;
    simplex[1] = c;
    *nVertices = 2;
    *dir = a_c.Cross(a_origin).Cross(a_c);
    }
    else
    {
    if (a_b.Dot(a_origin) > 0.0f)
    {
    simplex[0] = a;
    simplex[1] = b;
    *nVertices = 2;
    *dir = a_b.Cross(a_origin).Cross(a_b);
    }
    else
    {
    simplex[0] = a;
    *nVertices = 1;
    *dir = a_origin;
    }
    }
    }
    else
    {
    if (a_b_cross.Dot(a_origin) > 0.0f)
    {
    if (a_b.Dot(a_origin) > 0.0f)
    {
    simplex[0] = a;
    simplex[1] = b;
    *nVertices = 2;
    *dir = a_b.Cross(a_origin).Cross(a_b);
    }
    else
    {
    simplex[0] = a;
    *nVertices = 1;
    *dir = a_origin;
    }
    }
    else
    {
    if (abc.Dot(a_origin) > 0.0f)
    {
    simplex[0] = a;
    simplex[1] = b;
    simplex[2] = c;
    *nVertices = 3;
    *dir = abc;
    }
    else
    {
    simplex[0] = a;
    simplex[1] = c;
    simplex[2] = b;
    *nVertices = 3;
    *dir = -abc;
    }
    }
    }
    }
    else if (*nVertices == 4)
    {
    //Sorry about the "gotos", but this function is already a nightmare.
    Vector3f a = simplex[3],
    b = simplex[2],
    c = simplex[1],
    d = simplex[0];
    Vector3f a_origin = -a,
    a_b = b - a,
    a_c = c - a,
    abc = a_b.Cross(a_c);
    Vector3f a_d, acd, adb;

    if (abc.Dot(a_origin) > 0.0f)
    goto checkFace;

    a_d = d - a;
    acd = a_c.Cross(a_d);
    if (acd.Dot(a_origin) > 0.0f)
    {
    b = c;
    c = d;
    a_b = a_c;
    a_c = a_d;

    abc = acd;

    goto checkFace;
    }

    adb = a_d.Cross(a_b);
    if (adb.Dot(a_origin) > 0.0f)
    {
    c = b;
    b = d;
    a_c = a_b;
    a_b = a_d;

    abc = adb;

    goto checkFace;
    }

    dir->Normalize();
    return true;

    checkFace:

    //We have a CCW-wound triangle ABC. The point is in front of it.
    //It is NOT "below" BC or "above" the plane through A parallel to BC.

    Vector3f abp = a_b.Cross(abc);

    if (abp.Dot(a_origin) > 0.0f)
    {
    simplex[1] = a;
    simplex[0] = b;
    *nVertices = 2;
    *dir = a_b.Cross(a_origin);

    return false;
    }

    Vector3f acp = abc.Cross(a_c);
    if (acp.Dot(a_origin) > 0.0f)
    {
    simplex[0] = c;
    simplex[1] = a;
    *nVertices = 2;
    *dir = a_c.Cross(a_origin);

    return false;
    }

    simplex[2] = a;
    simplex[1] = b;
    simplex[0] = c;
    *nVertices = 3;
    *dir = abc;

    return false;
    }

    return false;
    };

    Vector3f startDir(1.0f, 0.0f, 0.0f);
    Vector3f seedValue = supportFunc(startDir);
    Vector3f simplex[4];
    simplex[0] = seedValue;
    int nVerts = 1;
    Vector3f dir = -seedValue;

    while (true)
    {
    Vector3f a = supportFunc(dir);

    if (a.Dot(dir) < 0.0f)
    return false;

    simplex[nVerts] = a;
    nVerts += 1;

    if (DoSimplex(simplex, &nVerts, &dir))
    return true;
    }








    return false;


    Vector3f a = capsule.GetEndpoint1(),
    b = capsule.GetEndpoint2();
    Vector3f min = Bounds.GetMinCorner(),
    max = Bounds.GetMaxCorner();
    float r = capsule.Radius;

    if (BasicMath::Max(a.y, b.y) < min.y || BasicMath::Min(a.y, b.y) > max.y)
    {
    if (BasicMath::Abs(a.x - max.x) < r || BasicMath::Abs(a.x - min.x) < r ||
    BasicMath::Abs(a.z - max.z) < r || BasicMath::Abs(a.z - min.z) < r)
    {
    return true;
    }
    else return false;
    }
    else return (capsule.TouchingSphere(Sphere(a, r)) || capsule.TouchingSphere(Sphere(b, r)));



    return false;

    //Test a collision for each of the cube's six planes.

    Vector3f center = GetCenter();
    Vector3f halfDims = Bounds.GetDimensions() * 0.5f;

    for (float sign = -1.0f; sign <= 1.0f; sign += 2.0f)
    {
    for (int axis = 0; axis < 3; ++axis)
    {
    int axis1 = (axis + 1) % 3,
    axis2 = (axis + 2) % 3;

    //Calculate the plane.
    Vector3f planePos = center,
    planeNormal = Vector3f();
    planePos[axis] = center[axis] + (sign * halfDims[axis]);
    planeNormal[axis] = sign;
    Plane pl(planePos, planeNormal);

    //If the capsule touches the plane, do more specific tests.
    if (capsule.TouchingPlane(pl))
    {
    //Get the capsule line's intersection with the plane.

    float distL1 = BasicMath::Abs(pl.GetDistanceToPlane(capsule.GetEndpoint1())),
    distL2 = BasicMath::Abs(pl.GetDistanceToPlane(capsule.GetEndpoint2()));
    Vector3f rayStart, rayEnd;
    if (distL1 > 0.0f)
    {
    if (distL2 > distL1)
    {
    rayStart = capsule.GetEndpoint2();
    rayEnd = capsule.GetEndpoint1();
    }
    else
    {
    rayStart = capsule.GetEndpoint1();
    rayEnd = capsule.GetEndpoint2();
    }
    }
    else
    {
    if (distL2 > distL1)
    {
    rayStart = capsule.GetEndpoint1();
    rayEnd = capsule.GetEndpoint2();
    }
    else
    {
    rayStart = capsule.GetEndpoint2();
    rayEnd = capsule.GetEndpoint1();
    }
    }

    RayTraceResult intersect = pl.RayHitCheck(rayStart, (rayEnd - rayStart).Normalized());


    }
    }

    faces = center + (Bounds.GetDimensions() * sign);

    //X plane.
    if (capsule.TouchingPlane(Plane(Vector3f(faces.x, center.y, center.z), Vector3f(sign * 1.0f, 0.0f, 0.0f))))
    {

    }

    //Y plane.
    if (capsule.TouchingPlane(Plane(Vector3f(center.x, faces.y, center.z), Vector3f(0.0f, sign * 1.0f, 0.0f))))
    {

    }

    //Z plane.
    if (capsule.TouchingPlane(Plane(Vector3f(center.x, center.y, faces.z), Vector3f(0.0f, 0.0f, sign * 1.0f))))
    {

    }
    }
    */
}
bool Cube::TouchingPlane(const Plane & plane) const
{
    //Taken from http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?print=1, section "A Box-Plane Intersection Test".

    //If this function gets remade for a box that isn't axis-aligned, use
    //    "BasicMath::Abs(plane.Normal.Dot([normalized axis vector])"
    //    for each axis rotated from box space to world space.
    float val = (Bounds.GetXSize() * BasicMath::Abs(plane.Normal.x)) +
                (Bounds.GetYSize() * BasicMath::Abs(plane.Normal.y)) +
                (Bounds.GetZSize() * BasicMath::Abs(plane.Normal.z));
    val *= 0.5f;

    return BasicMath::Abs(plane.GetDistanceToPlane(GetCenter())) <= val;
}

#pragma warning(disable: 4100)
bool Cube::TouchingTriangle(const Triangle & tris) const
{
    return false;
}
#pragma warning(default: 4100)

Cube::RayTraceResult Cube::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
    typedef GeometricMath::PointOnLineAtValueResult<Vector3f> PointOnFaces;

    Vector3f center = Bounds.GetCenter();
    Interval dInts[3] = { Bounds.GetXInterval(), Bounds.GetYInterval(), Bounds.GetZInterval() };

    //Get the closest x, y, and z faces. If the vector is pointing parallel to a face,
    //    there is no intersection along that face.
    Vector3f faces((rayDir.x == 0.0f) ?
                       BasicMath::NaN :
                       ((rayDir.x > 0.0f) ?
                           (rayStart.x > Bounds.GetXMin() ? Bounds.GetXMax() : Bounds.GetXMin()) :
                           (rayStart.x < Bounds.GetXMax() ? Bounds.GetXMin() : Bounds.GetXMax())),
                   (rayDir.y == 0.0f) ?
                       BasicMath::NaN :
                       ((rayDir.y > 0.0f) ?
                           (rayStart.y > Bounds.GetYMin() ? Bounds.GetYMax() : Bounds.GetYMin()) :
                           (rayStart.y < Bounds.GetYMax() ? Bounds.GetYMin() : Bounds.GetYMax())),
                   (rayDir.z == 0.0f) ?
                       BasicMath::NaN :
                       ((rayDir.z > 0.0f) ?
                           (rayStart.z > Bounds.GetZMin() ? Bounds.GetZMax() : Bounds.GetZMin()) :
                           (rayStart.z < Bounds.GetZMax() ? Bounds.GetZMin() : Bounds.GetZMax())));

    //For each face, get the ray's intersection with that face.
    PointOnFaces faceIntersectData[3] =
    {
        (faces.x == BasicMath::NaN ?
            PointOnFaces(Vector3f(), -1.0f) :
            GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, 0, faces.x)),
        (faces.y == BasicMath::NaN ?
            PointOnFaces(Vector3f(), -1.0f) :
            GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, 1, faces.y)),
        (faces.z == BasicMath::NaN ?
            PointOnFaces(Vector3f(), -1.0f) :
            GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, 2, faces.z)),
    };

    //Get the closest intersection.
    //Ignore any intersections that are behind the ray start or outside the face's bounds.
    RayTraceResult res;
    float closestDistSqr = std::numeric_limits<float>::max();
    int closestAxis = 0;
    float tempDistSqre;
    for (unsigned int axis = 0; axis < 3; ++axis)
    {
        if (faceIntersectData[axis].t > 0.0f &&
            dInts[(axis + 1) % 3].Touches(faceIntersectData[axis].Point[(axis + 1) % 3]) &&
            dInts[(axis + 2) % 3].Touches(faceIntersectData[axis].Point[(axis + 2) % 3]))
        {
            tempDistSqre = rayStart.DistanceSquared(faceIntersectData[axis].Point);
            if (tempDistSqre < closestDistSqr)
            {
                res = RayTraceResult(faceIntersectData[axis].Point, Vector3f(), faceIntersectData[axis].t);
                closestDistSqr = tempDistSqre;
                closestAxis = axis;
            }
        }
    }
    res.ReflectNormal[closestAxis] =
        (float)BasicMath::Sign(res.HitPos[closestAxis] - GetCenter()[closestAxis]);

    return res;
}


bool Sphere::TouchingCube(const Cube & cube) const
{
    //Get the closest point in/on the cube to the sphere's center and see if it's inside the sphere.
    const Box3D & bounds = cube.GetBounds();
    Vector3f clampedToCube = Vector3f(BasicMath::Clamp(GetCenter().x, bounds.GetXMin(), bounds.GetXMax()),
                                      BasicMath::Clamp(GetCenter().y, bounds.GetYMin(), bounds.GetYMax()),
                                      BasicMath::Clamp(GetCenter().z, bounds.GetZMin(), bounds.GetZMax()));
    return IsPointInside(clampedToCube);
}
bool Sphere::TouchingCapsule(const Capsule & capsule) const
{
    Vector3f capPoint = GeometricMath::ClosestToLine(capsule.GetEndpoint1(), capsule.GetEndpoint2(),
                                                     GetCenter(), false);

    return capPoint.DistanceSquared(GetCenter()) <=
           ((Radius + capsule.Radius) * (Radius + capsule.Radius));
}
bool Sphere::TouchingSphere(const Sphere& sphere) const
{
    float radSqr = sphere.Radius + Radius;
    radSqr *= radSqr;
    return GetCenter().DistanceSquared(sphere.GetCenter()) <= radSqr;
}
bool Sphere::TouchingPlane(const Plane & plane) const
{
    //Get the min/max distances along the plane normal that would make the sphere touch the cube.

    Vector3f dirRad = plane.Normal * Radius;
    Vector3f min = GetCenter() - dirRad,
        max = GetCenter() + dirRad;
    float minDist = min.Dot(plane.Normal),
        maxDist = max.Dot(plane.Normal);

    float dist = plane.GetCenter().Dot(plane.Normal);

    return dist >= minDist && dist <= maxDist;
}

#pragma warning(disable: 4100)
bool Sphere::TouchingTriangle(const Triangle & tris) const
{
    return false;
}
#pragma warning(default: 4100)

Sphere::RayTraceResult Sphere::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
    Vector3f cent = GetCenter();
    float r = Radius;

    Vector3f centerToRayStart = rayStart - cent;

    float rayDirSqr = rayDir.Dot(rayDir),
        b = (centerToRayStart * 2.0f).Dot(rayDir),
        c = (centerToRayStart.Dot(centerToRayStart)) - (r * r);

    float discriminant = (b * b) - (4.0f * rayDirSqr * c);

    //No roots.
    if (discriminant < 0.0f)
    {
        return RayTraceResult();
    }
    else
    {
        float denom = 1.0f / (2.0f * rayDirSqr);
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
        return RayTraceResult(p, (p - cent).Normalized(), t);
    }
}


Vector3f Capsule::FarthestPointInDirection(Vector3f dirNormalized) const
{
    return RayHitCheck(GetCenter() + (dirNormalized * (l2 - l1).LengthSquared()), -dirNormalized).HitPos;
}

bool Capsule::TouchingCube(const Cube & cube) const
{
    if (cube.GetCenter() == GetCenter()) return true;

    const Cube & cbe = cube;
    const Capsule * caps = this;
    auto supportFunc = [cbe, caps](Vector3f dir) -> Vector3f
    {
        return cbe.FarthestPointInDirection(dir) - caps->FarthestPointInDirection(-dir);
    };
    auto crossABA = [](Vector3f a, Vector3f b) -> Vector3f { return a.Cross(b).Cross(a); };
    Vector3f b, c, d,
        v;
    int n;

    auto DoSimplex = [cbe, caps, &b, &c, &d, &v, &n, crossABA](Vector3f a) -> bool
    {
        if (n == 0)
        {
            b = a;
            v = -a;
            n = 1;
        }
        else if (n == 1)
        {
            v = crossABA(b - a, -a);

            c = b;
            b = a;
            n = 2;
        }
        else if (n == 2)
        {
            Vector3f ao = -a,
                ab = b - a,
                ac = c - a;

            Vector3f abc = ab.Cross(ac),
                abp = ab.Cross(abc);

            if (abp.Dot(ao) > 0.0f)
            {
                c = b;
                b = a;
                v = crossABA(ab, ao);
            }
            else
            {
                Vector3f acp = abc.Cross(ac);
                if (acp.Dot(ao) > 0.0f)
                {
                    b = a;
                    v = crossABA(ac, ao);
                }
                else
                {
                    n = 3;

                    if (abc.Dot(ao) > 0.0f)
                    {
                        d = c;
                        c = b;
                        b = a;

                        v = abc;
                    }
                    else
                    {
                        d = b;
                        b = a;

                        v = -abc;
                    }
                }
            }
        }
        else if (n == 3)
        {
            Vector3f ao = -a,
                ab = b - a,
                ac = c - a;
            Vector3f abc = ab.Cross(ac);
            Vector3f ad, acd, adb;

            if (abc.Dot(ao) > 0.0f)
            {
                goto checkFace;
            }

            ad = d - a;
            acd = ac.Cross(ad);
            if (acd.Dot(ao) > 0.0f)
            {
                b = c;
                c = d;

                ab = ac;
                ac = ad;
                abc = acd;

                goto checkFace;
            }

            adb = ad.Cross(ab);
            if (adb.Dot(ao) > 0.0f)
            {
                c = b;
                b = d;

                ac = ab;
                ab = ad;

                abc = adb;

                goto checkFace;
            }

            return true;

        checkFace:

            Vector3f abp = ab.Cross(abc);

            if (abp.Dot(ao) > 0)
            {
                c = b;
                b = a;

                v = crossABA(ab, ao);

                n = 2;
            }
            else
            {
                Vector3f acp = abc.Cross(ac);
                if (acp.Dot(ao) > 0.0f)
                {
                    b = a;

                    v = crossABA(ac, ao);

                    n = 2;
                }
                else
                {
                    d = c;
                    c = b;
                    b = a;

                    v = abc;

                    n = 3;
                }
            }
        }

        return false;
    };

    v = Vector3f(1.0f, 0.0f, 0.0f);
    n = 0;

    //Normally you should loop infinitely, but thanks to floating point error,
    //    we might get caught in an infinte loop.
    for (unsigned int i = 0; i < 10; ++i)
    {
        Vector3f a = supportFunc(v);

        if (a.Dot(v) < 0.0f) return false;

        if (DoSimplex(a)) return true;

        v.Normalize();
    }

    //If the loop never managed to exit, it's very likely that this was an intersection.
    return true;
}
bool Capsule::TouchingSphere(const Sphere & sphere) const
{
    Vector3f capPoint = GeometricMath::ClosestToLine(l1, l2, sphere.GetCenter(), false);

    return capPoint.DistanceSquared(sphere.GetCenter()) <=
               ((sphere.Radius + Radius) * (sphere.Radius + Radius));
}
bool Capsule::TouchingCapsule(const Capsule & capsule) const
{
    GeometricMath::ClosestValues<Vector3f> cvs =
        GeometricMath::ClosestToIntersection(l1, l2, capsule.l1, capsule.l2, false);

    return cvs.OnFirstLine.DistanceSquared(cvs.OnSecondLine) <=
        ((Radius * Radius) + (capsule.Radius * capsule.Radius));
}
bool Capsule::TouchingPlane(const Plane & plane) const
{
    float distance1 = plane.GetDistanceToPlane(l1),
        distance2 = plane.GetDistanceToPlane(l2);

    Interval distanceIntvl(distance1, distance2, Plane::MarginOfError, true, true);
    distanceIntvl = distanceIntvl.Widen(Radius + Radius);

    return distanceIntvl.Touches(0.0f);
}

#pragma warning(disable: 4100)
bool Capsule::TouchingTriangle(const Triangle & tris) const
{
    return false;
}
#pragma warning(default: 4100)

Capsule::RayTraceResult Capsule::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
    //TODO: The ray fails if cast through the ends of the capsule.

    //Taken from http://blog.makingartstudios.com/?p=286

    //l1 is 'a', l2 is 'b'.
    Vector3f aToB = l2 - l1;
    Vector3f aToO = rayStart - l1;
    float aToB_dot_aToB = aToB.Dot(aToB);
    float m = aToB.Dot(rayDir) / aToB_dot_aToB,
        n = aToB.Dot(aToO) / aToB_dot_aToB;

    Vector3f q = rayDir - (aToB * m),
        r = aToO - (aToB * n);


    //Quadratic formula. The solutions are the 't' values for
    //    the ray's equation that correspond to intersections.

    float a = q.Dot(q),
        b = 2.0f * q.Dot(r),
        c = r.Dot(r) - (Radius * Radius);

    float determinant = (b * b) - (4.0f * a * c);
    if (determinant < 0.0f) return RayTraceResult();

    float detSqrt = sqrtf(determinant),
        otherNominator = -b,
        divDenominator = 0.5f / a;
    float t1 = (otherNominator + detSqrt) * divDenominator,
        t2 = (otherNominator - detSqrt) * divDenominator;


    //These intersections are actually for the infinite cylinder that this capsule is a subset of.
    //See if any of the intersections are actually on the capsule.

    RayTraceResult intersect1, intersect2;
    float capsuleT1 = (t1 * m) + n,
        capsuleT2 = (t2 * m) + n;

    if (capsuleT1 < 0.0f)
    {
        intersect1 = Sphere(l1, Radius).RayHitCheck(rayStart, rayDir);
    }
    else if (capsuleT1 > 1.0f)
    {
        intersect1 = Sphere(l2, Radius).RayHitCheck(rayStart, rayDir);
    }
    else if (t1 < 0.0f)
    {
        intersect1 = RayTraceResult();
    }
    else
    {
        Vector3f point = rayStart + (rayDir * t1);
        intersect1 = RayTraceResult(point, (point - (l1 + (aToB * capsuleT1))).Normalized(), t1);
    }

    if (capsuleT2 < 0.0f)
    {
        intersect2 = Sphere(l1, Radius).RayHitCheck(rayStart, rayDir);
    }
    else if (capsuleT2 > 1.0f)
    {
        intersect2 = Sphere(l2, Radius).RayHitCheck(rayStart, rayDir);
    }
    else if (t2 < 0.0f)
    {
        intersect2 = RayTraceResult();
    }
    else
    {
        Vector3f point = rayStart + (rayDir * t2);
        intersect2 = RayTraceResult(point, (point - (l1 + (aToB * capsuleT2))).Normalized(), t2);
    }


    //Note that if there was only one intersection, "intersect1" and "intersect2"
    //    will both contain that intersection.
    if ((!intersect1.DidHitTarget && !intersect2.DidHitTarget) ||
        intersect1.HitPos.DistanceSquared(rayStart) < intersect2.HitPos.DistanceSquared(rayStart))
        return intersect1;
    else return intersect2;
}

Box3D Capsule::GetBoundingBox(void) const
{
    return Box3D(BasicMath::Min(l1.x, l2.x) - Radius,
                 BasicMath::Max(l1.x, l2.x) + Radius,
                 BasicMath::Min(l1.y, l2.y) - Radius,
                 BasicMath::Max(l1.y, l2.y) + Radius,
                 BasicMath::Min(l1.z, l2.z) - Radius,
                 BasicMath::Max(l1.z, l2.z) + Radius);
}


const float Plane::MarginOfError = 0.001f;

bool Plane::TouchingCube(const Cube & cube) const
{
    //Refer to Cube::TouchingPlane(const Plane & plane) for the info about this formula.

    const Box3D & bounds = cube.GetBounds();

    float val = bounds.GetXSize() * BasicMath::Abs(Normal.x) +
        bounds.GetYSize() * BasicMath::Abs(Normal.y) +
        bounds.GetZSize() * BasicMath::Abs(Normal.z);
    val *= 0.5f;

    return BasicMath::Abs(GetDistanceToPlane(cube.GetCenter())) <= val;
}
bool Plane::TouchingSphere(const Sphere & sphere) const
{
    //Get the min/max distances along the plane normal that would make the sphere touch the cube.

    Vector3f dirRad = Normal * sphere.Radius;
    Vector3f min = sphere.GetCenter() - dirRad,
        max = sphere.GetCenter() + dirRad;
    float minDist = min.Dot(Normal),
        maxDist = max.Dot(Normal);

    float dist = GetCenter().Dot(Normal);

    return dist >= minDist && dist <= maxDist;
}
bool Plane::TouchingCapsule(const Capsule & capsule) const
{
    float distance1 = GetDistanceToPlane(capsule.GetEndpoint1()),
        distance2 = GetDistanceToPlane(capsule.GetEndpoint2());

    Interval distanceIntvl(distance1, distance2, MarginOfError, true, true);
    distanceIntvl = distanceIntvl.Widen(capsule.Radius + capsule.Radius);

    return distanceIntvl.Touches(0.0f);
}
bool Plane::TouchingPlane(const Plane & plane) const
{
    float dot = BasicMath::Round(Normal.Dot(plane.Normal), 2);

    return (dot != 1.0f && dot != -1.0f) ||
        (plane.GetCenter().DistanceSquared(GetCenter()) <= MarginOfError);
}

#pragma warning(disable: 4100)
bool Plane::TouchingTriangle(const Triangle & tris) const
{
    return false;
}
#pragma warning(default: 4100)

Plane::RayTraceResult Plane::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
    float denominator = rayDir.Dot(Normal);

    //If ray is perpendicular to plane, no intersection.
    if (BasicMath::Abs(denominator) <= MarginOfError) return RayTraceResult();

    float t = Normal.Dot(GetCenter() - rayStart) / denominator;
    if (t < 0.0f) return RayTraceResult();

    return RayTraceResult(rayStart + (rayDir * t),
                          Normal * (float)BasicMath::Sign(GetDistanceToPlane(rayStart)), t);
}

Box3D Plane::GetBoundingBox(void) const
{
    const float min = std::numeric_limits<float>::min(),
        max = std::numeric_limits<float>::max();

    //If the plane is aligned along an axis, one of the box's dimensions will have 0 size.
    bool xAligned = (1.0f - BasicMath::Abs(Normal.x)) < MarginOfError,
        yAligned = (1.0f - BasicMath::Abs(Normal.y)) < MarginOfError,
        zAligned = (1.0f - BasicMath::Abs(Normal.z)) < MarginOfError;

    return Box3D(xAligned ? GetCenter().x : min, xAligned ? GetCenter().x : max,
                 yAligned ? GetCenter().y : min, yAligned ? GetCenter().y : max,
                 zAligned ? GetCenter().z : min, zAligned ? GetCenter().z : max);
}


Box3D PolygonSolid::GetBoundingBox(void) const
{
    if (triangles.size() == 0) return Box3D();

    Vector3f min = triangles[0].GetVertices()[0],
        max = triangles[0].GetVertices()[0];

    std::_For_each(triangles.begin(), triangles.end(),
                   [&min, &max](const Triangle & tri)
    {
        const Vector3f * vertices = tri.GetVertices();
        for (int i = 0; i < 3; ++i)
        {
            if (vertices[i].x < min.x)
                min.x = vertices[i].x;
            if (vertices[i].x > max.x)
                max.x = vertices[i].x;

            if (vertices[i].y < min.y)
                min.y = vertices[i].y;
            if (vertices[i].y > max.y)
                max.y = vertices[i].y;

            if (vertices[i].z < min.z)
                min.z = vertices[i].z;
            if (vertices[i].z > max.z)
                max.z = vertices[i].z;
        }
    });

    return Box3D(min.x, max.x, min.y, max.y, min.z, max.z);
}