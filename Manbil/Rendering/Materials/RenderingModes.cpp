#include "RenderingModes.h"

#include <assert.h>


bool IsModeTransparent(RenderingModes mode)
{
    return (mode == RM_Transluscent || mode == RM_Additive);
}