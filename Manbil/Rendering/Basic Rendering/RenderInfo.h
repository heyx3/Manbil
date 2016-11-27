#pragma once

#include "OpenGLIncludes.h"
#include "../../Math/Higher Math/Camera.h"


//All the data necessary to render a scene.
struct RenderInfo
{
	//TODO: Rename to "CameraRenderInfo" or "CameraInfo".

    float TotalElapsedSeconds;
	Camera* Cam;

	Matrix4f *mView, *mProj;

    //Automatically computed in the constructor.
    //If the view/proj matrices are changed, this matrix will have to be manually recomputed.
    Matrix4f mVP;

    
	RenderInfo(float totalElapsedSeconds, Camera* camera, Matrix4f* viewM, Matrix4f* projM);
	RenderInfo(float totalElapsedSeconds, Camera* camera,
               Matrix4f* viewM, Matrix4f* projM, const Matrix4f& viewProjM);
};