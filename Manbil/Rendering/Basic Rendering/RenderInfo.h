#pragma once

#include "OpenGLIncludes.h"
#include "../../Math/Higher Math/Camera.h"
#include "../../Math/Higher Math/TransformObject.h"


//Represents all useful rendering data.
struct RenderInfo
{
    float TotalElapsedSeconds;
	Camera* Cam;

	Matrix4f *mView, *mProj;

    //Automatically computed when this struct is created.
    //If the view/proj matrices are changed, this matrix will have to be manually recomputed.
    Matrix4f mVP;


	RenderInfo(float totalElapsedSeconds, Camera* camera, Matrix4f* viewM, Matrix4f* projM);
};