#include "FreeTypeHandler.h"

#include <iostream>
#include "../Texture Management/MTexture.h"


FreeTypeHandler FreeTypeHandler::Instance = FreeTypeHandler();


unsigned int FreeTypeHandler::LoadFont(std::string path, FontSizeData dat, signed long faceIndex)
{
    FT_Face face;
    FT_Error err;

    //Create the font face.
    err = FT_New_Face(ftLib, path.c_str(), faceIndex, &face);
    if (err == FT_Err_Unknown_File_Format)
    {
        errorMsg = std::string() + "The file '" + path + "' could be opened and read, but is not supported.";
        return ERROR_ID;
    }
    else if (err != 0)
    {
        errorMsg = std::string() + "Unkown error code when creating font: " + std::to_string(err);
        return ERROR_ID;
    }

    //Set its size.
    err = FT_Set_Char_Size(face, dat.CharWidth, dat.CharHeight, dat.HorizontalDPI, dat.VerticalDPI);
    if (err != 0)
    {
        errorMsg = std::string() + "Unknown error when sizing text: " + std::to_string(err);
        return ERROR_ID;
    }

    //Record it in the map of font faces.
    faces[nextID] = face;
    nextID += 1;
    return nextID - 1;
}

bool FreeTypeHandler::LoadGlyph(unsigned int id, unsigned int charCode)
{
    FT_Error err;

    //Make sure the given font exists.
    FaceMapLoc location;
    if (!TryFindID(id, location)) return false;
    const FT_Face & face = location->second;

    //Make sure the given character exists.
    FT_UInt index = FT_Get_Char_Index(face, charCode);
    if (index == 0)
    {
        errorMsg = std::string() + "Character not found";
        return false;
    }

    //Load the glyph.
    err = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
    if (err != 0)
    {
        errorMsg = std::string() + "Error loading glyph: " + std::to_string(err);
        return false;
    }

    //If it isn't a bitmap, render it to a bitmap.
    if (face->glyph->format != FT_Glyph_Format::FT_GLYPH_FORMAT_BITMAP)
    {
        err = FT_Render_Glyph(face->glyph, FT_Render_Mode::FT_RENDER_MODE_NORMAL);
        if (err != 0)
        {
            errorMsg = std::string() + "Error rendering glyph to bitmap: " + std::to_string(err);
            return false;
        }
    }

    return true;
}

bool FreeTypeHandler::SetFontSize(unsigned int id, FontSizeData dat)
{
    //Try to find the font face with the given ID.
    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return false;

    //Try to set its size.
    FT_Error err = FT_Set_Char_Size(loc->second, dat.CharWidth, dat.CharHeight, dat.HorizontalDPI, dat.VerticalDPI);
    if (err != 0)
    {
        errorMsg = std::string("Freetype error when sizing font " + std::to_string(id) + ": " + std::to_string(err) + ". Did you try to set the pixel size to an unsupported value?");
        return false;
    }

    return true;
}
bool FreeTypeHandler::SetFontSize(unsigned int id, unsigned int pixelWidth, unsigned int pixelHeight)
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return false;

    FT_Error err = FT_Set_Pixel_Sizes(loc->second, pixelWidth, pixelHeight);
    if (err != 0)
    {
        errorMsg = std::string() + "Unknown FreeType error " + std::to_string(err) + ". Did you try to set the pixel size to an unsupported value?";
        return false;
    }

    return true;
}

unsigned int FreeTypeHandler::GetNumbGlyphs(unsigned int id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return 0;

    return loc->second->num_glyphs;
}
bool FreeTypeHandler::GetCanBeScaled(unsigned int id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return false;

    return (bool)(loc->second->face_flags | FT_FACE_FLAG_SCALABLE);
}
FreeTypeHandler::SupportedSizes FreeTypeHandler::GetSupportedSizes(unsigned int id)
{
    SupportedSizes ret;
    ret.Sizes = 0;
    ret.NumbSizes = 0;

    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return ret;

    ret.Sizes = loc->second->available_sizes;
    ret.NumbSizes = loc->second->num_fixed_sizes;
    return ret;
}

Vector2i FreeTypeHandler::GetGlyphSize(unsigned int id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return Vector2i();

    return Vector2i(loc->second->glyph->bitmap.width, loc->second->glyph->bitmap.rows);
}
Vector2i FreeTypeHandler::GetGlyphOffset(unsigned int id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return Vector2i();

    return Vector2i(loc->second->glyph->bitmap_left, loc->second->glyph->bitmap_top);
}
Vector2i FreeTypeHandler::GetMoveToNextGlyph(unsigned int id) const
{
    FaceMapLoc loc;
    if (!TryFindID(id, loc)) return Vector2i();

    return Vector2i(loc->second->glyph->advance.x >> 6, loc->second->glyph->advance.y >> 6);
}

FreeTypeHandler::CharRenderType FreeTypeHandler::RenderChar(unsigned int fontID, unsigned int charToRender)
{
    FaceMapLoc loc;
    if (!TryFindID(fontID, loc)) return CharRenderType::CRT_ERROR;

    FT_Face fce = loc->second;
    FT_Error err;
    FT_UInt charIndex;

    //Make sure the given character exists.
    charIndex = FT_Get_Char_Index(fce, charToRender);
    if (charIndex == 0)
    {
        errorMsg = std::string() + "Character not found";
        return CharRenderType::CRT_ERROR;
    }

    //Load the glyph.
    err = FT_Load_Glyph(fce, charIndex, FT_LOAD_DEFAULT);
    if (err != 0)
    {
        errorMsg = std::string() + "Error loading glyph: " + std::to_string(err);
        return CharRenderType::CRT_ERROR;
    }

    //If it isn't a bitmap, render it to a bitmap.
    if (fce->glyph->format != FT_Glyph_Format::FT_GLYPH_FORMAT_BITMAP)
    {
        err = FT_Render_Glyph(fce->glyph, FT_Render_Mode::FT_RENDER_MODE_NORMAL);
        if (err != 0)
        {
            errorMsg = std::string() + "Error rendering glyph to bitmap: " + std::to_string(err);
            return CharRenderType::CRT_ERROR;
        }
    }

    //Different pixel values work differently.
    char pixelMode = fce->glyph->bitmap.pixel_mode;
    unsigned int absPitch = (unsigned int)BasicMath::Abs(fce->glyph->bitmap.pitch);
    if (pixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_MONO)
    {
        //Each pixel is 1 bit.

        isGreyscale = true;
        renderedTextColor.Reset(1, 1);

        renderedTextGreyscale.Reset(fce->glyph->bitmap.width, fce->glyph->bitmap.rows);
        renderedTextGreyscale.FillFunc([&fce, absPitch](Vector2i loc, unsigned char * outP)
        {
            unsigned int index = ((unsigned int)loc.x / 8) + ((unsigned int)loc.y * absPitch);
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

        isGreyscale = true;
        renderedTextColor.Reset(1, 1);

        //TODO: Figure out wtf is going on.
        renderedTextGreyscale.Reset(fce->glyph->bitmap.width, fce->glyph->bitmap.rows);
        renderedTextGreyscale.FillFunc([&fce, absPitch](Vector2i loc, unsigned char * outP)
        {
            loc.y = fce->glyph->bitmap.rows - loc.y - 1;
            unsigned int index = (unsigned int)loc.x + ((unsigned int)loc.y * absPitch);
            unsigned char value = fce->glyph->bitmap.buffer[index];

            *outP = value;
        });

        return CharRenderType::CRT_GREYSCALE;
    }
    if (pixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_LCD)
    {
        //Each pixel is 3 components, one byte per component.

        isGreyscale = false;

        renderedTextColor.Reset(fce->glyph->bitmap.width, fce->glyph->bitmap.rows);
        renderedTextColor.FillFunc([&fce, absPitch](Vector2i loc, Vector4b * outP)
        {
            unsigned int index = ((unsigned int)loc.x * 3) + ((unsigned int)loc.y * absPitch);
            *outP = Vector4b(fce->glyph->bitmap.buffer[index], fce->glyph->bitmap.buffer[index + 1], fce->glyph->bitmap.buffer[index + 2], 255);
        });

        return CharRenderType::CRT_COLOR;
    }
    else
    {
        errorMsg = "The pixel mode '" + std::to_string(pixelMode) + "' is not supported.";
        return CharRenderType::CRT_ERROR;
    }
}

void FreeTypeHandler::GetChar(MTexture & outTex) const
{
    if (isGreyscale)
    {
        outTex.SetGreyscaleData(renderedTextGreyscale, PixelSizes::PS_8U_GREYSCALE);
    }
    else
    {
        outTex.SetColorData(renderedTextColor, PixelSizes::PS_8U);
    }
}

bool FreeTypeHandler::TryFindID(unsigned int id, FaceMapLoc & outLoc) const
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
        errorMsg = std::string() + "Error initializing FreeType library: " + std::to_string(err);
}
FreeTypeHandler::~FreeTypeHandler(void)
{
    FT_Error err = FT_Done_FreeType(ftLib);

    if (err == 0) return;

    std::cout << "Error ending freetype library: " << std::to_string(err);
    char dummy;
    std::cin >> dummy;
}