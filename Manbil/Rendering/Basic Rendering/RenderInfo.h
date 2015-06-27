#pragma once

#include "OpenGLIncludes.h"
#include "../../Math/Higher Math/Camera.h"
#include "../../Math/Higher Math/TransformObject.h"


//All the data necessary to render a scene.
struct RenderInfo
{
    float TotalElapsedSeconds;
	Camera* Cam;

	Matrix4f *mView, *mProj;

    //Automatically computed in the constructor.
    //If the view/proj matrices are changed, this matrix will have to be manually recomputed.
    Matrix4f mVP;


	RenderInfo(float totalElapsedSeconds, Camera* camera, Matrix4f* viewM, Matrix4f* projM);
};