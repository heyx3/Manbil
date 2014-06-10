#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include <freetype-gl.h>
#include <texture-atlas.h>
#include <texture-font.h>
#include <font-manager.h>
#include <text-buffer.h>

#include "../Vertices.h"


//Debugging/error-printing.
#include <iostream>
namespace GUITESTWORLD_NAMESPACE
{
    void Pause(void)
    {
        char dummy;
        std::cin >> dummy;
    }
    bool PrintError(bool whetherToPrint, std::string errorIntro, std::string errorMsg)
    {
        if (whetherToPrint)
        {
            std::cout << errorIntro << ": " << errorMsg << "\n";
        }

        return whetherToPrint;
    }
}
using namespace GUITESTWORLD_NAMESPACE;

/*
#pragma region Freetype-GL code


struct VertexPosTex1Color
{
    Vector3f Pos;
    Vector2f TexCoord;
    Vector4f Color;
    VertexPosTex1Color(Vector3f pos = Vector3f(), Vector2f texCoord = Vector2f(), Vector4f color = Vector4f())
        : Pos(pos), TexCoord(texCoord), Color(color)
    {

    }

    static VertexAttributes GetAttributeData(void) { return VertexAttributes(3, 2, 4, false, false, false); }
};


texture_atlas_t * ftgl_atlas;
texture_font_t * ftgl_font;
vertex_buffer_t * ftgl_buffer;

void LoadFreeTypeGLStuff(const char * filename = "Content/Fonts/Candara.ttf")
{
    ftgl_atlas = texture_atlas_new(512, 512, 1);
    ftgl_font = texture_font_new(ftgl_atlas, "Content/Fonts/Candara.ttf", 16);
    ftgl_buffer = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");

    //Load some glyphs into the atlas to save time later.
    texture_font_load_glyphs(ftgl_font, L" !\"#$%&'()*+,-./0123456789:;<=>?"
                                        L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                        L"`abcdefghijklmnopqrstuvwxyz{|}~");
}
void DeleteFreeTypeGLStuff(void)
{
    vertex_buffer_delete(ftgl_buffer);
    texture_font_delete(ftgl_font);
    texture_atlas_delete(ftgl_atlas);
}

void addText(wchar_t * text, vec4 * color, vec2 * pen)
{
    size_t i;
    float r = color->red,
          g = color->green,
          b = color->blue,
          a = color->alpha;
    for (i = 0; i < wcslen(text); ++i)
    {
        texture_glyph_t * glyph = texture_font_get_glyph(ftgl_font, text[i]);
        if (glyph != 0)
        {
            int kerning = 0;
            if (i > 0)
            {
                kerning = texture_glyph_get_kerning(glyph, text[i - 1]);
            }

            pen->x += kerning;
            int x0 = (int)(pen->x + glyph->offset_x),
                y0 = (int)(pen->y + glyph->offset_y);
            int x1 = (int)(x0 + glyph->width),
                y1 = (int)(y0 + glyph->height);
            float s0 = glyph->s0,
                  t0 = glyph->t0,
                  s1 = glyph->s1,
                  t1 = glyph->t1;
            GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };
            VertexPosTex1Color vertices[4] = { VertexPosTex1Color(Vector3f(x0, y0, 0), Vector2f(s0, t0), Vector4f(r, g, b, a)),
                                               VertexPosTex1Color(Vector3f(x0, y1, 0), Vector2f(s0, t1), Vector4f(r, g, b, a)),
                                               VertexPosTex1Color(Vector3f(x1, y1, 0), Vector2f(s1, t1), Vector4f(r, g, b, a)),
                                               VertexPosTex1Color(Vector3f(x1, y0, 0), Vector2f(s1, t0), Vector4f(r, g, b, a)) };
            vertex_buffer_push_back(ftgl_buffer, vertices, 4, indices, 6);
            pen->x += glyph->advance_x;
        }
    }
}

RenderObjHandle LoadText(wchar_t * text = L"A Quick Brown Fox Jumps Over The Laxy Dog 0123456789", Vector4f color = Vector4f(0.0f, 0.0f, 0.0f, 1.0f))
{
    vertex_buffer_clear(ftgl_buffer);

    //"Draw" the text.
    vec2 pen = { 0, 0 };
    vec4 black = { color.x, color.y, color.z, color.w };
    addText(text, &black, &pen);

    //Return the necessary data.
    return ftgl_atlas->id;
}


#pragma endregion
*/

Vector2i GUITestWorld::WindowSize = Vector2i(800, 800);


bool GUITestWorld::ReactToError(bool isEverythingOK, std::string errorIntro, std::string errorMsg)
{
    if (PrintError(!isEverythingOK, errorIntro, errorMsg))
    {
        EndWorld();
    }

    return isEverythingOK;
}
void GUITestWorld::OnInitializeError(std::string errorMsg)
{
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    Pause();
    EndWorld();
}
void GUITestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    WindowSize.x = newW;
    WindowSize.y = newH;
    glViewport(0, 0, newW, newH);
}

void GUITestWorld::DestroyMyStuff(void)
{
    
}

void GUITestWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    InitializeStaticSystems(false, false, true);

    glViewport(0, 0, WindowSize.x, WindowSize.y);
}

void GUITestWorld::UpdateWorld(float elapsed)
{

}
void GUITestWorld::RenderOpenGL(float elapsed)
{

}