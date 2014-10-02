#pragma once

#include "../IEditable.h"


//A color value that can be chosen in several different ways.
class ColorEditor : IEditable
{
public:

    bool UsesAlpha;
    Vector4f Color;

    virtual void BuildEditorElements(std::vector<EditorObjectPtr> & outElements) override;
};