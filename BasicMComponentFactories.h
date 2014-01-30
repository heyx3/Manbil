#pragma once

#include "BasicMComponents.h"


//Exposes "factories", which are classes that can birth new MComponents.


typedef std::shared_ptr<MComponent> MCompPtr;

class MComponentFactory
{
public:
	virtual MCompPtr BirthComponent(MObject & owner, TransformComponent * transform, unsigned int objectID) = 0;
};

class TransformComponentFactory : public MComponentFactory
{
public:

	Vector3f Position;
	Vector3f Scale;
	Vector3f Rot;

	TransformComponentFactory(Vector3f pos) : Position(pos), Scale(Vector3f(1, 1, 1)), Rot(Vector3f()) { }

	virtual MCompPtr BirthComponent(MObject & owner, TransformComponent * transform, unsigned int objectID) override;
};

class CameraComponentFactory : public MComponentFactory
{
public:

	Vector3f UpVectorNormalized, ForwardVectorNormalized;
	ProjectionInfo ProjInfo;
	float ClosestDotVariance;

	CameraComponentFactory(Vector3f upV, Vector3f forwardV, bool alreadyNormalized) : ClosestDotVariance(0.001f), UpVectorNormalized(upV), ForwardVectorNormalized(forwardV) { if (!alreadyNormalized) { UpVectorNormalized.Normalize(); ForwardVectorNormalized.Normalize(); } }

	virtual MCompPtr BirthComponent(MObject & owner, TransformComponent * transform, unsigned int objectID) override;
};

class CollisionComponentFactory : public MComponentFactory
{
public:

	CollisionComponentFactory(const Shape & colShape) : shape(colShape.GetClone()) { }

	void SetCollisionShape(std::shared_ptr<Shape> newShape) { shape.swap(newShape); }

	virtual MCompPtr BirthComponent(MObject & owner, TransformComponent * transform, unsigned int objectID) override;

private:

	std::shared_ptr<Shape> shape;
};