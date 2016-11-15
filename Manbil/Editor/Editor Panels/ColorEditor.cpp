#include "ColorEditor.h"

#include "../EditorObjects.h"


#pragma warning(disable: 4100)
std::string ColorEditor::BuildEditorElements(std::vector<EditorObjectPtr>& outElements,
                                             EditorMaterialSet& materialSet)
{
    if (!colorDisplayTex.IsValidTexture())
    {
        colorDisplayTex.Create();
    }

    UpdateTextureColor();

    typedef EditorObject::DescriptionData Description;
    typedef SlidingBarFloat<> MSlidingBarFloa;
    MSlidingBarFloa* rSlider = new MSlidingBarFloa(0.0f, 1.0f, Vector2f(0.0f, 10.0f),
                                                   Description("Red", true, 10.0f, 200),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.x = newVal;
                                                       ce->UpdateTextureColor();
                                                   },
                                                   Color.x, 1.0f, this);
    MSlidingBarFloa* gSlider = new MSlidingBarFloa(0.0f, 1.0f, Vector2f(0.0f, 10.0f),
                                                   Description("Green", true, 10.0f, 400),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.y = newVal;
                                                       ce->UpdateTextureColor();
                                                   },
                                                   Color.y, 1.0f, this);
    MSlidingBarFloa* bSlider = new MSlidingBarFloa(0.0f, 1.0f, Vector2f(0.0f, 5.0f),
                                                   Description("Blue", true, 10.0f, 200),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.z = newVal;
                                                       ce->UpdateTextureColor();
                                                   },
                                                   Color.z, 1.0f, this);

    EditorImage* img = new EditorImage(&colorDisplayTex, Description(),
                                       Vector2f(100.0f, 20.0f), Vector2f(0.0f, 5.0f));

    MSlidingBarFloa* aSlider = new MSlidingBarFloa(0.0f, 1.0f, Vector2f(),
                                                   Description("Alpha", true, 10.0f, 400),
                                                   [](GUISlider* slider, float newVal, void* pData)
                                                   {
                                                       ColorEditor* ce = (ColorEditor*)pData;
                                                       ce->Color.w = newVal;
                                                   },
                                                   Color.w, 1.0f, this);

    outElements.push_back(EditorObjectPtr(rSlider));
    outElements.push_back(EditorObjectPtr(gSlider));
    outElements.push_back(EditorObjectPtr(bSlider));
    outElements.push_back(EditorObjectPtr(img));
    outElements.push_back(EditorObjectPtr(aSlider));


    return "";
}
#pragma warning(default: 4100)

void ColorEditor::UpdateTextureColor(void)
{
    Array2D<Vector4f> colData(1, 1, Color);
    colData[Vector2u(0, 0)].w = 1.0f;
    colorDisplayTex.SetColorData(colData);
}