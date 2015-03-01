#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/GUI/GUIManager.h"
#include "../Editor/EditorMaterialSet.h"
#include "../Editor/Editor Panels/ColorEditor.h"
#include "../Rendering/GUI/TextRenderer.h"


//TODO: Rename to something like "EditorSystemTestWorld".
class GUITestWorld : public SFMLOpenGLWorld
{
public:

    Vector2i WindowSize;

    TextRenderer* TextRender;


    GUITestWorld(void)
        : SFMLOpenGLWorld(WindowSize.x, WindowSize.y, sf::ContextSettings(24, 0, 0, 4, 1)) { }


protected:

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void);
    
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;


private:

    //If there is an error, prints 'errorIntro + ": " + errorMsg', ends the world, and returns false.
    bool ReactToError(bool isEverythingOK, std::string errorIntro, std::string errorMsg);


    GUIManager guiManager;
    ColorEditor colEd;
    EditorMaterialSet* editorMaterials = 0;
};