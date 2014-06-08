#include "TextRenderer.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"
#include "../Materials/Data Nodes/Miscellaneous/DataNodeGenerators.h"


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

    textRendererCam.Info.Width = 1024;
    textRendererCam.Info.Height = 1024;
    textRendererCam.MinOrthoBounds = Vector3f(-512.0f, -512.0f, -1.0f);
    textRendererCam.MaxOrthoBounds = Vector3f(512.0f, 512.0f, 1.0f);

    worldMat.SetAsIdentity();
    textRendererCam.GetViewTransform(viewMat);
    textRendererCam.GetOrthoProjection(projMat);

    textRendererInfo.World = world;
    textRendererInfo.Cam = &textRendererCam;
    textRendererInfo.Trans = &textRendererTransform;
    textRendererInfo.mWorld = &worldMat;
    textRendererInfo.mView = &viewMat;
    textRendererInfo.mProj = &projMat;


    //Material.
    textRendererParams.ClearUniforms();
    VertexAttributes quadAtts = DrawingQuad::GetAttributeData(),
                     fragmentInputs(2, false);
    DataNodePtr vertexIns(new VertexInputNode(quadAtts));
    DataNodePtr fragIns(new FragmentInputNode(fragmentInputs));

    DataLine textSampler(DataNodePtr(new TextureSampleNode(DataLine(fragIns, 0), textSamplerName)),
                         TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));

    std::unordered_map<RenderingChannels, DataLine> channels;
    channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(vertexIns, 1);
    channels[RenderingChannels::RC_VertexPosOutput] = DataNodeGenerators::ObjectPosToScreenPos<DrawingQuad>(0);
    //channels[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new CombineVectorNode(DataLine(vertexIns, 0), DataLine(1.0f))), 0);
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new SwizzleNode(textSampler, SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X)), 0);

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


unsigned int TextRenderer::CreateTextRenderSlot(std::string fontPath, TextureSettings settings, unsigned int pWidth, unsigned int pHeight)
{
    //Create texture and set its settings.
    unsigned int texID = TexManager.CreateSFMLTexture();
    sf::Texture * tex = TexManager[texID].SFMLTex;
    sf::Texture::bind(tex);
    settings.SetData();

    //Create render target.
    RendTargetColorTexSettings colorSettings;
    colorSettings.Settings = ColorTextureSettings(2048, 512, ColorTextureSettings::Sizes::CTS_8_GREYSCALE, settings);
    RendTargetDepthTexSettings depthSettings;
    depthSettings.UsesDepthTexture = false;
    depthSettings.Settings = DepthTextureSettings(1, 1, DepthTextureSettings::Sizes::DTS_16, settings);
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
}

bool TextRenderer::RenderString(unsigned int slot, std::string textToRender)
{
    //Try to find the slot.
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return false;
    const Slot & slotC = loc->second;

    //Set up rendering data.
    RenderingState renderState(false, false, RenderingState::C_BACK);
    renderState.EnableState();

    //Get texture/render target.
    sf::Texture * tex = TexManager[slotC.TexID].SFMLTex;
    RenderTarget * rendTarg = RTManager[slotC.RenderTargetID];
    if (tex == 0) { errorMsg = "Associated texture did not exist!"; return false; }
    if (rendTarg == 0) { errorMsg = "Associated render target did not exist!"; return false; }

    //Render each character into the render target.
    textRendererQuad->SetOrigin(Vector2f(-1.0f, -1.0f));
    textRendererQuad->SetPos(Vector2f());
    for (unsigned int i = 0; i < textToRender.size(); ++i)
    {
        char ch = textToRender.c_str()[i];

        //Render the character into an array.
        if (!FreeTypeHandler::Instance.RenderChar(slot, ch))
        {
            errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + FreeTypeHandler::Instance.GetError();
            return false;
        }

        //Render the array into a texture.
        if (!FreeTypeHandler::Instance.GetChar(ManbilTexture(tex)))
        {
            errorMsg = std::string() + "Error copying character #" + std::to_string(i) + ", '" + ch + "', into an SFML texture: " + FreeTypeHandler::Instance.GetError();
            return false;
        }
        Vector2i size = FreeTypeHandler::Instance.GetGlyphSize(slot);
        textRendererQuad->SetSize(Vector2f((float)size.x, (float)size.y));

        //Render the character.
        if (!textRendererQuad->Render(RenderPasses::BaseComponents, textRendererInfo, textRendererParams, *textRenderer))
        {
            errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + textRenderer->GetErrorMsg();
            return false;
        }

        //Move the quad to the next position for the letter.
        Vector2i movement = FreeTypeHandler::Instance.GetMoveToNextGlyph(slot);
        textRendererQuad->IncrementPos(Vector2f((float)movement.x, (float)movement.y));
    }

    return true;
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

RenderObjHandle TextRenderer::GetRenderedString(unsigned int slot) const
{
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return 0;

    return RTManager[loc->second.RenderTargetID]->GetColorTextures()[0];
}
const char * TextRenderer::GetString(unsigned int slot) const
{
    SlotMapLoc loc;
    if (!TryFindSlot(slot, loc)) return 0;

    return loc->second.String;
}