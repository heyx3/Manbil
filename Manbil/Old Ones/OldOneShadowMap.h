#pragma once

#include "WorldObject.h"


//The directional light shadow-mapping system.
class OldOneShadowMap
{
public:

    //If there is an error, outputs an error string to the "outError" parameter.
    OldOneShadowMap(std::vector<std::shared_ptr<WorldObject>>& worldObjects,
                    std::string& outError);
    ~OldOneShadowMap(void);


    Matrix4f* GetViewM(void) { return &viewM; }
    Matrix4f* GetProjM(void) { return &projM; }

    void Render(float totalElapsedSeconds);


private:

    std::vector<std::shared_ptr<WorldObject>>& objs;

    RenderTarget rt;
    MTexture2D depthTex;

    Matrix4f viewM, projM;

    Material *matUVNormal,
             *matUVNoNormal,
             *matNormalNoUV,
             *matNoUVNormal;
};