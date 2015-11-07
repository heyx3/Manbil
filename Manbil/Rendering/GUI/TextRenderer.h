#pragma once

#include "FreeTypeHandler.h"
#include "../Management/RenderTargetManager.h"
#include "../Textures/MTexture2D.h"
#include "../Primitives/DrawingQuad.h"



//Handles rendering of strings into a render target using FreeType.
//The rendered string textures only have a greyscale (i.e. red) component,
//    storing the transparency of each pixel.
//To use this class, first call "CreateAFont()" and store the resulting FontID.
//Then, create at least one render slot with "CreateTextRenderSlots()".
//Finally, render a string into a slot by calling "RenderString()",
//    and get the result with "GetRenderedString()".
//TODO: Support deleting of fonts and change the interface a bit so that all uses of this system are caught and can be fixed to ensure font deletion after use.
class TextRenderer
{
public:

    struct FontSlot
    {
        unsigned int FontID, SlotIndex;
        FontSlot(FreeTypeHandler::FontID fontID = FreeTypeHandler::ERROR_ID,
                 unsigned int slotIndex = 0)
            : FontID(fontID), SlotIndex(slotIndex) { }
    };


    //Must be called before rendering any text.
    static std::string InitializeSystem(void);
    //Must be called after this system will no longer be needed.
    static void DestroySystem(void);


    //TODO: Don't need more than one render target; just manually hold onto a render target instance and swap out its color attachment.
    RenderTargetManager RTManager;


    TextRenderer(void) : RTManager() { }
    ~TextRenderer(void);


    //Returns the FreeTypeHandler font ID to use when referencing this font,
    //    or "FreeTypeHandler::ERROR_ID" if there was an error.
    unsigned int CreateAFont(std::string fontPath, std::string& outErrorMsg,
                             unsigned int pixelWidth = 50, unsigned int pixelHeight = 0);
    //Creates the given number of render slots, all with the given width/height space.
    //Returns whether or not this function succeeded.
    bool CreateTextRenderSlots(FreeTypeHandler::FontID fontID, std::string& outErrorMsg,
                               unsigned int finalRenderWidth, unsigned int finalRenderHeight,
                               bool useMipmapping, const TextureSampleSettings2D& finalRenderSettings,
                               unsigned int numbSlots = 1);

    //Gets whether the given slot exists.
    bool DoesSlotExist(FontSlot slot) const;

    //Gets the number of slots currently available for the given font.
    //Returns -1 if the given font doesn't exist.
    int GetNumbSlots(FreeTypeHandler::FontID fontID) const;
    //Gets the size of the given font/slot's final text render target.
    //Returns { 0, 0 } if the given font/slot doesn't exist.
    Vector2i GetSlotRenderSize(FontSlot slot) const;
    //Gets the bounding box size of the currently-rendered text in the given font/slot.
    //Returns { 0, 0 } if the given font/slot doesn't exist.
    Vector2i GetSlotBoundingSize(FontSlot slot) const;
    //Gets the size of the maximum-possible glyph in the given font.
    //Returns {0, 0 } if the given font doesn't exist.
    Vector2u GetMaxCharacterSize(FontSlot slot) const { return GetMaxCharacterSize(slot.FontID); }
    //Gets the size of the maximum-possible glyph in the given font.
    //Returns {0, 0 } if the given font doesn't exist.
    Vector2u GetMaxCharacterSize(FreeTypeHandler::FontID id) const;
    //Gets the string currently being rendered at the given font/slot.
    //Returns 0 if the given font/slot doesn't exist.
    std::string GetString(FontSlot slot) const;
    //Gets the texture holding the rendered text from the given font/slot.
    //Returns 0 if the given font/slot doesn't exist.
    MTexture2D* GetRenderedString(FontSlot slot) const;

    //Renders the given string into the given slot.
    //Takes in the width and height to reset the back buffer to
    //    after rendering the text into the render target.
    //Returns whether it was successful.
    //TODO: remove "backBufferWidth/Height" vars.
    bool RenderString(FontSlot slot, std::string textToRender,
                      unsigned int backBufferWidth = 0, unsigned int backBufferHeight = 0);


private:

    struct Slot
    {
        unsigned int RenderTargetID;
        MTexture2D* ColorTex;
        std::string String;
        unsigned int TextWidth, TextHeight;
    };
    typedef std::unordered_map<FreeTypeHandler::FontID, std::vector<Slot>> FontCollection;
    typedef FontCollection::const_iterator SlotCollection;

    std::unordered_map<FreeTypeHandler::FontID, std::vector<Slot>> fonts;


    //The following helper functions handle searching through the collection of fonts and slots.

    //Tries to find the slots for the given font ID. If it is found, sets "outLoc" and returns true.
    //If it isn't, returns false.
    bool TryFindSlotCollection(FreeTypeHandler::FontID fontID, SlotCollection& outCollection) const;
    //Tries to find the given slot in the given collection of slots.
    //If it is found, sets "outSlot" to point to the found slot and returns true.
    //Otherwise, returns false.
    bool TryFindSlot(unsigned int slotIndex, const std::vector<Slot>& slots, const Slot*& outSlot) const;
    //Tries to find the given slot in the given vector of slots.
    //If it is found, sets "outSlot" and returns true.
    //Otherwise, returns false.
    bool TryFindSlot(unsigned int slotNumb, std::vector<Slot>& slots, Slot*& outSlot);

    //Tries to find the given slot in the given font. If it is found, sets "outSlot" and returns true.
    //Otherwise, returns false.
    bool TryFindFontSlot(FontSlot slot, const Slot*& outSlot) const;
    //Tries to find the given font/slot. If it is found, sets "outSlot" and returns true.
    //Otherwise, returns false.
    bool TryFindFontSlot(FontSlot slot, Slot*& outSlot);


    //Renders the given string into the given render target, using the given font.
    bool RenderString(std::string string, FreeTypeHandler::FontID fontID, RenderTarget* finalRender,
                      unsigned int& outTextWidth, unsigned int& outTextHeight,
                      unsigned int backBufferWidth = 0, unsigned int backBufferHeight = 0);



    static MTexture2D tempTex;
    static Material* textRenderer;
    static UniformDictionary textRendererParams;
    static RenderInfo textRendererInfo;
    static Camera textRendererCam;
    static Matrix4f viewMat, projMat;

    static FreeTypeHandler& GetHandler(void) { return FreeTypeHandler::Instance; }
};