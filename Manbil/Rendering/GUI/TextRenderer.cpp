#include "TextRenderer.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"


Material * TextRenderer::textRenderer = 0;
DrawingQuad * TextRenderer::textRendererQuad = 0;
UniformDictionary TextRenderer::textRendererParams = UniformDictionary();

const char * textSamplerName = "u_charSampler";


std::string TextRenderer::InitializeSystem(void)
{
    if (textRenderer != 0) return "System was already initialized.";

    textRendererQuad = new DrawingQuad();


    VertexAttributes quadAtts = DrawingQuad::GetAttributeData(),
                     fragmentInputs(2, false);
    DataNodePtr vertexIns(new VertexInputNode(quadAtts));
    DataNodePtr fragIns(new FragmentInputNode(fragmentInputs));

    DataLine textSampler(DataNodePtr(new TextureSampleNode(DataLine(fragIns, 0), textSamplerName)),
                         TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));

    std::unordered_map<RenderingChannels, DataLine> channels;
    channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(vertexIns, 1);
    channels[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new CombineVectorNode(DataLine(vertexIns, 0), DataLine(1.0f))), 0);
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
    colorSettings.Settings = ColorTextureSettings(1, 1, ColorTextureSettings::Sizes::CTS_8_GREYSCALE, settings);
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

bool TextRenderer::RenderString(unsigned int slot, const char * textToRender)
{
    //TODO: Finish.
    return false;
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