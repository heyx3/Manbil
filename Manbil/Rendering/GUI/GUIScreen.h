#pragma once

#include <unordered_map>
#include <memory>
#include "../../Math/Vectors.h"
#include "../Texture Management/RenderTargetManager.h"

/*
class GUIElement;
typedef std::shared_ptr<GUIElement> GUIElementPtr;



//A collection of GUI elements.
//TODO: Finish after finalizing GUIElement rendering.
class GUIScreen
{
public:

    struct GUIElementEntry { Vector2f Offset; GUIElementPtr Element; };


    RenderTargetManager & RTManager;
    std::vector<GUIElementEntry> Elements;


    GUIScreen(RenderTargetManager & rtManager) : RTManager(rtManager) { }


    const RenderTarget * GetFinalRender(void) const { return RTManager[finalScreenRenderTargetID]; }
    RenderTarget * GetFinalRender(void) { return RTManager[finalScreenRenderTargetID]; }

    Vector2i GetFinalRenderSize(void) const { return Vector2i((int)GetFinalRender()->GetWidth(), (int)GetFinalRender()->GetHeight()); }


private:

    unsigned int finalScreenRenderTargetID;
};

*/