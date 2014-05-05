#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include "../Vectors.h"
#include "../BasicMath.h"
#include "../Higher Math/GeometricMath.h"
#include "Boxes.h"


class Cube;
class Sphere;
class Capsule;
class Plane;
class Triangle;
class PolygonSolid;


//Represents some kind of 3D shape. Uses double dispatch for checking collision against other shapes.
class Shape
{
public:

    typedef std::shared_ptr<Shape> ShapePtr;


    Shape(Vector3f centerPos) : center(centerPos)
    { }


    //Gets the point on the shape's surface farthest in the given direction.
    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const = 0;

    virtual bool TouchingShape(const Shape & shape) const = 0;

    virtual bool TouchingCube(const Cube & cube) const = 0;
    virtual bool TouchingSphere(const Sphere & sphere) const = 0;
    virtual bool TouchingCapsule(const Capsule & capsule) const = 0;
    virtual bool TouchingPlane(const Plane & plane) const = 0;
    virtual bool TouchingTriangle(const Triangle & tri) const = 0;
    virtual bool TouchingPolygon(const PolygonSolid & poly) const;


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
    virtual bool TouchingTriangle(const Triangle & tri) const override;

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
    virtual bool TouchingSphere(const Sphere & sphere) const override { return GetCenter().DistanceSquared(sphere.GetCenter()) <= (BasicMath::Square(sphere.Radius + Radius) + BasicMath::Square(0.0f)); }
    virtual bool TouchingCapsule(const Capsule & capsule) const override;
    virtual bool TouchingPlane(const Plane & plane) const override;
    virtual bool TouchingTriangle(const Triangle & tri) const override;

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
    virtual bool TouchingTriangle(const Triangle & tri) const override;

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
        return point.DistanceSquared(GeometricMath::ClosestToLine(l1, l2, point, false)) <=
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
        if (BasicMath::Abs(dot) < MarginOfError)
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
    virtual bool TouchingTriangle(const Triangle & tri) const override;

    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

    virtual bool IsPointInside(Vector3f point) const override
    {
        return BasicMath::Abs(GetDistanceToPlane(point)) <= MarginOfError;
    }

    virtual ShapePtr GetClone(void) const override { return ShapePtr(new Plane(GetCenter(), Normal)); }

    virtual Box3D GetBoundingBox(void) const override;
};




class Triangle : public Shape
{
public:

    Triangle(Vector3f p1, Vector3f p2, Vector3f p3)
        : Shape((p1 + p2 + p3) * 0.333333f)
    {
        vertices[0] = p1;
        vertices[1] = p2;
        vertices[2] = p3;
    }
    Triangle(const Vector3f _vertices[3])
        : Shape((_vertices[0] + _vertices[1] + _vertices[2]) * 0.333333f)
    {
        vertices[0] = _vertices[0];
        vertices[1] = _vertices[1];
        vertices[2] = _vertices[2];
    }


    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const override
    {
        return GetCenter();
    }

    virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingTriangle(*this); }

    virtual bool TouchingCube(const Cube & cube) const override;
    virtual bool TouchingSphere(const Sphere & sphere) const override;
    virtual bool TouchingCapsule(const Capsule & capsule) const override;
    virtual bool TouchingPlane(const Plane & plane) const override;
    virtual bool TouchingTriangle(const Triangle & tri) const override;

    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

    virtual ShapePtr GetClone(void) const override { return ShapePtr(new Triangle(vertices)); }

    virtual bool IsPointInside(Vector3f point) const override;

    virtual void SetCenter(Vector3f newCenter) override
    {
        Vector3f delta = newCenter - GetCenter();

        Shape::SetCenter(newCenter);
        vertices[0] += delta;
        vertices[1] += delta;
        vertices[2] += delta;
    }

    virtual Box3D GetBoundingBox(void) const override
    {
        return Box3D(BasicMath::Min(vertices[0].x, vertices[1].x, vertices[2].x),
                     BasicMath::Max(vertices[0].x, vertices[1].x, vertices[2].x),
                     BasicMath::Min(vertices[0].y, vertices[1].y, vertices[2].y),
                     BasicMath::Max(vertices[0].y, vertices[1].y, vertices[2].y),
                     BasicMath::Min(vertices[0].z, vertices[1].z, vertices[2].z),
                     BasicMath::Max(vertices[0].z, vertices[1].z, vertices[2].z));
    }

    //Gets the three vertices that represent this triangle.
    const Vector3f * GetVertices(void) const { return vertices; }


private:

    Vector3f vertices[3];
};



//TODO: PolygonSolid stuff currently doesn't take into account the inside of the polygon -- only the surface. Maybe have two polygon classes: SimplePolygon (convex) and ComplexPolygon (concave, made of several SimplePolygons).
class PolygonSolid : public Shape
{
public:

    PolygonSolid(const std::vector<Triangle> & _triangles) : Shape(GetAverage(_triangles)), triangles(_triangles) { }


    const std::vector<Triangle> & GetTriangles(void) const { return triangles; }



    virtual Vector3f FarthestPointInDirection(Vector3f dirNormalized) const override
    {
        return GetCenter();
    }


    virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingPolygon(*this); }

    virtual bool TouchingCube(const Cube & cube) const override
    {
        return std::any_of(triangles.begin(), triangles.end(),
                           [&cube](const Triangle & tri) { return cube.TouchingTriangle(tri); });
    }
    virtual bool TouchingSphere(const Sphere & sphere) const override
    {
        return std::any_of(triangles.begin(), triangles.end(),
                           [&sphere](const Triangle & tri) { return sphere.TouchingTriangle(tri); });
    }
    virtual bool TouchingCapsule(const Capsule & capsule) const override
    {
        return std::any_of(triangles.begin(), triangles.end(),
                           [&capsule](const Triangle & tri) { return capsule.TouchingTriangle(tri); });
    }
    virtual bool TouchingPlane(const Plane & plane) const override
    {
        return std::any_of(triangles.begin(), triangles.end(),
                           [&plane](const Triangle & tri) { return plane.TouchingTriangle(tri); });
    }
    virtual bool TouchingTriangle(const Triangle & triangle) const override
    {
        return std::any_of(triangles.begin(), triangles.end(),
                           [&triangle](const Triangle & tri) { return triangle.TouchingTriangle(tri); });
    }
    virtual bool TouchingPolygon(const PolygonSolid & poly) const override
    {
        return std::any_of(triangles.begin(), triangles.end(),
                           [&poly](const Triangle & tri)
        {
            return std::any_of(poly.triangles.begin(), poly.triangles.end(),
                               [&tri](const Triangle & tri2)
            {
                return tri.TouchingTriangle(tri2);
            });
        });
    }

    virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

    virtual bool IsPointInside(Vector3f point) const override;

    virtual void SetCenter(Vector3f newCenter) override
    {
        Shape::SetCenter(newCenter);
        std::_For_each(triangles.begin(), triangles.end(),
                       [&newCenter](Triangle & tri) { tri.SetCenter(newCenter); });
    }

    virtual ShapePtr GetClone(void) const override { return ShapePtr(new PolygonSolid(triangles)); }

    virtual Box3D GetBoundingBox(void) const override;


private:

    static Vector3f GetAverage(const std::vector<Triangle> & tris)
    {
        if (tris.size() == 0) return Vector3f(0.0f, 0.0f, 0.0f);

        Vector3f sum = Vector3f(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < tris.size(); ++i)
            sum += tris[i].GetCenter();

        return sum / (float)tris.size();
    }

    std::vector<Triangle> triangles;
};