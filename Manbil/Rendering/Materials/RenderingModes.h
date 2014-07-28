#pragma once

#include <string>


//Different kinds of materials.
enum RenderingModes
{
    RM_Opaque,
    RM_Transluscent,
    RM_Additive,
};
bool IsModeTransparent(RenderingModes mode);