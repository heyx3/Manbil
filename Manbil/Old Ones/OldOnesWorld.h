#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/Rendering.hpp"

#include "../Input/MovingCamera.h"

#include "WorldObject.h"
#include "PostProcessing.h"
#include "FractalRenderer.h"
#include "OldOnesSkybox.h"
#include "OldOneShadowMap.h"


class OldOnesWorld : public SFMLOpenGLWorld
{
public:

    OldOnesWorld(void);
    

protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override;
    virtual std::string GetWindowTitle(void) override;
    virtual sf::Uint32 GetSFStyleFlags(void) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;


private:

    //Potentially render the world at a different size than the window.
    //Higher sizes results in a very high-quality Anti-Aliasing (known as Supersampling).
    //Lower sizes results in better performance (at the cost of quality).
    Vector2u windowSize, renderSize;

    MovingCamera gameCam;

    std::vector<std::shared_ptr<WorldObject>> objs;
    
    OldOnesSkybox* skybox;
    FractalRenderer* oldOne;
    PostProcessing* ppEffects;
    OldOneShadowMap* shadowMap;

    RenderTarget* worldRT;
    MTexture2D worldColor, worldDepth;

    Material* finalRenderMat;
    UniformDictionary finalRenderParams;


    void RenderWorld(RenderInfo& info);
};