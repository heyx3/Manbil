#pragma once

#include <string>
#include "../../RenderTarget.h"


//Manages render targets for material passes.
class RenderTargetPool
{
public:

    static bool HasError(void) { return !errorMsg.empty(); }
    static std::string GetErrorMessage(void) { return errorMsg; }
    static void ClearErrorMessage(void) { errorMsg.clear(); }


    static bool IsInitialized(void) { isInitialized; }
    static bool InitializePool(bool color, bool depth, unsigned int width, unsigned int height);
    static void DestroyPool(void);

    static void ResizePool(unsigned int width, unsigned int height);

    static RenderTarget * GetScreenOne(void) { return screenOne; }
    static RenderTarget * GetScreenTwo(void) { return screenTwo; }
    static RenderTarget * GetScreenThree(void) { return screenThree; }

private:

    static RenderTarget * screenOne, * screenTwo, * screenThree;

    static std::string errorMsg;

    static bool isInitialized;
};