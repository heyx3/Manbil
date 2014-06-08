#pragma once

#include "FreeTypeHandler.h"
#include "../Texture Management/TextureManager.h"


//Handles rendering of strings using FreeType.
//Strings are rendered into textures, and the textures are stored in a texture manager.
class TextRenderer
{
public:

    FreeTypeHandler & FTHandler;
    TextureManager & TexManager;


    std::string GetError(void) const { return errorMsg; }
    bool HasError(void) const { return !errorMsg.empty(); }


    TextRenderer(FreeTypeHandler & ftHandler, TextureManager & texManager)
        : FTHandler(ftHandler), TexManager(texManager)
    {

    }

    //Returns the ID to use when referencing this slot, or "FreeTypeHandler::ERROR_ID" if there was an error.
    unsigned int CreateTextRenderSlot(std::string fontPath, TextureSettings settings, unsigned int pixelWidth = 100, unsigned int pixelHeight = 0);


private:

    std::string errorMsg;
    std::unordered_map<unsigned int, unsigned int> fontIDToTexID;
};