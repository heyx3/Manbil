#pragma once

#include "../SFMLOpenGLWorld.h"
#include "../Rendering/GUI/GUIManager.h"
#include "../Editor/EditorMaterialSet.h"
#include "../Editor/Editor Panels/ColorEditor.h"


class GUITestWorld : public SFMLOpenGLWorld
{
public:

    static Vector2i WindowSize;


    GUITestWorld(void) : SFMLOpenGLWorld(WindowSize.x, WindowSize.y, sf::ContextSettings(24, 0, 0, 4, 1)) { }
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
    //Releases all OpenGL and heap-allocated data that currently exists.
    void DestroyMyStuff(bool destroyStatics);


    GUIManager guiManager;
    ColorEditor colEd;
    EditorMaterialSet* editorMaterials = 0;
};