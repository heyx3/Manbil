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

    ClearAllRenderingErrors();
    delete targets[id];
    targets.erase(location);

    errorMsg = GetCurrentRenderingError();
    if (!errorMsg.empty())
    {
        errorMsg = "Error releasing GL resources when destroying render target with ID " + std::to_string(id) + ": " + errorMsg;
        return false;
    }

    return true;
}

bool RenderTargetManager::ResizeTarget(unsigned int id, unsigned int w, unsigned int h)
{
    ClearAllRenderingErrors();

    //If the given render target doesn't exist, return an error.
    RenderTarget * old = operator[](id);
    if (old == 0)
    {
        errorMsg = "Error resizing render target " + std::to_string(id) + ": render target not found.";
        return false;
    }
    //If the new target wasn't created correctly, return an error.
    RenderTarget * newTarget = new RenderTarget(w, h, old->GetUsesColor(), old->GetUsesDepth());
    if (newTarget->HasError())
    {
        errorMsg = "Error recreating render target " + std::to_string(id) + " to resize it: " + newTarget->GetErrorMessage();
        return false;
    }
    //If the old target wasn't deleted correctly, return an error.
    if (!DeleteRenderTarget(id))
    {
        errorMsg = "Error deleting render target to resize it: " + errorMsg;
        return false;
    }

    //Insert the new target.
    targets[id] = newTarget;

    return true;
}

RenderTarget * RenderTargetManager::operator[](unsigned int index)
{
    if (targets.find(index) == targets.end())
    {
        return 0;
    }

    return targets[index];
}