#include "RenderTargetPool.h"

bool RenderTargetPool::InitializePool(bool color, bool depth, unsigned int width, unsigned int height)
{
    screenOne = new RenderTarget(width, height, color, depth);
    screenTwo = new RenderTarget(width, height, color, depth);
    screenThree = new RenderTarget(width, height, color, depth);

    errorMsg = "";

    if (screenOne->HasError())
    {
        errorMsg += "Error creating first render target: ";
        errorMsg += screenOne->GetErrorMessage();
    }
    if (screenTwo->HasError())
    {
        errorMsg += "Error creating second render target: ";
        errorMsg += screenTwo->GetErrorMessage();
    }
    if (screenThree->HasError())
    {
        errorMsg += "Error creating third render target: ";
        errorMsg += screenThree->GetErrorMessage();
    }

    isInitialized = errorMsg.empty();
    return isInitialized;
}

void RenderTargetPool::DestroyPool(void)
{
    delete screenOne, screenTwo, screenThree;

    screenOne = 0;
    screenTwo = 0;
    screenThree = 0;

    isInitialized = false;
}

void RenderTargetPool::ResizePool(unsigned int w, unsigned int h)
{
    if (!isInitialized) return;

    screenOne->ChangeSize(w, h);
    screenTwo->ChangeSize(w, h);
    screenThree->ChangeSize(w, h);
}