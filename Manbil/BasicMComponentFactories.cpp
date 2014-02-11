/*#include "BasicMComponentFactories.h"

MCompPtr TransformComponentFactory::BirthComponent(MObject & owner, TransformComponent * transform, unsigned int objectID)
{
	TransformComponent * tc = new TransformComponent(owner, objectID, Position);

	tc->SetRotation(Rot);
	tc->SetScale(Scale);

	return MCompPtr(tc);
}

MCompPtr CameraComponentFactory::BirthComponent(MObject & owner, TransformComponent * transform, unsigned int objectID)
{
	CameraComponent * cc = new CameraComponent(owner, objectID, transform);

	cc->SetRotation(ForwardVectorNormalized, UpVectorNormalized, true);
	cc->Info = ProjInfo;
	cc->SetClosestDotVariance(ClosestDotVariance);

	return MCompPtr(cc);
}

MCompPtr CollisionComponentFactory::BirthComponent(MObject & owner, TransformComponent * transform, unsigned int objectID)
{
	CollisionComponent * cc = new CollisionComponent(owner, objectID, transform, shape);

	return MCompPtr(cc);
}*/