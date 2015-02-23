#pragma once

#include "../IEditable.h"


//A color value that can be chosen in several different ways.
class ColorEditor : IEditable
{
public:

    Vector4f Color;

    ColorEditor(void)
        : colorDisplayTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP),
                          PixelSizes::PS_32F, false) { }

    virtual std::string BuildEditorElements(std::vector<EditorObjectPtr>& outElements,
                                            EditorMaterialSet& materialSet) override;
    void UpdateTextureColor(void);

private:

    MTexture2D colorDisplayTex;
};