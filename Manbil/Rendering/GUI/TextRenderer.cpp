#include "TextRenderer.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"
#include "../Materials/Data Nodes/Miscellaneous/DataNodeGenerators.h"
#include "../../ScreenClearer.h"
#include "../Texture Management/TextureConverters.h"


Material * TextRenderer::textRenderer = 0;
DrawingQuad * TextRenderer::textRendererQuad = 0;
UniformDictionary TextRenderer::textRendererParams = UniformDictionary();
RenderInfo TextRenderer::textRendererInfo = RenderInfo(0, 0, 0, 0, 0, 0);
Camera TextRenderer::textRendererCam = Camera();
TransformObject TextRenderer::textRendererTransform = TransformObject();
Matrix4f TextRenderer::viewMat = Matrix4f(),
         TextRenderer::projMat = Matrix4f(),
         TextRenderer::worldMat = Matrix4f();

const char * textSamplerName = "u_charSampler";


std::string TextRenderer::InitializeSystem(SFMLOpenGLWorld * world)
{
    if (textRenderer != 0) return "System was already initialized.";

    textRendererQuad = new DrawingQuad();


    //Transform matrices and render info.

    worldMat.SetAsIdentity();
    textRendererCam.GetViewTransform(viewMat);
    textRendererCam.GetOrthoProjection(projMat);

    textRendererInfo = RenderInfo(world, &textRendererCam, &textRendererTransform, &worldMat, &viewMat, &projMat);


    //Material.
    textRendererParams.ClearUniforms();
    VertexAttributes quadAtts = DrawingQuad::GetAttributeData(),
                     fragmentInputs(2, false);
    DataNodePtr vertexIns(new VertexInputNode(quadAtts));
    DataNodePtr fragIns(new FragmentInputNode(fragmentInputs));

    DataLine textSampler(DataNodePtr(new TextureSampleNode(DataLine(fragIns, 0), textSamplerName)),
                         TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllChannels));

    std::unordered_map<RenderingChannels, DataLine> channels;
    DataLine worldPos(DataNodePtr(new ObjectPosToWorldPosCalcNode(DataLine(vertexIns, 0))), 0);
    channels[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new CombineVectorNode(worldPos, DataLine(1.0f))), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(vertexIns, 1);
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new SwizzleNode(textSampler, SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X)), 0);
    channels[RenderingChannels::RC_Opacity] = DataLine(DataNodePtr(new VectorComponentsNode(textSampler)), 0);

    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(channels, textRendererParams, quadAtts, RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!genM.ErrorMessage.empty())
    {
        delete textRendererQuad;
        return std::string() + "Error generating text renderer material: " + genM.ErrorMessage;
    }

    textRenderer = genM.Mat;

    return "";
}
void TextRenderer::DestroySystem(void)
{
    if (textRenderer == 0) return;

    delete textRendererQuad;
    delete textRenderer;
    textRendererParams.ClearUniforms();
}


unsigned int TextRenderer::CreateTextRenderSlot(std::string fontPath, TextureSettings settings, unsigned int renderSpaceWidth, unsigned int renderSpaceHeight, unsigned int pWidth, unsigned int pHeight)
{
    //Create texture and set its settings.
    unsigned int texID = TexManager.CreateSFMLTexture();
    sf::Texture * tex = TexManager[texID].SFMLTex;
    sf::Texture::bind(tex);
    settings.SetData();

    //Create render target.
    RendTargetColorTexSettings colorSettings;
    colorSettings.Settings = ColorTextureSettings(renderSpaceWidth, renderSpaceHeight, ColorTextureSettings::Sizes::CTS_8_GREYSCALE, settings);
    RendTargetDepthTexSettings depthSettings;
    depthSettings.UsesDepthTexture = false;
    depthSettings.Settings = DepthTextureSettings(renderSpaceWidth, renderSpaceHeight, DepthTextureSettings::Sizes::DTS_16, settings);
    unsigned int rendTargetID = RTManager.CreateRenderTarget(colorSettings, depthSettings);
    if (rendTargetID == RenderTargetManager::ERROR_ID)
    {
        TexManager.DeleteTexture(texID);
        errorMsg = "Error creating render target: " + RTManager.GetError();
        return FreeTypeHandler::ERROR_ID;
    }

    //Try to create the font.
    unsigned int fontID = GetHandler().LoadFont(fontPath, FontSizeData(1, 0, 0, 0), 0);
    if (fontID == FreeTypeHandler::ERROR_ID)
    {
        TexManager.DeleteTexture(texID);
        RTManager.DeleteRenderTarget(rendTargetID);
        errorMsg = "Error loading font from " + fontPath + ": " + GetHandler().GetError();
        return FreeTypeHandler::ERROR_ID;
    }

    //Try to set the font's size.
    if (!GetHandler().SetFontSize(fontID, pWidth, pHeight))
    {
        TexManager.DeleteTexture(texID);
        RTManager.DeleteRenderTarget(rendTargetID);
        errorMsg = "Error setting font size to " + std::to_string(pWidth) + "x" + std::to_string(pHeight) + ": " + GetHandler().GetError();
        return FreeTypeHandler::ERROR_ID;
    }

    //Add the ID's to the collection.
    Slot slot;
    slot.TexID = texID;
    slot.RenderTargetID = rendTargetID;
    slot.String = "";
    slot.Width = 0;
    slot.Height = 0;
    slots[fontID] = slot;
    return fontID;
}

bool TextRenderer::RenderString(unsigned int slot, std::string textToRender, unsigned int backBufferWidth, unsigned int backBufferHeight)
{
    //Try to find the slot.
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return false;
    Slot & slotC = slots[slot];

    //Get texture/render target.
    sf::Texture * tex = TexManager[slotC.TexID].SFMLTex;
    RenderTarget * rendTarg = RTManager[slotC.RenderTargetID];
    if (tex == 0) { errorMsg = "Associated texture did not exist!"; return false; }
    if (rendTarg == 0) { errorMsg = "Associated render target did not exist!"; return false; }
    ManbilTexture texM(tex);
    textRendererParams.TextureUniforms[textSamplerName].Texture = texM;




    /*

    //Load the first character.
    //Create an array with a good estimate for the size of the final texture.
    Array2D<Vector4b> outTexArray(0, 0);
    Vector2i penTopLeft;
    Vector2i charSize, charOffset, charMoveAfterDraw;
    for (unsigned int c = 0; c < textToRender.size(); ++c)
    {
        char ch = textToRender.c_str()[c];

        //Load the character.
        //if (!FreeTypeHandler::Instance.LoadGlyph(slot, textToRender.c_str()[0]))
        //{
        //    errorMsg = std::string() + "Error loading glyph for '" + textToRender.c_str()[0] + "': " + FreeTypeHandler::Instance.GetError();
        //    return false;
        //}
        if (!FreeTypeHandler::Instance.RenderChar(slot, ch))
        {
            errorMsg = std::string() + "Error loading glyph for '" + ch + "': " + FreeTypeHandler::Instance.GetError();
            return false;
        }

        //Get data for the given character.
        charSize = FreeTypeHandler::Instance.GetGlyphSize(slot);
        charOffset = FreeTypeHandler::Instance.GetGlyphOffset(slot);
        charMoveAfterDraw = FreeTypeHandler::Instance.GetMoveToNextGlyph(slot);
        const Array2D<Vector4b> & charArray = FreeTypeHandler::Instance.GetChar();


        //If this is the first character, use its size as an approximation for the final render array.
        if (c == 0)
        {
            outTexArray.Reset((unsigned int)(charSize.x * (1 + textToRender.size())),
                              (unsigned int)(charSize.y * 2),
                              Vector4b());
        }
        //Otherwise, if the new character is too big for the current final render array, expand it.
        else
        {
            bool tooBigX = (penTopLeft.x + charArray.GetWidth() > outTexArray.GetWidth()),
                 tooBigY = (penTopLeft.y + charArray.GetHeight() > outTexArray.GetHeight());
            unsigned int newWidth = outTexArray.GetWidth() + (charSize.x * 2 * (textToRender.size() - c)),
                         newHeight = outTexArray.GetHeight() * 2;

            if (tooBigX && tooBigY)
                outTexArray.Resize(newWidth, newHeight, Vector4b());
            else if (tooBigX)
                outTexArray.Resize(newWidth, outTexArray.GetHeight(), Vector4b());
            else if (tooBigY)
                outTexArray.Resize(outTexArray.GetWidth(), newHeight, Vector4b());
        }


        //"Render" the character.
        for (Vector2i charArrayLoc; charArrayLoc.y < charArray.GetHeight(); ++charArrayLoc.y)
        {
            for (charArrayLoc.x = 0; charArrayLoc.x < charArray.GetWidth(); ++charArrayLoc.x)
            {
                outTexArray[charArrayLoc + penTopLeft] += charArray[charArrayLoc];
            }
        }

        //Move the pen forward.
        penTopLeft += charMoveAfterDraw;
    }

    slotC.Width = outTexArray.GetWidth();
    slotC.Height = outTexArray.GetHeight();

    //Put the values into a texture.
    sf::Image img;
    TextureConverters::ToImage(outTexArray, img);
    tex->loadFromImage(img);


    return true;
    */






    //Set up rendering.
    rendTarg->EnableDrawingInto();
    glViewport(0, 0, rendTarg->GetColorSettings()[0].Settings.Width, rendTarg->GetColorSettings()[0].Settings.Height);
    RenderingState(RenderingState::C_BACK,
                   RenderingState::BlendingExpressions::BE_SOURCE_COLOR, RenderingState::BlendingExpressions::BE_ONE_MINUS_SOURCE_COLOR,
                   false, false).EnableState();
    ScreenClearer(true, true, false, Vector4f(0.1f, 0.0f, 0.0f, 0.0f)).ClearScreen();


    //Render each character into the render target.
    Vector2f pos = Vector2f(-1.0f, 1.0f);
    Vector2i size = Vector2i(), drawOffset = Vector2i(), movement = Vector2i();
    Vector2f scaledSize = Vector2f(), scaledOffset = Vector2f(), scaledMovement = Vector2f();
    Vector2f invRendTargSize(2.0f / (float)rendTarg->GetColorSettings()[0].Settings.Width,
                             -2.0f / (float)rendTarg->GetColorSettings()[0].Settings.Height);
    for (unsigned int i = 0; i < textToRender.size(); ++i)
    {
        char ch = textToRender.c_str()[i];

        //Render the character into an array.
        if (!FreeTypeHandler::Instance.RenderChar(slot, ch))
        {
            errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + FreeTypeHandler::Instance.GetError();
            rendTarg->DisableDrawingInto(backBufferWidth, backBufferHeight);
            return false;
        }

        //If the character is empty (i.e. a space), don't bother rendering it.
        if (FreeTypeHandler::Instance.GetChar().GetWidth() > 0 && FreeTypeHandler::Instance.GetChar().GetHeight() > 0)
        {
            //Render the array into a texture.
            if (!FreeTypeHandler::Instance.GetChar(texM))
            {
                errorMsg = std::string() + "Error copying character #" + std::to_string(i) + ", '" + ch + "', into an SFML texture: " + FreeTypeHandler::Instance.GetError();
                rendTarg->DisableDrawingInto(backBufferWidth, backBufferHeight);
                return false;
            }

            //Compute layout data.
            size = FreeTypeHandler::Instance.GetGlyphSize(slot);
            scaledSize = ToV2f(size).ComponentProduct(invRendTargSize);
            drawOffset = FreeTypeHandler::Instance.GetGlyphOffset(slot);
            scaledOffset = ToV2f(drawOffset).ComponentProduct(invRendTargSize);
            movement = FreeTypeHandler::Instance.GetMoveToNextGlyph(slot);
            scaledMovement = ToV2f(movement).ComponentProduct(invRendTargSize);

            //Set up the render quad size/location.
            //textRendererQuad->SetSize(scaledSize);
            //textRendererQuad->SetOrigin(textRendererQuad->GetOrigin() - scaledSize);
            //textRendererQuad->IncrementPos(-scaledOffset);
            //textRendererQuad->SetBounds(pos, pos + ToV2f(size));
            textRendererQuad->SetBounds(pos, pos + scaledSize);
            textRendererQuad->MakeSizePositive();

            //Render.
            if (!textRendererQuad->Render(RenderPasses::BaseComponents, textRendererInfo, textRendererParams, *textRenderer))
            {
                errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + textRenderer->GetErrorMsg();
                rendTarg->DisableDrawingInto(backBufferWidth, backBufferHeight);
                return false;
            }

            //Reset the render quad location.
            //textRendererQuad->IncrementPos(scaledOffset);
            //textRendererQuad->SetOrigin(textRendererQuad->GetOrigin() + scaledSize);
            pos += drawOffset;
        }

        //Move the quad to the next position for the letter.
        pos += scaledMovement;//ToV2f(size) + Vector2f((float)movement.x, 0.0f);
    }

    rendTarg->DisableDrawingInto(backBufferWidth, backBufferHeight);
    return true;
}

Vector2i TextRenderer::GetRenderedStringSize(unsigned int slot) const
{
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return Vector2i();

    return Vector2i(loc->second.Width, loc->second.Height);
}

bool TextRenderer::RenderString(unsigned int slot, std::string textToRender, RenderInfo & info, UniformSamplerValue & texIn,
                                const std::vector<const Mesh*> & meshes, Material * toRender, UniformDictionary & params)
{
    //Try to find the slot.
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return false;
    const Slot & slotC = loc->second;

    //Get texture/render target.
    sf::Texture * tex = TexManager[slotC.TexID].SFMLTex;
    ManbilTexture texMB(tex);
    RenderTarget * rendTarg = RTManager[slotC.RenderTargetID];
    if (tex == 0) { errorMsg = "Associated texture did not exist!"; return false; }
    if (rendTarg == 0) { errorMsg = "Associated render target did not exist!"; return false; }

    //Calculate the "side" vectors for each mesh.
    std::vector<Vector3f> sideways;
    sideways.reserve(meshes.size());
    for (unsigned int i = 0; i < meshes.size(); ++i)
        sideways.insert(sideways.end(), meshes[i]->Transform.GetRightward());

    //Render each character into the render target.
    Vector3f deltaPos;
    for (unsigned int i = 0; i < textToRender.size(); ++i)
    {
        char ch = textToRender.c_str()[i];

        //Render the character into an array.
        if (!FreeTypeHandler::Instance.RenderChar(slot, ch))
        {
            errorMsg = std::string() + "Error loading character #" + std::to_string(i) + ", '" + ch + "': " + FreeTypeHandler::Instance.GetError();
            return false;
        }

        //Render the array into a texture.
        if (!FreeTypeHandler::Instance.GetChar(texMB))
        {
            errorMsg = std::string() + "Error copying character #" + std::to_string(i) + ", '" + ch + "', into an SFML texture: " + FreeTypeHandler::Instance.GetError();
            return false;
        }
        texIn.Texture = texMB;

        //Render the texture.
        if (!toRender->Render(RenderPasses::BaseComponents, info, meshes, params))
        {
            errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + toRender->GetErrorMsg();
            return false;
        }

        //Move the meshes sideways to the next letter.
        for (unsigned int i = 0; i < meshes.size(); ++i)
            ;//meshes[i]->Transform.IncrementPosition()
        //TODO: Finish.
        errorMsg = std::string() + "This feature not finished yet.";
        return false;
    }

    return true;
}


bool TextRenderer::TryFindSlot(unsigned int slot, TextRenderer::SlotMapLoc & outLoc) const
{
    outLoc = slots.find(slot);
    if (outLoc == slots.end())
    {
        errorMsg = "Couldn't find slot " + std::to_string(slot);
        return false;
    }
    else return true;
}

ManbilTexture TextRenderer::GetRenderedString(unsigned int slot) const
{
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return ManbilTexture();

    return RTManager[loc->second.RenderTargetID]->GetColorTextures()[0];
}
const char * TextRenderer::GetString(unsigned int slot) const
{
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return 0;

    return loc->second.String;
}