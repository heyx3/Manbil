#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include "../Lower Math/Vectors.h"
#include "../Lower Math/Mathf.h"
#include "../Higher Math/Geometryf.h"
#include "Boxes.h"


//Provides an interface for 3D shape collision detection and related geometric stuff.
//TODO: Replace this with Bullet physics engine.


class Cube;
class Sphere;
class Capsule;
class Plane;



//Represents some kind of 3D shape. Uses double dispatch for checking collision against other shapes.
class Shape
{
public:

    typedef std::shared_ptr<Shape> ShapePtr;


    Shape(Vector3f centerPos) : center(centerPos)
    { }


    //Gets the point on the shape's surface farthest in the given direction.
    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const = 0;

    virtual bool TouchingShape(const Shape& shape) const = 0;

    virtual bool TouchingCube(const Cube& cube) const = 0;
    virtual bool TouchingSphere(const Sphere& sphere) const = 0;
    virtual bool TouchingCapsule(const Capsule& capsule) const = 0;
    virtual bool TouchingPlane(const Plane& plane) const = 0;


    struct RayTraceResult
    {
        bool DidHitTarget;
        Vector3f HitPos;
        //HitPos = [ray start] + ([ray direction] * HitT)
        float HitT;
        Vector3f ReflectNormal;
        RayTraceResult(Vector3f hitPos, Vector3f refNormal, float hitT) : HitPos(hitPos), ReflectNormal(refNormal), HitT(hitT), DidHitTarget(true) { }
        RayTraceResult(bool didHitTarget = false) : DidHitTarget(didHitTarget) { }
    };
    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const = 0;

    virtual ShapePtr GetClone(void) const = 0;

    void MoveCenter(Vector3f deltaCenter) { SetCenter(center + deltaCenter); }

    virtual void SetCenter(Vector3f newCenter) { center = newCenter; }
    Vector3f GetCenter(void) const { return center; }

    virtual bool IsPointInside(Vector3f point) const = 0;

    virtual Box3D GetBoundingBox(void) const = 0;


private:

    Vector3f center;
};


class Cube : public Shape
{
public:

    //TODO: Add "Quaternion Rotation" here, then add Shape::Rotate(const Quaternion & rot), Shape::Transform(const Matrix4f & transform), and Shape::TouchingAABB(const Box3D & box).

    const Box3D & GetBounds(void) const { return Bounds; }

    Cube(Vector3f center, Vector3f dimensions) : Shape(center), Bounds(center, dimensions) { }
    Cube(const Box3D & toCopy) : Shape(toCopy.GetCenter()), Bounds(toCopy) { }


    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const override;

    virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingCube(*this); }

    virtual bool TouchingCube(const Cube & cube) const override { return Bounds.Touches(cube.Bounds); }
    virtual bool TouchingSphere(const Sphere & sphere) const override;
    virtual bool TouchingCapsule(const Capsule & capsule) const override;
    virtual bool TouchingPlane(const Plane & plane) const override;

    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

    virtual ShapePtr GetClone(void) const override { return ShapePtr(new Cube(Bounds.GetMinCorner(), Bounds.GetDimensions())); }

    virtual void SetCenter(Vector3f newCenter) override { Shape::SetCenter(newCenter); Bounds.SetCenterOfBox(newCenter); }
    void SetDimensions(Vector3f dimensions) { Bounds.SetDimensions(dimensions); }

    virtual bool IsPointInside(Vector3f point) const override
    {
        return Bounds.PointTouches(point);
    }

    void GetBoundingSphere(Sphere & sOut) const;

    virtual Box3D GetBoundingBox(void) const override { return Bounds; }

private:

    Box3D Bounds;
};
class Sphere : public Shape
{
public:

    float Radius;


    Sphere(Vector3f center, float radius) : Shape(center), Radius(radius) { }


    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const override
    {
        return GetCenter() + (dirNormalized * Radius);
    }

    virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingSphere(*this); }

    virtual bool TouchingCube(const Cube & cube) const override;
    virtual bool TouchingSphere(const Sphere & sphere) const override;
    virtual bool TouchingCapsule(const Capsule & capsule) const override;
    virtual bool TouchingPlane(const Plane & plane) const override;

    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

    virtual bool IsPointInside(Vector3f point) const override
    {
        return (point.DistanceSquared(GetCenter())) <= (Radius * Radius);
    }

    virtual ShapePtr GetClone(void) const override { return ShapePtr(new Sphere(GetCenter(), Radius)); }

    virtual Box3D GetBoundingBox(void) const override { return Box3D(GetCenter(), Vector3f(Radius, Radius, Radius) * 2.0f); }
};
class Capsule : public Shape
{
public:

    float Radius;

    Capsule(Vector3f _l1, Vector3f _l2, float radius)
        : Shape((_l1 + _l2) * 0.5f), Radius(radius), l1(_l1), l2(_l2)
    { }


    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const override;

    virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingCapsule(*this); }

    virtual bool TouchingCube(const Cube & cube) const override;
    virtual bool TouchingSphere(const Sphere & sphere) const override;
    virtual bool TouchingCapsule(const Capsule & capsule) const override;
    virtual bool TouchingPlane(const Plane & plane) const override;

    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

    void SetEndpoint1(Vector3f newL1) { l1 = newL1; RecomputeCenter(); }
    void SetEndpoint2(Vector3f newL2) { l2 = newL2; RecomputeCenter(); }

    void Translate(Vector3f amount) { l1 += amount; l2 += amount; RecomputeCenter(); }

    Vector3f GetEndpoint1(void) const { return l1; }
    Vector3f GetEndpoint2(void) const { return l2; }

    Vector3f GetParallelVector(void) const { return (l2 - l1).Normalized(); }
    float GetLength(void) const { return l2.Distance(l1) + Radius; }

    virtual void SetCenter(Vector3f center) override
    {
        Vector3f delta = center - GetCenter();
        l1 += delta;
        l2 += delta;
        Shape::SetCenter(center);
    }

    virtual bool IsPointInside(Vector3f point) const override
    {
        return point.DistanceSquared(Geometryf::ClosestToLine(l1, l2, point, false)) <=
            (Radius * Radius);
    }

    virtual ShapePtr GetClone(void) const override { return ShapePtr(new Capsule(l1, l2, Radius)); }

    virtual Box3D GetBoundingBox(void) const override;

private:

    void RecomputeCenter(void) { Shape::SetCenter((l1 + l2) * 0.5f); }

    Vector3f l1, l2;
};
class Plane : public Shape
{
public:


    //The margin of error for certain plane operations such as "IsPointInside()".
    static const float MarginOfError;

    Vector3f Normal;

    Plane(Vector3f onPlane, Vector3f normal) : Shape(onPlane), Normal(normal) { }

    //Gets the signed distance from the given point to this plane.
    float GetDistanceToPlane(Vector3f point) const
    {
        return (GetCenter() - point).Dot(Normal);
    }

    struct PointOnPlaneInfo { public: Vector3f OnPlane; float Distance; };
    //Gets the point on this Plane closest to the given point, as well as
    //   the signed distance from the given point to this plane.
    PointOnPlaneInfo GetClosestPointOnPlane(Vector3f point) const
    {
        PointOnPlaneInfo ret;
        ret.Distance = GetDistanceToPlane(point);
        ret.OnPlane = point + (Normal * ret.Distance);
        return ret;
    }


    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const override
    {
        //The plane is infinite, so this gets a little weird.

        //If the direction is along the plane, return a value in the right direction.
        float dot = dirNormalized.Dot(Normal);
        if (Mathf::Abs(dot) < MarginOfError)
        {
            return GetCenter() + (dirNormalized * std::numeric_limits<float>::infinity());
        }

        //Otherwise, the only viable value is the plane center.
        return GetCenter();
    }

    virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingPlane(*this); }

    virtual bool TouchingCube(const Cube & cube) const override;
    virtual bool TouchingSphere(const Sphere & sphere) const override;
    virtual bool TouchingCapsule(const Capsule & capsule) const override;
    virtual bool TouchingPlane(const Plane & plane) const override;

    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

    virtual bool IsPointInside(Vector3f point) const override
    {
        return Mathf::Abs(GetDistanceToPlane(point)) <= MarginOfError;
    }

    virtual ShapePtr GetClone(void) const override { return ShapePtr(new Plane(GetCenter(), Normal)); }

    virtual Box3D GetBoundingBox(void) const override;
};