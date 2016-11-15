#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/GUI/GUIManager.h"
#include "../Editor/EditorMaterialSet.h"
#include "../Editor/Editor Panels/ColorEditor.h"
#include "../Rendering/GUI/TextRenderer.h"


//Shows how to use the Editor system for easy creation of GUI panels for modifying values.
class EditorGUIWorld : public SFMLOpenGLWorld
{
public:

    Vector2i WindowSize;
    TextRenderer* TextRender;


    EditorGUIWorld(void);


protected:
    
    virtual std::string GetWindowTitle(void) override { return "EditorGUIWorld"; }
    virtual sf::ContextSettings GenerateContext(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void);
    
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;


private:

    //Returns whether there was an error setting up editor stuff.
    bool InitializeEditorStuff(void);


    //If there is an error, prints 'errorIntro + ": " + errorMsg', ends the world, and returns false.
    bool Assert(bool isEverythingOK, std::string errorIntro, std::string errorMsg);


    GUIManager guiManager;
    EditorMaterialSet* editorMaterials = 0;
};