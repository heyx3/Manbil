#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Rendering/Primitives/DrawingQuad.h"
#include "../Rendering/Management/RenderTargetManager.h"
#include "../Input/MovingCamera.h"
#include "../Oculus Rift/OculusDevice.h"
#include <OVR_CAPI_GL.h>


//A simple game world that demos the capabilities of the Rift.
class RiftTestWorld : public SFMLOpenGLWorld
{
public:

    RenderTargetManager RenderTargets;


    RiftTestWorld(void);


protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override
    {
        return sf::VideoMode(windowW, windowH);
    }
    virtual sf::Uint32 GetSFStyleFlags(void) override
    {
        return sf::Style::None;
    }


    virtual void InitializeWorld(void) override;
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;
    virtual void OnWorldEnd(void) override;

    virtual void RenderWorld(float elapsedSeconds) override { RenderOpenGL(elapsedSeconds); }

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newW, unsigned int newH) override;


private:

    //If the given test value is false, the following is done:
    //  1) Prints the given error, then pauses for the user to acknowledge.
    //  2) Ends the world.
    //  3) Returns false.
    //Otherwise, returns true.
    bool Assert(bool test, std::string errorIntro, const std::string& error);


    void InitializeOculus(void);
    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);
    void SetUpOculusDevice(void);

    //Renders the world geometry using the given rendering info.
    void RenderWorldGeometry(const RenderInfo& info);


    Material* objectMat;
    Mesh cube;
    UniformDictionary floorParams, obj1Params, obj2Params;
    TransformObject floorT, obj1T, obj2T;
    MTexture2D floorTex, obj1Tex, obj2Tex;

    Vector3f lightCol, lightDir;
    float ambientLight, diffuseLight, specLight;
    float specIntensity;


    Material* quadMat;
    UniformDictionary quadParams;
    unsigned int eyeRendTarget;
    MTexture2D eyeColorTex1, eyeColorTex2;
    MTexture2D eyeDepthTex;


    Vector2u windowSize;
    MovingCamera baseCam;


    ovrHmd hmd = 0;
    ovrTrackingState hmdState;
    ovrEyeRenderDesc hmdEyeSettings[2];
    ovrGLTexture ovrTexes[2];
    ovrFrameTiming ovrTiming;
};