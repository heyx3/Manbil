#pragma once

#include "../Rendering/GUI/GUIMaterials.h"


//A set of materials for use in creating editor panels.
//TODO: Rename to EditorContentData.
//TODO: Add text render texture filter quality to this class.
struct EditorMaterialSet
{
public:

    //Generates default data (including textures and materials) for an instance of this class.
    //Returns an error message if something went wrong, or the empty string if everything went fine.
    static std::string GenerateDefaultInstance(EditorMaterialSet & outSet);


    //The min/max colors used for animating GUI elements.
    Vector4f MinAnimateColor = Vector4f(0.7f, 0.7f, 0.7f, 1.0f),
             MaxAnimateColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    //The speed of animated GUI elements.
    float AnimateSpeed = 10.0f;

    TextRenderer & TextRender;
    unsigned int FontID;

    MTexture2D ButtonTex, SliderBarTex, SliderNubTex,
               TextBoxBackgroundTex, PanelBackgroundTex,
               CheckBoxBackgroundTex, CheckBoxCheckTex,
               CollapsibleEditorTitleBarTex,
               AddToCollectionTex, DeleteFromCollectionTex,
               SelectionBoxBoxTex, SelectionBoxBackgroundTex;

    Vector2f SliderBarScale = Vector2f(100.0f, 18.5f),
             SliderNubScale = Vector2f(10.0f, 30.0f);
    float TextBoxCursorWidth = 8.0f;
    Vector4f TextColor = Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
             CollapsibleEditorTitleTextCol = Vector4f(0.875f, 0.875f, 0.875f, 1.0f);
    Vector2f TextScale = Vector2f(0.3f, 0.3f);
    float DropdownBoxItemSpacing = 10.0f;
    unsigned int TextRenderSpaceHeight = 128;

    Material *AnimatedMatGrey = 0,
             *StaticMatGrey = 0,
             *AnimatedMatColor = 0,
             *StaticMatColor = 0,
             *AnimatedMatText = 0,
             *StaticMatText = 0;
    UniformDictionary AnimatedMatGreyParams = UniformDictionary(),
                      StaticMatGreyParams = UniformDictionary(),
                      AnimatedMatColParams = UniformDictionary(),
                      StaticMatColParams = UniformDictionary(),
                      AnimatedMatTextParams = UniformDictionary(),
                      StaticMatTextParams = UniformDictionary();


    EditorMaterialSet(const EditorMaterialSet & cpy) = delete;
    EditorMaterialSet(TextRenderer & renderer);

    ~EditorMaterialSet(void);


    //Gets the animated material in this material set that would be used for the given texture.
    Material* GetAnimatedMaterial(const MTexture2D * tex) const;
    //Gets the static material in this material set that would be used for the given texture.
    Material* GetStaticMaterial(const MTexture2D * tex) const;

    //Gets the animated material parameters in this material set that would be used for the given texture.
    const UniformDictionary & GetAnimatedMatParams(const MTexture2D * tex) const;
    //Gets the static material parameters in this material set that would be used for the given texture.
    const UniformDictionary & GetStaticMatParams(const MTexture2D * tex) const;
};