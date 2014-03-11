#include "RenderTargetManager.h"

unsigned int RenderTargetManager::CreateRenderTarget(unsigned int width, unsigned int height, bool useColor, bool useDepth)
{
    RenderTarget * newTarget = new RenderTarget(width, height, useColor, useDepth);
    if (newTarget->HasError())
    {
        errorMsg = "Error creating render target of size { " + std::to_string(width) + ", " + std::to_string(height) +
            " using " + (useColor && useDepth ? "color and depth" : (useColor ? "color" : (useDepth ? "depth" : "neither color nor depth"))) +
            ": " + newTarget->GetErrorMessage();
        return ERROR_ID;
    }

    targets[nextID] = newTarget;
    nextID += 1;
    return nextID - 1;
}

bool RenderTargetManager::DeleteRenderTarget(unsigned int id)
{
    auto location = targets.find(id);

    if (location == targets.end())
    {
        errorMsg = "Error deleting render target with ID " + std::to_string(id) + ": ID wasn't found";
        return false;
    }

    delete targets[id];
    targets.erase(location);
    return true;
}

RenderTarget * RenderTargetManager::operator[](unsigned int index)
{
    if (targets.find(index) == targets.end())
    {
        errorMsg = "Error accessing render target with ID " + std::to_string(index) + ": ID wasn't found";
        return 0;
    }

    return targets[index];
}