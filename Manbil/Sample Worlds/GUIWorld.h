#pragma once


#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/GUI/GUIManager.h"
#include "../Rendering/GUI/TextRenderer.h"


//Shows a basic example of how to use the GUI system, including font rendering.
class GUIWorld : public SFMLOpenGLWorld
{
public:

    GUIWorld(void);


protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override;
    virtual std::string GetWindowTitle(void) override;
    virtual sf::Uint32 GetSFStyleFlags(void) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void);
    
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;


private:
    
    void InitializeMaterials(void);
    void InitializeTextures(void);
    void InitializeGUI(void);


    Vector2u windowSize;

    TextRenderer* textRenderer;
    GUIManager guiManager;

    //Three types of GUI materials: animated, not animated, and text.
    Material *simpleGUIMat, *animatedGUIMat, *guiTextMat;
    UniformDictionary simpleGUIMatParams, animatedGUIMatParams, guiTextMatParams;

    MTexture2D texBackground, texCheck, texSliderBar, texSliderNub;
    
    FreeTypeHandler::FontID textFont;
};