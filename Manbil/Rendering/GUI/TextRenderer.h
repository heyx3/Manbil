#pragma once

#include "FreeTypeHandler.h"
#include "../Texture Management/RenderTargetManager.h"
#include "../Texture Management/MTexture2D.h"
#include "../Helper Classes/DrawingQuad.h"



//Handles rendering of strings into a render target using FreeType.
//The rendered strings are pure red; the red value indicates the alpha of the pixel.
//To use this class, first call "CreateAFont" and store the resulting ID.
//Then, create at least one render slot with "CreateTextRenderSlots".
//Finally, use a render slot by calling "RenderString", and get the result with "GetRenderedString".
//TODO: Support deleting of fonts and change the interface a bit so that all uses of this system are caught and can be fixed to ensure font deletion after use.
class TextRenderer
{
public:

    struct FontSlot
    {
        unsigned int FontID, SlotIndex;
        FontSlot(FreeTypeHandler::FontID fontID = FreeTypeHandler::ERROR_ID, unsigned int slotIndex = 0)
            : FontID(fontID), SlotIndex(slotIndex) { }
    };


    //Must be called before rendering any text.
    static std::string InitializeSystem(SFMLOpenGLWorld * world);
    //Must be called after rendering any text.
    static void DestroySystem(void);


    RenderTargetManager & RTManager;


    std::string GetError(void) const { return errorMsg; }
    bool HasError(void) const { return !errorMsg.empty(); }


    TextRenderer(RenderTargetManager & rtManager)
        : RTManager(rtManager)
    {

    }
    ~TextRenderer(void);


    //Returns the FreeTypeHandler font ID to use when referencing this font,
    //    or "FreeTypeHandler::ERROR_ID" if there was an error.
    unsigned int CreateAFont(std::string fontPath, unsigned int pixelWidth = 50, unsigned int pixelHeight = 0);
    //Creates the given number of render slots, all with the given width/height space.
    //Returns whether or not this function succeeded.
    bool CreateTextRenderSlots(FreeTypeHandler::FontID fontID,
                               unsigned int finalRenderWidth, unsigned int finalRenderHeight,
                               bool useMipmapping, const TextureSampleSettings2D & finalRenderSettings,
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
    Vector2u GetMaxCharacterSize(FreeTypeHandler::FontID id) const { return FreeTypeHandler::Instance.GetGlyphMaxSize(id); }
    //Gets the string currently being rendered at the given font/slot.
    //Returns 0 if the given font/slot doesn't exist.
    std::string GetString(FontSlot slot) const;
    //Gets the texture holding the rendered text from the given font/slot.
    //Returns 0 if the given font/slot doesn't exist.
    MTexture2D * GetRenderedString(FontSlot slot) const;

    //Renders the given string into the given slot.
    //Takes in the width and height to reset the back buffer to after rendering the text into the render target.
    bool RenderString(FontSlot slot, std::string textToRender,
                      unsigned int backBufferWidth = 0, unsigned int backBufferHeight = 0);


private:

    struct Slot
    {
        unsigned int RenderTargetID;
        MTexture2D * ColorTex;
        std::string String;
        unsigned int TextWidth, TextHeight;
    };
    typedef std::unordered_map<FreeTypeHandler::FontID, std::vector<Slot>>::const_iterator SlotCollectionLoc;

    std::unordered_map<FreeTypeHandler::FontID, std::vector<Slot>> fonts;
    mutable std::string errorMsg;


    //Tries to find the slots for the given font ID. If it is found, sets "outLoc" and returns true.
    //If it isn't, sets the error message and returns false.
    bool TryFindSlotCollection(FreeTypeHandler::FontID fontID, SlotCollectionLoc & outCollection) const;
    //Tries to find the given slot in the given vector of slots. If it is found, sets "outSlot" and returns true.
    //Otherwise, sets the error message and returns false.
    bool TryFindSlot(unsigned int slotNumb, const std::vector<Slot> & slots, const Slot *& outSlot) const;
    //Tries to find the given slot in the given vector of slots. If it is found, sets "outSlot" and returns true.
    //Otherwise, sets the error message and returns false.
    bool TryFindSlot(unsigned int slotNumb, std::vector<Slot> & slots, Slot *& outSlot);

    //Tries to find the given font/slot. If it is found, sets "outSlot" and returns true.
    //Otherwise, sets the error message and returns false.
    bool TryFindFontSlot(FontSlot slot, const Slot*& outSlot) const;
    //Tries to find the given font/slot. If it is found, sets "outSlot" and returns true.
    //Otherwise, sets the error message and returns false.
    bool TryFindFontSlot(FontSlot slot, Slot*& outSlot);


    //Renders the given string into the given render target, using the given font.
    bool RenderString(std::string string, FreeTypeHandler::FontID fontID, RenderTarget * finalRender,
                      unsigned int & outTextWidth, unsigned int & outTextHeight,
                      unsigned int backBufferWidth = 0, unsigned int backBufferHeight = 0);



    static MTexture2D tempTex;
    static Material* textRenderer;
    static DrawingQuad* textRendererQuad;
    static UniformDictionary textRendererParams;
    static RenderInfo textRendererInfo;
    static Camera textRendererCam;
    static TransformObject textRendererTransform;
    static Matrix4f worldMat, viewMat, projMat;

    static FreeTypeHandler & GetHandler(void) { return FreeTypeHandler::Instance; }
};