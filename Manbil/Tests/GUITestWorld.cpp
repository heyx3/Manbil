#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include "../Vertices.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"

#include "../ScreenClearer.h"
#include "../RenderingState.h"


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


Vector2i GUITestWorld::WindowSize = Vector2i(1000, 1000);
std::string textSamplerName = "u_textSampler";


#pragma region Freetype-GL code

/*
#include <freetype-gl.h>
#include <texture-font.h>
#include <vertex-buffer.h>
#include <markup.h>



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

*/

#pragma endregion



#pragma region My TextRenderer code

#include "../Rendering/GUI/TextRenderer.h"


unsigned int textRendererID = FreeTypeHandler::ERROR_ID;


//Returns an error message, or an empty string if everything went fine.
std::string LoadFont(TextRenderer * rendr, std::string fontPath, unsigned int size)
{
    if (textRendererID != FreeTypeHandler::ERROR_ID)
        return "'textRendererID' was already set to " + std::to_string(textRendererID);

    textRendererID = rendr->CreateTextRenderSlot(fontPath, TextureSettings(TextureSettings::TF_LINEAR, TextureSettings::TW_CLAMP, false),
                                                 2048, 64, 50);

    if (textRendererID == FreeTypeHandler::ERROR_ID)
        return "Error creating font slot for '" + fontPath + "': " + rendr->GetError();

    return "";
}
//Returns an error message, or an empty string if everything went fine.
std::string RenderText(TextRenderer * rendr, std::string text)
{
    if (textRendererID == FreeTypeHandler::ERROR_ID)
        return "'textRendererID' wasn't set to anything";

    if (!rendr->RenderString(textRendererID, text, GUITestWorld::WindowSize.x, GUITestWorld::WindowSize.y))
        return "Error rendering string '" + text + "': " + rendr->GetError();

    return "";
}


#pragma endregion




bool GUITestWorld::ReactToError(bool isEverythingOK, std::string errorIntro, std::string errorMsg)
{
    if (PrintError(!isEverythingOK, errorIntro, errorMsg))
    {
        Pause();
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


void GUITestWorld::InitializeWorld(void)
{
    std::string err;


    SFMLOpenGLWorld::InitializeWorld();
    glViewport(0, 0, WindowSize.x, WindowSize.y);

    //Initialize static stuff.
    err = InitializeStaticSystems(false, true, true);
    if (!ReactToError(err.empty(), "Error initializing static systems", err))
        return;


    //Create the drawing quad.
    quad = new DrawingQuad();
    quad->SetSize(Vector2f(1.0f, .2f));


    //Create the quad rendering material.
    std::unordered_map<RenderingChannels, DataLine> channels;
    DataNodePtr vertexIns(new VertexInputNode(DrawingQuad::GetAttributeData()));
    DataLine worldPos(DataNodePtr(new ObjectPosToWorldPosCalcNode(DataLine(vertexIns, 0))), 0);
    channels[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new CombineVectorNode(worldPos, DataLine(1.0f))), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(vertexIns, 1);
    //The text texture only stores the red component (to save space).
    DataLine redText(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new FragmentInputNode(VertexAttributes(2, false))), 0),
                                                       textSamplerName)),
                     TextureSampleNode::GetOutputIndex(ChannelsOut::CO_Red));
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new CombineVectorNode(redText, redText, redText)), 0);
    channels[RenderingChannels::RC_Opacity] = redText;
    ShaderGenerator::GeneratedMaterial genMat = ShaderGenerator::GenerateMaterial(channels, quadParams, DrawingQuad::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating quad material", genMat.ErrorMessage))
        return;
    quadMat = genMat.Mat;


    //Load the font.
    err = LoadFont(TextRender, "Content/Fonts/Candara.ttf", 25);
    if (!ReactToError(err.empty(), "Error loading 'Content/Fonts/Candara.ttf'", err))
        return;

    //Render a string.
    err = RenderText(TextRender, "Hello World");
    if (!ReactToError(err.empty(), "Error rendering 'A'", err))
        return;
    quadParams.TextureUniforms[textSamplerName].Texture = TextRender->GetRenderedString(textRendererID);
}
void GUITestWorld::DestroyMyStuff(bool destroyStatics)
{
    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(quadMat);

    if (destroyStatics) DestroyStaticSystems(false, true, true);
}


void GUITestWorld::UpdateWorld(float elapsed)
{

}
void GUITestWorld::RenderOpenGL(float elapsed)
{
    //Prepare the back-buffer to be rendered into.
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    ScreenClearer().ClearScreen();
    RenderingState(false, false, RenderingState::C_NONE).EnableState();

    //Set up the "render info" struct.
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(-1.0f, -1.0f, -1.0f);
    cam.MaxOrthoBounds = Vector3f(1.0f, 1.0f, 1.0f);
    cam.Info.Width = WindowSize.x;
    cam.Info.Height = WindowSize.y;
    Matrix4f worldM, viewM, projM;
    quad->GetMesh().Transform.GetWorldTransform(worldM);
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);
    RenderInfo info(this, &cam, &quad->GetMesh().Transform, &worldM, &viewM, &projM);

    //Render the quad.
    if (!ReactToError(quad->Render(RenderPasses::BaseComponents, info, quadParams, *quadMat), "Error rendering quad", quadMat->GetErrorMsg()))
        return;
}