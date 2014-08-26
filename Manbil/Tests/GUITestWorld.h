#pragma once

#include "../SFMLOpenGLWorld.h"
#include "../Rendering/Helper Classes/DrawingQuad.h"
#include "../Rendering/GUI/GUIManager.h"
#include "../Rendering/GUI/GUI Elements/GUILabel.h"
#include "../Rendering/GUI/GUI Elements/GUITexture.h"
#include "../Rendering/GUI/GUI Elements/GUISlider.h"
#include "../Rendering/GUI/GUI Elements/GUISelectionBox.h"


class GUITestWorld : public SFMLOpenGLWorld
{
public:

    static Vector2i WindowSize;

    GUITestWorld(void)
        : SFMLOpenGLWorld(WindowSize.x, WindowSize.y, sf::ContextSettings(24, 0, 0, 4, 1)),
        quad(0), quadMat(0), curveMesh(PrimitiveTypes::TriangleStrip), curveMat(0),
        curveStartSlope(-0.99f, -0.99f, 0.0f), curveEndSlope(0.99f, 0.99f, 0.0f),
        guiMat(0), guiTexData(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_32F, false),
        guiBarTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U, false),
        guiNubTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_8U, false),
        guiLabel(UniformDictionary()), guiTex(UniformDictionary()), guiBar(UniformDictionary())
    {
    }
    virtual ~GUITestWorld(void) { DestroyMyStuff(false); }


protected:

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) { DestroyMyStuff(true); }
    
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;


private:

    //If there is an error, prints 'errorIntro + ": " + errorMsg', ends the world, and returns false.
    bool ReactToError(bool isEverythingOK, std::string errorIntro, std::string errorMsg);

    void DestroyMyStuff(bool destroyStatics);


    DrawingQuad * quad;
    UniformDictionary quadParams;
    Material * quadMat;

    GUIManager guiManager;
    Material * guiMat;

    GUILabel guiLabel;
    GUITexture guiTex;
    MTexture2D guiTexData;
    GUISlider guiBar;
    MTexture2D guiBarTex, guiNubTex;
    GUISelectionBox guiSelector;

    Mesh curveMesh;
    Material * curveMat;
    UniformDictionary curveParams;
    Vector3f curveStartSlope, curveEndSlope;
};