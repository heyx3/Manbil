#pragma once

#include "FreeTypeHandler.h"
#include "../Texture Management/RenderTargetManager.h"
#include "../Texture Management/TextureManager.h"
#include "../Helper Classes/DrawingQuad.h"


#pragma warning(disable: 4512)

//Handles rendering of strings into a render target using FreeType.
//The rendered strings are greyscale; the greyscale value indicates the alpha of the pixel.
//Use this class by reserving slots to render text into by calling "CreateTextRenderSlot".
//Actually render text via "RenderString".
//Get the output of that render via "GetRenderedString".
class TextRenderer
{
public:

    //Must be called before rendering any text.
    static std::string InitializeSystem(void);
    //Must be called after rendering any text.
    static void DestroySystem(void);


    RenderTargetManager & RTManager;
    TextureManager & TexManager;


    std::string GetError(void) const { return errorMsg; }
    bool HasError(void) const { return !errorMsg.empty(); }


    TextRenderer(RenderTargetManager & rtManager, TextureManager & texManager)
        : RTManager(rtManager), TexManager(texManager)
    {

    }

    //Returns the ID to use when referencing this slot, or "FreeTypeHandler::ERROR_ID" if there was an error.
    unsigned int CreateTextRenderSlot(std::string fontPath, TextureSettings settings, unsigned int pixelWidth = 100, unsigned int pixelHeight = 0);

    //Renders the given string into the given slot.
    bool RenderString(unsigned int slot, const char * textToRender);
    //Gets the texture holding the rendered text from the given slot.
    RenderObjHandle GetRenderedString(unsigned int slot) const;
    //Gets the string currently being rendered at the given slot.
    const char * GetString(unsigned int slot) const;


private:

    struct Slot { unsigned int TexID, RenderTargetID; const char * String; };
    std::unordered_map<unsigned int, Slot> slots;

    typedef std::unordered_map<unsigned int, Slot>::const_iterator SlotMapLoc;


    //Tries to find the given slot. If it is found, sets "outLoc" and returns true.
    //If it isn't, sets the error message and returns false.
    bool TryFindSlot(unsigned int slot, SlotMapLoc & outLoc) const;


    mutable std::string errorMsg;


    static Material * textRenderer;
    static DrawingQuad * textRendererQuad;
    static UniformDictionary textRendererParams;

    static FreeTypeHandler & GetHandler(void) { return FreeTypeHandler::Instance; }
};

#pragma warning(default: 4512)