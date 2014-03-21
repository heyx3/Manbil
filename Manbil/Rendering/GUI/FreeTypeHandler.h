#pragma once

#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H



//Wraps usage of the FreeType library.
class FreeTypeHandler
{
public:

    static const unsigned int ERROR_ID = 0;


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }


    FreeTypeHandler(void)
        : nextID(1)
    {
        FT_Error err = FT_Init_FreeType(&ftLib);
        if (err != 0)
            errorMsg = std::string() + "Error initializing FreeType library: " + std::to_string(err);
    }


    //Returns the ID of the loaded font. If font creation failed, returns ERROR_ID.
    //A value of 0 for "characterWidth" means it is the same size as "characterHeight",
    //   and vice-versa.
    //Same goes for horizontal-/verticalResolution; additionally, if horizontal- AND verticalResolution
    //   are 0, they default to 72.
    unsigned int LoadFont(std::string path, signed long characterWidth, signed long characterHeight,
                          unsigned int horizontalDPI = 0, unsigned int verticalDPI = 0,
                          signed long faceIndex = 0)
    {
        FT_Face face;
        FT_Error err = FT_New_Face(ftLib, path.c_str(), faceIndex, &face);


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

        err = FT_Set_Char_Size(face, characterWidth, characterHeight, horizontalDPI, verticalDPI);
        if (err != 0)
        {
            errorMsg = std::string() + "Unknown error when sizing text: " + std::to_string(err);
            return ERROR_ID;
        }


        faces[nextID] = face;
        nextID += 1;
        return nextID - 1;
    }

    //Gets the texture for the given font and character.
    bool GetGlyph(unsigned int id, unsigned int charCode)
    {
        FT_Error err;

        //Make sure the given font exists.
        std::unordered_map<unsigned int, FT_Face>::const_iterator location = faces.find(id);
        if (location == faces.end())
        {
            errorMsg = std::string() + "Font id '" + std::to_string(id) + "' doesn't exist";
            return false;
        }

        FT_Face face = faces[id];

        //Make sure the given character exists.
        FT_UInt index = FT_Get_Char_Index(face, charCode);
        if (index == 0)
        {
            errorMsg = std::string() + "Character not found";
            return false;
        }

        //Load the glyph.
        err = FT_Load_Glyph(face, index, 0);
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
    }



private:

    unsigned int nextID;

    FT_Library ftLib;
    std::unordered_map<unsigned int, FT_Face> faces;

    std::string errorMsg;
};