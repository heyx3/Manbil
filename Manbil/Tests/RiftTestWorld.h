#pragma once

#include "../SFMLOpenGLWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Rendering/Helper Classes/DrawingQuad.h"
#include "../MovingCamera.h"
#include "../Oculus Rift/OculusDevice.h"


//A simple game world that demos the capabilities of the Rift.
class RiftTestWorld : public SFMLOpenGLWorld
{
public:

    RiftTestWorld(void);
    ~RiftTestWorld(void);


protected:

    virtual void InitializeWorld(void) override;
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newW, unsigned int newH) override;


private:

    //If the given test value is false, the following is done:
    //  1) Prints the given error, then pauses for the user to acknowledge.
    //  2) Ends the world.
    //  3) Returns false.
    //Otherwise, returns true.
    bool Assert(bool test, std::string errorIntro, const std::string& error);


    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);

    //Renders the world geometry using the given rendering info.
    void RenderWorldGeometry(const RenderInfo& info);


    Material* objectMat;
    Mesh cube;
    UniformDictionary floorParams, obj1Params, obj2Params;
    TransformObject floorT, obj1T, obj2T;


    Material* quadMat;
    DrawingQuad* quad;
    UniformDictionary quadParams;


    MTexture2D floorTex, obj1Tex, obj2Tex;


    Vector3f lightCol, lightDir;
    float ambientLight, diffuseLight, specLight;
    float specIntensity;

    Vector2u windowSize;
    MovingCamera baseCam;


    ovrHmd hmd = 0;
    ovrTrackingState hmdState;
};