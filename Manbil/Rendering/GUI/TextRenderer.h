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
class TextRenderer
{
public:

    struct FontSlot
    {
        FreeTypeHandler::FontID FontID, SlotID;

        FontSlot(FreeTypeHandler::FontID fontID = FreeTypeHandler::ERROR_ID,
                 unsigned int slotID = 0)
            : FontID(fontID), SlotID(slotID) { }
    };


    //Must be called before rendering any text.
    static std::string InitializeSystem(void);
    //Must be called after this system will no longer be needed.
    static void DestroySystem(void);


    //TODO: Don't need more than one render target; just manually hold onto a render target instance and swap out its color attachment.
    RenderTargetManager RTManager;


    TextRenderer(void) : RTManager() { }
    ~TextRenderer(void);

    TextRenderer(const TextRenderer& cpy) = delete;


    //Returns the FreeTypeHandler font ID to use when referencing this font,
    //    or "FreeTypeHandler::ERROR_ID" if there was an error.
    unsigned int CreateAFont(std::string fontPath, std::string& outErrorMsg,
                             unsigned int pixelWidth = 50, unsigned int pixelHeight = 0);
    //returns whether the given font was found and deleted.
    //Note that all associated slots will be deleted as well.
    bool DeleteFont(FreeTypeHandler::FontID font);

    //Creates a slot for rendering text in the given font.
    //Returns the slot. If an error occurred, outputs text into the given "error message" string.
    FontSlot CreateTextRenderSlot(FreeTypeHandler::FontID fontID, std::string& outErrorMsg,
                                  unsigned int finalRenderWidth, unsigned int finalRenderHeight,
                                  bool useMipmapping, TextureSampleSettings2D finalRenderSettings);
    //Returns whether the given slot was found and deleted.
    bool DeleteTextRenderSlot(FontSlot slot);

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
    //Returns whether it was successful.
    bool RenderString(FontSlot slot, std::string textToRender);


private:

    struct Slot
    {
        unsigned int RenderTargetID;
        MTexture2D* ColorTex;
        std::string String;
        unsigned int TextWidth, TextHeight;
    };
    typedef std::unordered_map<FreeTypeHandler::FontID,
                               std::unordered_map<unsigned int, Slot>> FontCollection;
    typedef FontCollection::mapped_type SlotCollection;

    
    static FreeTypeHandler& GetHandler(void) { return FreeTypeHandler::Instance; }


    static MTexture2D tempTex;
    static Material* textRenderer;
    static UniformDictionary textRendererParams;
    static RenderInfo textRendererInfo;
    static Camera textRendererCam;
    static Matrix4f viewMat, projMat;


    FontCollection fonts;


    //Renders the given string into the given render target, using the given font.
    //Returns whether the operation was a success.
    bool RenderString(std::string string, FreeTypeHandler::FontID fontID, RenderTarget* finalRender,
                      unsigned int& outTextWidth, unsigned int& outTextHeight);


    //The following helper functions search through the various fonts and slots.
    //Each function has a const and non-const version.
    
    //Tries to find the slots for the given font ID.
    //Returns the found slot collection, or 0 if it wasn't found.
    const SlotCollection* TryFindSlotCollection(FreeTypeHandler::FontID fontID) const;
    //Tries to find the given slot in the given collection of slots.
    //Returns the found slot, or 0 if it wasn't found.
    const Slot* TryFindSlot(unsigned int slotID, const SlotCollection* slots) const;
    //Tries to find the given font/slot. Returns the found slot, or 0 if it wasn't found.
    const Slot* TryFindFontSlot(FontSlot slot) const;
    
    //Tries to find the slots for the given font ID.
    //Returns the found slot collection, or 0 if it wasn't found.
    SlotCollection* TryFindSlotCollection(FreeTypeHandler::FontID fontID);
    //Tries to find the given slot in the given collection of slots.
    //Returns the found slot, or 0 if it wasn't found.
    Slot* TryFindSlot(unsigned int slotIndex, SlotCollection* slots);
    //Tries to find the given font/slot. Returns the found slot, or 0 if it wasn't found.
    Slot* TryFindFontSlot(FontSlot slot);
};