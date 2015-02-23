#include "RenderTargetManager.h"

#include "../../DebugAssist.h"



unsigned int RenderTargetManager::CreateRenderTarget(PixelSizes pixelSize, std::string& outErr)
{
    //Make sure the pixel size is valid.
    if (!IsPixelSizeDepth(pixelSize))
    {
        return ERROR_ID;
    }

    //Try to create the render target.
    targets[nextID] = RenderTarget(pixelSize, outErr);
    if (!outErr.empty())
    {
        outErr = "Error creating render target : " + outErr;
        return ERROR_ID;
    }

    //Increment the ID counter.
    nextID += 1;
    return nextID - 1;
}

bool RenderTargetManager::DeleteRenderTarget(unsigned int id)
{
    auto location = targets.find(id);
    if (location == targets.end())
    {
        return false;
    }

    targets.erase(location);
    return true;
}

RenderTarget* RenderTargetManager::operator[](unsigned int index)
{
    if (targets.find(index) == targets.end())
    {
        return 0;
    }

    return &targets[index];
}