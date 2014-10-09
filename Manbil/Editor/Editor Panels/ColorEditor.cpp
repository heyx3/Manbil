#include "ColorEditor.h"

#include "../EditorObjects.h"


void ColorEditor::BuildEditorElements(std::vector<EditorObjectPtr> & outElements)
{
    if (!colorDisplayTex.IsValidTexture())
        colorDisplayTex.Create();

    UpdateTextureColor();


    SlidingBarFloat* rSlider = new SlidingBarFloat(0.0f, 1.0f, Vector2f(0.0f, 10.0f),
                                                   EditorObject::DescriptionData("Red", true, 10.0f, 200),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.x = newVal;
                                                       ce->UpdateTextureColor();
                                                   },
                                                   Color.x, 1.0f, this);
    SlidingBarFloat* gSlider = new SlidingBarFloat(0.0f, 1.0f, Vector2f(0.0f, 10.0f),
                                                   EditorObject::DescriptionData("Green", true, 10.0f, 400),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.y = newVal;
                                                       ce->UpdateTextureColor();
                                                   },
                                                   Color.y, 1.0f, this);
    SlidingBarFloat* bSlider = new SlidingBarFloat(0.0f, 1.0f, Vector2f(0.0f, 5.0f),
                                                   EditorObject::DescriptionData("Blue", true, 10.0f, 200),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.z = newVal;
                                                       ce->UpdateTextureColor();
                                                   },
                                                   Color.z, 1.0f, this);
    EditorImage* img = new EditorImage(&colorDisplayTex, EditorObject::DescriptionData(),
                                       Vector2f(100.0f, 20.0f), Vector2f(0.0f, 5.0f));
    SlidingBarFloat* aSlider = new SlidingBarFloat(0.0f, 1.0f, Vector2f(),
                                                   EditorObject::DescriptionData("Alpha", true, 10.0f, 400),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.w = newVal;
                                                   },
                                                   Color.w, 1.0f, this);

    outElements.insert(outElements.end(), EditorObjectPtr(rSlider));
    outElements.insert(outElements.end(), EditorObjectPtr(gSlider));
    outElements.insert(outElements.end(), EditorObjectPtr(bSlider));
    outElements.insert(outElements.end(), EditorObjectPtr(img));
    outElements.insert(outElements.end(), EditorObjectPtr(aSlider));
}

void ColorEditor::UpdateTextureColor(void)
{
    Array2D<Vector4f> colData(1, 1, Color);
    colData[Vector2u(0, 0)].w = 1.0f;
    colorDisplayTex.SetColorData(colData);
}