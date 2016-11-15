#pragma once

#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "../../Math/Lower Math/Vectors.h"
#include "../../Math/Lower Math/Array2D.h"


class MTexture2D;


//Wraps all data about a font's size.
struct FontSizeData
{
public:

    //At least one of the fields "CharWidth" and "CharHeight" must not be 0.
    //A value of 0 indicates that the value will match the other field's value.
    signed long CharWidth, CharHeight;
    //If both "HorizontalDPI" and "VerticalDPI" are 0, they will both default to 72.
    //Otherwise, a value of 0 indicates that the value will match the other field's value.
    unsigned int HorizontalDPI, VerticalDPI;

    FontSizeData(signed long charWidth = 0, signed long charHeight = 0,
                 unsigned int horizontalDPI = 0, unsigned int verticalDPI = 0)
        : CharWidth(charWidth), CharHeight(charHeight),
          HorizontalDPI(horizontalDPI), VerticalDPI(verticalDPI)
    {

    }
};


//Wraps usage of the FreeType library.
//Generally, any method that returns a boolean is returning whether or not that function succeeded.
//If a function fails, an error message will be exposed via GetError().
//This class is a singleton, and it should be checked for an error message
//    before it is used for the first time.
class FreeTypeHandler
{
public:

    static const unsigned int ERROR_ID = 0;

    typedef unsigned int FontID;

    static FreeTypeHandler Instance;


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }


    //Returns the ID of the loaded font. If font creation failed, returns ERROR_ID.
    //A value of 0 for "characterWidth" means it is the same size as "characterHeight",
    //   and vice-versa. They cannot both be 0.
    //Same goes for horizontal-/verticalResolution; additionally, if horizontal- AND verticalResolution
    //   are 0, they default to 72.
    FontID LoadFont(std::string path, FontSizeData dat, signed long faceIndex = 0);
    //Deletes the given font from memory.
    bool DeleteFont(FontID fontID);

    //Loads the texture for the given font and character.
    bool LoadGlyph(FontID id, unsigned int charCode);
    //Gets a bitmap image for the most recently-loaded glyph for the given font.
    //Returns 0 if the given font doesn't exist.
    const FT_Bitmap* GetGlyph(FontID id) const;

    //Attempts to set the font size for the given font.
    bool SetFontSize(FontID id, FontSizeData dat);
    //Attempts to directly set the pixel size for the given font's glyphs.
    bool SetFontSize(FontID id, unsigned int pixelWidth = 0, unsigned int pixelHeight = 0);

    //Gets the number of available glyphs for the given font face.
    //Returns 0 if the given id doesn't correspond to a font.
    unsigned int GetNumbGlyphs(FontID id) const;
    //Gets whether the given font can be scaled.
    //Returns false if the given font doesn't exist.
    bool GetCanBeScaled(FontID id) const;


    std::vector<FT_Bitmap_Size> GetSupportedSizes(FontID id);

    //Gets the width/height for the currently-loaded glyph for the given font.
    //Returns a width/height of 0 if the given font doesn't exist.
    Vector2i GetGlyphSize(FontID id) const;
    //Gets the maximum width/height of a glyph for the given font.
    //Returns a width/height of 0 if the given font doesn't exist.
    Vector2u GetGlyphMaxSize(FontID id) const;
    //Gets the offset for drawing the currently-loaded glyph for the given font.
    //Returns an offset of 0 if the given font doesn't exist.
    Vector2i GetGlyphOffset(FontID id) const;
    //Gets the amount to move to draw the next character.
    //Returns { 0, 0 } if the given font doesn't exist.
    Vector2i GetMoveToNextGlyph(FontID id) const;
    

    //The different outputs of "RenderChar()".
    enum CharRenderType
    {
        CRT_COLOR,
        CRT_GREYSCALE,
        CRT_ERROR,
    };
    //Renders the given character into a private color array that can be accessed through "GetChar()".
    //Returns the type of color the rendered character uses,
    //    or "CRT_ERROR" if there was an error loading the char.
    CharRenderType RenderChar(FontID fontID, unsigned int charToRender);

    //Gets whether the most recently-rendered char is greyscale or full RGB color.
    bool GetIsGreyscale(void) const { return isGreyscale; }

    //Gets the most recently-rendered character as a color array.
    //Returns 0 if the currently-rendered character is a greyscale character.
    const Array2D<Vector4b>* GetColorChar(void) const;
    //Gets the most recently-rendered character as a greyscale color array.
    //Returns 0 if the currently-rendered character is not a greyscale character.
    const Array2D<unsigned char>* GetGreyscaleChar(void) const;

    //Gets the most recently-rendered char and stores it into the given texture.
    //Returns whether the given out texture's data was successfully set.
    bool GetChar(MTexture2D& outTex) const;


private:

    typedef std::unordered_map<FontID, FT_Face>::const_iterator FaceMapLoc;
    std::unordered_map<FontID, FT_Face> faces;

    FontID nextID;
    bool TryFindID(FontID id, FaceMapLoc& outLoc) const;

    FT_Library ftLib;

    bool isGreyscale;
    Array2D<unsigned char> renderedTextGreyscale;
    Array2D<Vector4b> renderedTextColor;

    mutable std::string errorMsg;


    //This class is a singleton.
    FreeTypeHandler(void);
    ~FreeTypeHandler(void);
    FreeTypeHandler(const FreeTypeHandler& cpy) = delete;
    FreeTypeHandler(FreeTypeHandler&& cpy) = delete;
};