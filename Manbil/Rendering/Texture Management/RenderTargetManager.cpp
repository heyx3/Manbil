#include "RenderTargetManager.h"

#include "../../DebugAssist.h"



unsigned int RenderTargetManager::CreateRenderTarget(PixelSizes pixelSize)
{
    if (!IsPixelSizeDepth(pixelSize))
    {
        errorMsg = "The pixel size for the depth renderbuffer wasn't a depth size; it was " + DebugAssist::ToString(pixelSize);
        return ERROR_ID;
    }
    RenderTarget * newTarget = new RenderTarget(pixelSize);
    if (newTarget->HasError())
    {
        errorMsg = "Error creating render target : " + newTarget->GetErrorMessage();
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

RenderTarget * RenderTargetManager::operator[](unsigned int index)
{
    if (targets.find(index) == targets.end())
    {
        return 0;
    }

    return targets[index];
}