#pragma once

#include "../Rendering/GUI/GUIManager.h"
#include "../Editor/EditorMaterialSet.h"

#include "OldOneEditableData.h"


class OldOneEditorGUI
{
public:

    OldOneEditorGUI(std::string& outErr);


    void Update(float frameSeconds, Vector2i mousePos, bool mouseClicked);
    void Render(float frameSeconds, float totalSeconds, Vector2i windowSize);

    const OldOneEditableData& GetData(void) const { return data; }


private:

    OldOneEditableData data;

    TextRenderer textRender;
    GUIManager manager;
    EditorMaterialSet materials;
};