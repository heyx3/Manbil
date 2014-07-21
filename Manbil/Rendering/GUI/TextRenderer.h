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
class TextRenderer
{
public:

    struct FontSlot
    {
        unsigned int FontID, SlotIndex;
        FontSlot(unsigned int fontID = FreeTypeHandler::ERROR_ID, unsigned int slotIndex = 0) : FontID(fontID), SlotIndex(slotIndex) { }
    };


    //TODO: Support text wrapping by changing the rendering algorithm. Render each character of a word into a separate texture and track the layout info, then see if the word will go off the right side of the texture. If so, start a new line there (also start a new line if the '\n' character is found).
    //MTexture2D GetCharTextureHolder(unsigned int texture) { if (textures.size() <= texture) textures.insert(etc.....


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


    //Returns the FreeTypeHandler font ID to use when referencing this font, or "FreeTypeHandler::ERROR_ID" if there was an error.
    unsigned int CreateAFont(std::string fontPath, unsigned int pixelWidth = 50, unsigned int pixelHeight = 0);
    //Creates the given number of render slots, all with the given width/height space.
    //Returns whether or not this function succeeded.
    bool CreateTextRenderSlots(unsigned int fontID, unsigned int finalRenderWidth, unsigned int finalRenderHeight,
                               bool useMipmapping, TextureSampleSettings2D & finalRenderSettings, unsigned int numbSlots = 1);

    //Gets whether the given slot exists.
    bool DoesSlotExist(FontSlot slot) const;

    //Gets the number of slots currently available for the given font. Returns -1 if the given font doesn't exist.
    int GetNumbSlots(unsigned int fontID) const;
    //Gets the size of the given font/slot's final text render target. Returns { 0, 0 } if the given font/slot doesn't exist.
    Vector2i GetSlotRenderSize(FontSlot slot) const;
    //Gets the bounding box size of the currently-rendered text in the given font/slot. Returns { 0, 0 } if the given font/slot doesn't exist.
    Vector2i GetSlotBoundingSize(FontSlot slot) const;
    //Gets the string currently being rendered at the given font/slot. Returns 0 if the given font/slot doesn't exist.
    const char * GetString(FontSlot slot) const;
    //Gets the texture holding the rendered text from the given font/slot. Returns 0 if the given font/slot doesn't exist.
    MTexture2D * GetRenderedString(FontSlot slot) const;

    //Renders the given string into the given slot.
    //Takes in the width and height to reset the back buffer to after rendering the text into the render target.
    bool RenderString(FontSlot slot, std::string textToRender, unsigned int backBufferWidth, unsigned int backBufferHeight);

    //Renders the given string using the given font/slot, material, and parameters.
    //TODO: Implement for convenience/efficiency. Don't set any OpenGL state that isn't specified in this function's arguments.
    //bool RenderString(FontSlot slot, std::string textToRender, RenderInfo & info, Material * toRender, std::string textSamplerUniformName, UniformDictionary & params);


private:

    struct Slot
    {
        unsigned int RenderTargetID;
        MTexture2D * ColorTex;
        const char * String;
        unsigned int TextWidth, TextHeight;
    };
    typedef std::unordered_map<unsigned int, std::vector<Slot>>::const_iterator SlotCollectionLoc;

    std::unordered_map<unsigned int, std::vector<Slot>> fonts;
    mutable std::string errorMsg;


    //Tries to find the slots for the given font ID. If it is found, sets "outLoc" and returns true.
    //If it isn't, sets the error message and returns false.
    bool TryFindSlotCollection(unsigned int fontID, SlotCollectionLoc & outCollection) const;
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
    bool RenderString(std::string string, unsigned int fontID, RenderTarget * finalRender, unsigned int backBufferWidth, unsigned int backBufferHeight);



    static MTexture2D tempTex;
    static Material * textRenderer;
    static DrawingQuad * textRendererQuad;
    static UniformDictionary textRendererParams;
    static RenderInfo textRendererInfo;
    static Camera textRendererCam;
    static TransformObject textRendererTransform;
    static Matrix4f worldMat, viewMat, projMat;

    static FreeTypeHandler & GetHandler(void) { return FreeTypeHandler::Instance; }
};