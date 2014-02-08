#pragma once

#include <string>


//Manages render targets for material passes.
class RenderTargetPool
{
public:

    static bool HasError(void) { return !errorMsg.empty(); }
    static std::string GetErrorMessage(void) { return errorMsg; }
    static void ClearErrorMessage(void) { errorMsg.clear(); }


    static bool IsInitialized(void) { isInitialized; }
    static bool InitializePool(void);
    static bool DestroyPool(void);

private:

    

    static std::string errorMsg;

    static bool isInitialized;
};