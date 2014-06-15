#pragma once

#include "FreeTypeHandler.h"
#include "../Texture Management/RenderTargetManager.h"
#include "../Texture Management/TextureManager.h"
#include "../Helper Classes/DrawingQuad.h"


#pragma warning(disable: 4512)


//Handles rendering of strings into a render target using FreeType.
//The rendered strings are pure red; the red value indicates the alpha of the pixel.
//To use this class, first call "CreateAFont" and store the resulting ID.
//Then, create at least one render slot with "CreateTextRenderSlots".
//Finally, use a render slot by calling "RenderString", and get the result with "GetRenderedString".
class TextRenderer
{
public:

    //TODO: Support for "\n".

    //Must be called before rendering any text.
    static std::string InitializeSystem(SFMLOpenGLWorld * world);
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

    //Returns the FreeTypeHandler font ID to use when referencing this font, or "FreeTypeHandler::ERROR_ID" if there was an error.
    unsigned int CreateAFont(std::string fontPath, unsigned int pixelWidth = 50, unsigned int pixelHeight = 0);
    //Creates the given number of render slots, all with the given width/height space.
    //Returns whether or not this function succeeded.
    bool CreateTextRenderSlots(unsigned int fontID, unsigned int finalRenderWidth, unsigned int finalRenderHeight, TextureSettings & finalRenderSettings, unsigned int numbSlots = 1);

    //Gets the number of slots currently available for the given font. Returns -1 if the given font doesn't exist.
    int GetNumbSlots(unsigned int fontID) const;
    //Gets the size of the given font/slot's final text render target. Returns { 0, 0 } if the given font/slot doesn't exist.
    Vector2i GetSlotRenderSize(unsigned int fontID, unsigned int slotIndex = 0) const;
    //Gets the bounding box size of the currently-rendered text in the given font/slot. Returns { 0, 0 } if the given font/slot doesn't exist.
    Vector2i GetSlotBoundingSize(unsigned int fontID, unsigned int slotIndex = 0) const;
    //Gets the string currently being rendered at the given font/slot. Returns 0 if the given font/slot doesn't exist.
    const char * GetString(unsigned int fontID, unsigned int slotIndex = 0) const;
    //Gets the texture holding the rendered text from the given font/slot. Returns ManbilTexture() if the given font/slot doesn't exist.
    ManbilTexture GetRenderedString(unsigned int fontID, unsigned int slot = 0) const;

    //Renders the given string into the given slot.
    //Takes in the width and height to reset the back buffer to after rendering the text into the render target.
    bool RenderString(unsigned int fontID, unsigned int slot, std::string textToRender, unsigned int backBufferWidth, unsigned int backBufferHeight);

    //Renders the given string using the given meshes, material, and parameters,
    //   along with the slot to put the character textures into.
    //TODO: Implement for convenience/efficiency.
    //bool RenderString(unsigned int slot, std::string textToRender, RenderInfo & info, UniformSamplerValue & textureIn,
    //                  const std::vector<const Mesh*> & meshes, Material * toRender, UniformDictionary & params);


private:

    //TODO: Only need one temp texture for the whole instance, not per font.

    struct Slot { unsigned int RenderTargetID; const char * String; unsigned int Width, Height, TextWidth, TextHeight; };
    struct SlotCollection { std::vector<Slot> Slots; unsigned int TexID; };
    std::unordered_map<unsigned int, SlotCollection> slots; //PRIORITY: Rename to "fonts".



    typedef std::unordered_map<unsigned int, SlotCollection>::const_iterator SlotCollectionLoc;

    //Tries to find the slots for the given font ID. If it is found, sets "outLoc" and returns true.
    //If it isn't, sets the error message and returns false.
    bool TryFindSlotCollection(unsigned int fontID, SlotCollectionLoc & outCollection) const;
    //Tries to find the given slot in the given vector of slots. If it is found, sets "outSlot" and returns true.
    //Otherwise, sets the error message and returns false.
    bool TryFindSlot(unsigned int slotNumb, const std::vector<Slot> & slots, const Slot *& outSlot) const;
    //Tries to find the given slot in the given vector of slots. If it is found, sets "outSlot" and returns true.
    //Otherwise, sets the error message and returns false.
    bool TryFindSlot(unsigned int slotNumb, std::vector<Slot> & slots, Slot *& outSlot);



    mutable std::string errorMsg;


    static Material * textRenderer;
    static DrawingQuad * textRendererQuad;
    static UniformDictionary textRendererParams;
    static RenderInfo textRendererInfo;
    static Camera textRendererCam;
    static TransformObject textRendererTransform;
    static Matrix4f worldMat, viewMat, projMat;

    static FreeTypeHandler & GetHandler(void) { return FreeTypeHandler::Instance; }

    //Renders the given string into the given render target, using the given font and given temporary texture.
    bool RenderString(std::string string, unsigned int fontID, ManbilTexture tex, RenderTarget * finalRender, unsigned int backBufferWidth, unsigned int backBufferHeight);
};

#pragma warning(default: 4512)