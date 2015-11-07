#include "FreeTypeHandler.h"

#include <iostream>
#include "../Textures/MTexture2D.h"


FreeTypeHandler FreeTypeHandler::Instance = FreeTypeHandler();


FreeTypeHandler::FontID FreeTypeHandler::LoadFont(std::string path, FontSizeData dat,
                                                  signed long faceIndex)
{
    FT_Face face;
    FT_Error err;

    //Create the font face.
    err = FT_New_Face(ftLib, path.c_str(), faceIndex, &face);
    if (err == FT_Err_Unknown_File_Format)
    {
        errorMsg = std::string("The file '") + path +
                        "' could be opened and read, but is not supported.";
        return ERROR_ID;
    }
    else if (err != 0)
    {
        errorMsg = std::string("Unkown error code when creating font: ") + std::to_string(err);
        return ERROR_ID;
    }

    //Set its size.
    err = FT_Set_Char_Size(face, dat.CharWidth, dat.CharHeight, dat.HorizontalDPI, dat.VerticalDPI);
    if (err != 0)
    {
        errorMsg = std::string("Unknown error when sizing text: ") + std::to_string(err);
        return ERROR_ID;
    }

    //Record it in the map of font faces.
    faces[nextID] = face;
    nextID += 1;
    return nextID - 1;
}
bool FreeTypeHandler::DeleteFont(FontID fontID)
{
    //Make sure the given font exists.
    FaceMapLoc location;
    if (!TryFindID(fontID, location))
    {
        return false;
    }
    const FT_Face& face = location->second;

    //Try to delete it.
    FT_Error err = FT_Done_Face(face);
    if (err != 0)
    {
        errorMsg = "Unknown error code when deleting font: " + std::to_string(err);
    }

    faces.erase(location);
    return true;
}

bool FreeTypeHandler::LoadGlyph(FontID id, unsigned int charCode)
{
    FT_Error err;

    //Make sure the given font exists.
    FaceMapLoc location;
    if (!TryFindID(id, location))
    {
        return false;
    }
    const FT_Face& face = location->second;

    //Make sure the given character exists.
    FT_UInt index = FT_Get_Char_Index(face, charCode);
    if (index == 0)
    {
        errorMsg = std::string("Character not found");
        return false;
    }

    //Load the glyph.
    err = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
    if (err != 0)
    {
        errorMsg = std::string("Error loading glyph: ") + std::to_string(err);
        return false;
    }

    //If it isn't a bitmap, render it to a bitmap.
    if (face->glyph->format != FT_Glyph_Format::FT_GLYPH_FORMAT_BITMAP)
    {
        err = FT_Render_Glyph(face->glyph, FT_Render_Mode::FT_RENDER_MODE_NORMAL);
        if (err != 0)
        {
            errorMsg = std::string("Error rendering glyph to bitmap: ") + std::to_string(err);
            return false;
        }
    }

    return true;
}
const FT_Bitmap* FreeTypeHandler::GetGlyph(FontID id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return 0;
    }

    return &loc->second->glyph->bitmap;
}

bool FreeTypeHandler::SetFontSize(FontID id, FontSizeData dat)
{
    //Try to find the font face with the given ID.
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return false;
    }

    //Try to set its size.
    FT_Error err = FT_Set_Char_Size(loc->second, dat.CharWidth, dat.CharHeight,
                                    dat.HorizontalDPI, dat.VerticalDPI);
    if (err != 0)
    {
        errorMsg = std::string("Freetype error when sizing font ") + std::to_string(id) + ": " +
                        std::to_string(err) +
                        ". Did you try to set the pixel size to an unsupported value?";
        return false;
    }

    return true;
}
bool FreeTypeHandler::SetFontSize(FontID id, unsigned int pixelWidth, unsigned int pixelHeight)
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return false;
    }

    FT_Error err = FT_Set_Pixel_Sizes(loc->second, pixelWidth, pixelHeight);
    if (err != 0)
    {
        errorMsg = std::string("Unknown FreeType error ") + std::to_string(err) +
                       ". Did you try to set the pixel size to an unsupported value?";
        return false;
    }

    return true;
}

unsigned int FreeTypeHandler::GetNumbGlyphs(FontID id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return 0;
    }

    return loc->second->num_glyphs;
}
bool FreeTypeHandler::GetCanBeScaled(FontID id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return false;
    }

    return (bool)(loc->second->face_flags | FT_FACE_FLAG_SCALABLE);
}
std::vector<FT_Bitmap_Size> FreeTypeHandler::GetSupportedSizes(FontID id)
{
    std::vector<FT_Bitmap_Size> ret;

    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return ret;
    }

    //Copy the data into the vector.
    ret.resize(loc->second->num_fixed_sizes);
    memcpy(ret.data(), loc->second->available_sizes,
           sizeof(FT_Bitmap_Size) * ret.size());
    return ret;
}

Vector2i FreeTypeHandler::GetGlyphSize(FontID id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return Vector2i();
    }

    return Vector2i(loc->second->glyph->bitmap.width,
                    loc->second->glyph->bitmap.rows);
}
Vector2u FreeTypeHandler::GetGlyphMaxSize(FontID id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return Vector2u();
    }

    return Vector2u(Mathf::Abs(loc->second->bbox.xMax - loc->second->bbox.xMin),
                    Mathf::Abs(loc->second->bbox.yMax - loc->second->bbox.yMin));
}
Vector2i FreeTypeHandler::GetGlyphOffset(FontID id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return Vector2i();
    }

    return Vector2i(loc->second->glyph->bitmap_left,
                    loc->second->glyph->bitmap_top);
}
Vector2i FreeTypeHandler::GetMoveToNextGlyph(FontID id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc))
    {
        return Vector2i();
    }

    return Vector2i(loc->second->glyph->advance.x >> 6,
                    loc->second->glyph->advance.y >> 6);
}

FreeTypeHandler::CharRenderType FreeTypeHandler::RenderChar(FontID fontID, unsigned int charToRender)
{
    FaceMapLoc loc;
    if (!TryFindID(fontID, loc))
    {
        return CharRenderType::CRT_ERROR;
    }

    FT_Face fce = loc->second;
    FT_Error err;
    FT_UInt charIndex;

    //Make sure the given character exists.
    charIndex = FT_Get_Char_Index(fce, charToRender);
    if (charIndex == 0)
    {
        errorMsg = std::string("Character not found");
        return CharRenderType::CRT_ERROR;
    }

    //Load the glyph.
    err = FT_Load_Glyph(fce, charIndex, FT_LOAD_DEFAULT);
    if (err != 0)
    {
        errorMsg = std::string("Error loading glyph: ") + std::to_string(err);
        return CharRenderType::CRT_ERROR;
    }

    //If it isn't a bitmap, render it to a bitmap.
    if (fce->glyph->format != FT_Glyph_Format::FT_GLYPH_FORMAT_BITMAP)
    {
        err = FT_Render_Glyph(fce->glyph, FT_Render_Mode::FT_RENDER_MODE_NORMAL);
        if (err != 0)
        {
            errorMsg = std::string("Error rendering glyph to bitmap: ") + std::to_string(err);
            return CharRenderType::CRT_ERROR;
        }
    }

    //Different pixel values work differently.
    char pixelMode = fce->glyph->bitmap.pixel_mode;
    unsigned int absPitch = (unsigned int)Mathf::Abs(fce->glyph->bitmap.pitch);
    if (pixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_MONO)
    {
        //Each pixel is 1 bit.

        isGreyscale = true;
        renderedTextColor.Reset(1, 1);

        renderedTextGreyscale.Reset(fce->glyph->bitmap.width, fce->glyph->bitmap.rows);
        renderedTextGreyscale.FillFunc([&fce, absPitch](Vector2u loc, unsigned char* outP)
        {
            unsigned int index = (loc.x / 8) + (loc.y * absPitch);
            int bit = loc.x % 8;

            unsigned char value = fce->glyph->bitmap.buffer[index] | (128 >> bit);
            value = value << bit;
            value *= 255;

            *outP = value;
        });

        return CharRenderType::CRT_GREYSCALE;
    }
    if (pixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_GRAY)
    {
        //Each pixel is 1 byte.

        isGreyscale = false;
        renderedTextGreyscale.Reset(1, 1);

        //TODO: Figure out why this mode isn't working correctly.
        renderedTextColor.Reset(fce->glyph->bitmap.width, fce->glyph->bitmap.rows);
        renderedTextColor.FillFunc([&fce, absPitch](Vector2u loc, Vector4b* outP)
        {
            loc.y = fce->glyph->bitmap.rows - loc.y - 1;
            unsigned int index = loc.x + (loc.y * absPitch);
            unsigned char value = fce->glyph->bitmap.buffer[index];

            *outP = Vector4b(value, value, value, value);
        });

        return CharRenderType::CRT_COLOR;
    }
    if (pixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_LCD)
    {
        //Each pixel is 3 components, one byte per component.

        isGreyscale = false;

        renderedTextColor.Reset(fce->glyph->bitmap.width, fce->glyph->bitmap.rows);
        renderedTextColor.FillFunc([&fce, absPitch](Vector2u loc, Vector4b* outP)
        {
            unsigned int index = (loc.x * 3) + (loc.y * absPitch);
            *outP = Vector4b(fce->glyph->bitmap.buffer[index],
                             fce->glyph->bitmap.buffer[index + 1],
                             fce->glyph->bitmap.buffer[index + 2],
                             255);
        });

        return CharRenderType::CRT_COLOR;
    }
    else
    {
        errorMsg = "The pixel mode '" + std::to_string(pixelMode) + "' is not supported.";
        return CharRenderType::CRT_ERROR;
    }
}

const Array2D<Vector4b>* FreeTypeHandler::GetColorChar(void) const
{
    if (isGreyscale)
    {
        return 0;
    }
    return &renderedTextColor;
}
const Array2D<unsigned char>* FreeTypeHandler::GetGreyscaleChar(void) const
{
    if (!isGreyscale)
    {
        return 0;
    }
    return &renderedTextGreyscale; 
}

bool FreeTypeHandler::GetChar(MTexture2D& outTex) const
{
    if (isGreyscale)
    {
        if (!outTex.SetGreyscaleData(renderedTextGreyscale, PixelSizes::PS_8U_GREYSCALE))
        {
            errorMsg = "Error setting greyscale texture data.";
            return false;
        }
    }
    else
    {
        if (!outTex.SetColorData(renderedTextColor, PixelSizes::PS_8U))
        {
            errorMsg = "Error setting color texture data.";
            return false;
        }
    }

    return true;
}

bool FreeTypeHandler::TryFindID(FontID id, FaceMapLoc& outLoc) const
{
    outLoc = faces.find(id);
    if (outLoc == faces.end())
    {
        errorMsg = std::string() + "Unknown id " + std::to_string(id);
        return false;
    }
    return true;
}

FreeTypeHandler::FreeTypeHandler(void)
    : nextID(1), renderedTextColor(1, 1), renderedTextGreyscale(1, 1)
{
    FT_Error err = FT_Init_FreeType(&ftLib);
    if (err != 0)
    {
        errorMsg = std::string("Error initializing FreeType library: ") + std::to_string(err);
        std::cout << errorMsg << "\n\n";
    }
}
FreeTypeHandler::~FreeTypeHandler(void)
{
    FT_Error err = FT_Done_FreeType(ftLib);

    if (err == 0)
    {
        return;
    }

    std::cout << "Error ending freetype library: " << std::to_string(err);
    char dummy;
    std::cin >> dummy;
}