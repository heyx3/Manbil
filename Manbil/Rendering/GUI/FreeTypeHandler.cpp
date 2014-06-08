#include "FreeTypeHandler.h"

#include <iostream>
#include "../Texture Management/ManbilTexture.h"
#include "../Texture Management/TextureConverters.h"


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

    return loc->second->face_flags | FT_FACE_FLAG_SCALABLE;
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
    //return Vector2i(loc->second->glyph->metrics.width, loc->second->glyph->metrics.height);
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

bool FreeTypeHandler::RenderChar(unsigned int fontID, unsigned int charToRender)
{
    FaceMapLoc loc;
    if (!TryFindID(fontID, loc)) return false;

    FT_Face fce = loc->second;
    FT_Error err;
    FT_UInt charIndex;

    //Make sure the given character exists.
    charIndex = FT_Get_Char_Index(fce, charToRender);
    if (charIndex == 0)
    {
        errorMsg = std::string() + "Character not found";
        return false;
    }

    //Load the glyph.
    err = FT_Load_Glyph(fce, charIndex, FT_LOAD_DEFAULT);
    if (err != 0)
    {
        errorMsg = std::string() + "Error loading glyph: " + std::to_string(err);
        return false;
    }

    //If it isn't a bitmap, render it to a bitmap.
    if (fce->glyph->format != FT_Glyph_Format::FT_GLYPH_FORMAT_BITMAP)
    {
        err = FT_Render_Glyph(fce->glyph, FT_Render_Mode::FT_RENDER_MODE_NORMAL);
        if (err != 0)
        {
            errorMsg = std::string() + "Error rendering glyph to bitmap: " + std::to_string(err);
            return false;
        }
    }

    //Get the pixel format of the bitmap.
    Vector4b(*colorGetter)(FT_Bitmap & bmp, unsigned int absPitch, unsigned int x, unsigned int y);
    switch (fce->glyph->bitmap.pixel_mode)
    {
        case FT_Pixel_Mode::FT_PIXEL_MODE_MONO:
            //Each pixel is 1 bit.
            colorGetter = [](FT_Bitmap & bmp, unsigned int absPitch, unsigned int x, unsigned int y)
            {
                unsigned int index = (x / 8) + (y * absPitch);
                int bit = x % 8;

                unsigned char value = bmp.buffer[index] | (128 >> bit);
                value = value << bit;
                value *= 255;

                return Vector4b(value, value, value, value);
            };
            break;

        case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY:
            //Each pixel is one byte.
            colorGetter = [](FT_Bitmap & bmp, unsigned int absPitch, unsigned int x, unsigned int y)
            {
                y = bmp.rows - y - 1;
                unsigned int index = x + (y * absPitch);
                unsigned char value = bmp.buffer[index];
                return Vector4b(value, value, value, value);
            };
            break;

        case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY2:
        case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY4:
            errorMsg = "Pixel modes 'gray2' and 'gray4' are not supported because I didn't feel like implementing them and they are almost never used.";
            return false;

        case FT_Pixel_Mode::FT_PIXEL_MODE_LCD:
            colorGetter = [](FT_Bitmap & bmp, unsigned int absPitch, unsigned int x, unsigned int y)
            {
                unsigned int index = (x * 3) + (y * absPitch);

                return Vector4b(bmp.buffer[index], bmp.buffer[index + 1], bmp.buffer[index + 2], 255);
            };
            break;

        case FT_Pixel_Mode::FT_PIXEL_MODE_LCD_V:
            errorMsg = "Pixel mode 'LCD_V' is not supported because I didn't fell like implementing it and I don't think it's that useful.";
            return false;

        default:
            errorMsg = std::string() + "Error getting pixel format of the bitmap: unknown format " + std::to_string(fce->glyph->bitmap.pixel_mode);
            return false;
    }


    renderedText.Reset(RoundUpToPowerOfTwo((unsigned int)fce->glyph->bitmap.width), RoundUpToPowerOfTwo((unsigned int)fce->glyph->bitmap.rows), Vector4b());
    renderedText.Reset(fce->glyph->bitmap.width, fce->glyph->bitmap.rows);
    unsigned int absPitch = (unsigned int)BasicMath::Abs(fce->glyph->bitmap.pitch);
    for (unsigned int y = 0; y < fce->glyph->bitmap.rows; ++y)
        for (unsigned int x = 0; x < fce->glyph->bitmap.width; ++x)
            renderedText[Vector2i((int)x, (int)y)] = colorGetter(fce->glyph->bitmap, absPitch, x, y);

    return true;
}

bool FreeTypeHandler::GetChar(ManbilTexture & outTex) const
{
    const Array2D<Vector4b> & charArray = GetChar();
    sf::Image img;
    TextureConverters::ToImage(charArray, img);

    if (outTex.UsesGLTex())
    {
        RenderDataHandler::SetTexture2DData(outTex.GLTex, img);
        return true;
    }
    else if (outTex.UsesSFMLTex())
    {
        outTex.SFMLTex->loadFromImage(img);
        return true;
    }
    else
    {
        errorMsg = "The given texture is not an OpenGL texture OR an SFML texture!";
        return false;
    }
}

unsigned int FreeTypeHandler::RoundUpToPowerOfTwo(unsigned int x)
{
    //I doubt rendered bitmaps will be smaller than 16x16.
    unsigned int value = 16;
    while (value < x)
        value *= 2;
    return value;
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
    : nextID(1), renderedText(0, 0, Vector4b())
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