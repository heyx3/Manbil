#include "RenderInfo.h"


RenderInfo::RenderInfo(float totalElapsedSeconds, Camera* camera, Matrix4f* viewM, Matrix4f* projM)
    : TotalElapsedSeconds(totalElapsedSeconds), Cam(camera), mView(viewM), mProj(projM)
{
    if (mProj != 0 && mView != 0)
    {
        mVP = Matrix4f::Multiply(*mProj, *mView);
    }
    else
    {
        mVP.SetAsIdentity();
    }
}

RenderInfo::RenderInfo(float totalElapsedSeconds, Camera* camera,
                       Matrix4f* viewM, Matrix4f* projM, const Matrix4f& viewProjM)
    : TotalElapsedSeconds(totalElapsedSeconds), Cam(camera),
      mView(viewM), mProj(projM), mVP(viewProjM) { }