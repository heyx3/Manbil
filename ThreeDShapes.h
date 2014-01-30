#pragma once

#include "Vectors.h"
#include "BasicMath.h"
#include "GeometricMath.h"
#include "Boxes.h"
#include <memory>

class Cube;
class Sphere;
class Capsule;

//Represents some kind of 3D shape. Uses double dispatch for checking collision against other shapes.
class Shape
{
public:

	typedef std::shared_ptr<Shape> ShapePtr;

	Shape(Vector3f centerPos) : center(centerPos) { }

	virtual bool TouchingCube(const Cube & cube) const = 0;
	virtual bool TouchingSphere(const Sphere & sphere) const = 0;
	virtual bool TouchingCapsule(const Capsule & capsule) const = 0;

	virtual bool TouchingShape(const Shape & shape) const = 0;

	struct RayTraceResult
	{
		bool DidHitTarget;
		Vector3f HitPos;
		Vector3f ReflectNormal;
		RayTraceResult(Vector3f hitPos, Vector3f refNormal) : HitPos(hitPos), ReflectNormal(refNormal), DidHitTarget(true) { }
		RayTraceResult(bool didHitTarget = false) : DidHitTarget(didHitTarget) { }
	};
	virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const = 0;

	virtual ShapePtr GetClone(void) const = 0;

	virtual void SetCenter(Vector3f newCenter) { center = newCenter; }
	Vector3f GetCenter(void) const { return center; }

private:

	Vector3f center;
};


class Cube : public Shape
{
public:

	const Box3D & GetBounds(void) const { return Bounds; }

	Cube(Vector3f topLeftFront, Vector3f dimensions) : Shape(topLeftFront + (dimensions * 0.5f)), Bounds(topLeftFront, dimensions) { }
	Cube(const Box3D & toCopy) : Shape(toCopy.GetCenter()), Bounds(toCopy) { }

	virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingCube(*this); }

	virtual bool TouchingCube(const Cube & cube) const override { return Bounds.Touches(cube.Bounds); }
	virtual bool TouchingSphere(const Sphere & sphere) const override;
	virtual bool TouchingCapsule(const Capsule & capsule) const override;

	virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

	virtual ShapePtr GetClone(void) const override { return ShapePtr(new Cube(Bounds.GetTopLeftFront(), Bounds.GetDimensions())); }
	
	virtual void SetCenter(Vector3f newCenter) override { Shape::SetCenter(newCenter); Bounds.SetCenterOfBox(newCenter); }
	void SetDimensions(Vector3f dimensions) { Bounds.SetDimensions(dimensions); }

	void GetBoundingSphere(Sphere & sOut) const;

private:

	Box3D Bounds;
};
class Sphere : public Shape
{
public:

	Sphere(Vector3f center, float radius) : Shape(center), Radius(radius) { }

	float Radius;

	virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingSphere(*this); }
	
	virtual bool TouchingCube(const Cube & cube) const override;
	virtual bool TouchingSphere(const Sphere & sphere) const override { return GetCenter().DistanceSquared(sphere.GetCenter()) <= (BasicMath::Square(sphere.Radius) + BasicMath::Square(Radius)); }
	virtual bool TouchingCapsule(const Capsule & capsule) const override;
	
	virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

	virtual ShapePtr GetClone(void) const override { return ShapePtr(new Sphere(GetCenter(), Radius)); }

	void GetBoundingCube(Cube & cOut) const;
};
class Capsule : public Shape
{
public:

	float Radius;

	Capsule(Vector3f _l1, Vector3f _l2, float radius) : Shape((l1 + l2) * 0.5f), Radius(radius), l1(_l1), l2(_l2) { }
	
	virtual bool TouchingShape(const Shape & shape) const override { return shape.TouchingCapsule(*this); }
	
	virtual bool TouchingCube(const Cube & cube) const override;
	virtual bool TouchingSphere(const Sphere & sphere) const override;
	virtual bool TouchingCapsule(const Capsule & capsule) const override;
	
	virtual RayTraceResult RayHitCheck(Vector3f rayStart, Vector3f rayDir) const override;

	void SetEndpoint1(Vector3f newL1) { l1 = newL1; RecomputeCenter(); }
	void SetEndpoint2(Vector3f newL2) { l2 = newL2; RecomputeCenter(); }

	void Translate(Vector3f amount) { l1 += amount; l2 += amount; RecomputeCenter(); }

	Vector3f GetEndpoint1(void) const { return l1; }
	Vector3f GetEndpoint2(void) const { return l2; }

	Vector3f GetParallelVector(void) const { return (l2 - l1).Normalized(); }
	float GetLength(void) const { return l2.Distance(l1) + Radius; }
	
	virtual void SetCenter(Vector3f center) override { Translate(center - GetCenter()); }

	virtual ShapePtr GetClone(void) const override { return ShapePtr(new Capsule(l1, l2, Radius)); }

private:

	void RecomputeCenter(void) { SetCenter((l1 + l2) * 0.5f); }

	Vector3f l1, l2;
};