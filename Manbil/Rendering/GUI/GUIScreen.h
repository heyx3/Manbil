#pragma once

#include "../../Math/Vectors.h"
#include <unordered_map>
#include "GUIElement.h"


//A collection of GUI elements.
class GUIScreen
{
public:

    typedef unsigned int GUI_ID;


private:

    typedef std::unordered_map<GUI_ID, Vector2f> OffsetMap;
    typedef std::unordered_map<GUI_ID, GUIElement> ElementMap;
};