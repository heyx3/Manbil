#include "RenderTargetManager.h"

unsigned int RenderTargetManager::CreateRenderTarget(const std::vector<RendTargetColorTexSettings> & colTexes, const RendTargetDepthTexSettings & depthTex)
{
    RenderTarget * newTarget = new RenderTarget(colTexes, depthTex);
    if (newTarget->HasError())
    {
        errorMsg = "Error creating render target : " + newTarget->GetErrorMessage();
        return ERROR_ID;
    }
    if (!newTarget->IsValid())
    {
        errorMsg = "Error validating render target : " + newTarget->GetErrorMessage();
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

bool RenderTargetManager::ResizeTarget(unsigned int id, unsigned int w, unsigned int h, int colAttch)
{
    ClearAllRenderingErrors();

    //If the given render target doesn't exist, return an error.
    RenderTarget * old = operator[](id);
    if (old == 0)
    {
        errorMsg = "Error resizing render target " + std::to_string(id) + ": render target not found.";
        return false;
    }

    //Build the new color settings (identical to the old settings but with new texture sizes).
    std::vector<RendTargetColorTexSettings> cts = old->GetColorSettings();
    for (int i = 0; i < cts.size(); ++i)
    {
        if (colAttch < 0 || colAttch == i)
        {
            cts[i].Settings.Width = w;
            cts[i].Settings.Height = h;
        }
    }

    //Create the new target.
    RenderTarget * newTarget = new RenderTarget(cts, old->GetDepthSettings());
    if (newTarget->HasError())
    {
        errorMsg = "Error recreating render target " + std::to_string(id) + " to resize it: " + newTarget->GetErrorMessage();
        return false;
    }

    //Delete the old target.
    if (!DeleteRenderTarget(id))
    {
        errorMsg = "Error deleting old render target after creating newly-sized target: " + errorMsg;
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