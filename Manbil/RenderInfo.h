#pragma once

#include "SFMLOpenGLWorld.h"
#include "OpenGLIncludes.h"
#include "Math/Higher Math/Camera.h"
#include "Math/Higher Math/TransformObject.h"


//Represents all useful rendering data.
struct RenderInfo
{
	SFMLOpenGLWorld * World;
	Camera * Cam;
	TransformObject * Trans;

	Matrix4f * mWorld, * mView, * mProj;
    Matrix4f mWVP;

	RenderInfo(SFMLOpenGLWorld * world, Camera * camera, TransformObject * trans, Matrix4f * worldM, Matrix4f * viewM, Matrix4f * projM)
		: World(world), Cam(camera), Trans(trans), mWorld(worldM), mView(viewM), mProj(projM)
    {
        mWVP.SetAsWVP(*mProj, *mView, *mWorld);
    }
	RenderInfo(const RenderInfo & copy) : World(copy.World), Cam(copy.Cam), Trans(copy.Trans), mWorld(copy.mWorld), mView(copy.mView), mProj(copy.mProj), mWVP(copy.mWVP) { }
};