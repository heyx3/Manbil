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
    tempTex.Create(ColorTextureSettings(1, 1, ColorTextureSettings::Sizes::CTS_8_GREYSCALE, false,
                                        TextureSettings(TextureSettings::FilteringTypes::FT_NEAREST, TextureSettings::WrappingTypes::WT_CLAMP)));


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
    tempTex.DeleteIfValid();
}


TextRenderer::~TextRenderer(void)
{
    //Delete every slot's render target.
    for (auto font = fonts.begin(); font != fonts.end(); ++font)
        for (auto slot = font->second.begin(); slot != font->second.end(); ++slot)
            RTManager.DeleteRenderTarget(slot->RenderTargetID);
    //TODO: Double-check that there is no way to delete fonts.
}


unsigned int TextRenderer::CreateAFont(std::string fontPath, unsigned int pixelWidth, unsigned int pixelHeight)
{
    //Try to create the font.
    unsigned int fontID = GetHandler().LoadFont(fontPath, FontSizeData(1, 0, 0, 0), 0);
    if (fontID == FreeTypeHandler::ERROR_ID)
    {
        errorMsg = "Error creating font from '" + fontPath + "': " + GetHandler().GetError();
        return FreeTypeHandler::ERROR_ID;
    }

    //Try to set the font's size.
    if (!GetHandler().SetFontSize(fontID, pixelWidth, pixelHeight))
    {
        errorMsg = "Error resizing font '" + fontPath + "': " + GetHandler().GetError();
        return FreeTypeHandler::ERROR_ID;
    }

    //Create the entry in the "slots" map.
    fonts[fontID] = std::vector<Slot>();
    return fontID;
}
bool TextRenderer::CreateTextRenderSlots(unsigned int fontID, unsigned int renderSpaceWidth, unsigned int renderSpaceHeight, bool useMipmaps, TextureSettings & settings, unsigned int numbSlots)
{
    SlotCollectionLoc loc;
    if (!TryFindSlotCollection(fontID, loc)) return false;
    std::vector<Slot> & slotCollection = fonts[fontID];

    //Set up the settings used for each render target.
    RendTargetColorTexSettings colorSettings;
    colorSettings.Settings = ColorTextureSettings(renderSpaceWidth, renderSpaceHeight, ColorTextureSettings::Sizes::CTS_8_GREYSCALE, useMipmaps, settings);
    RendTargetDepthTexSettings depthSettings;
    depthSettings.UsesDepthTexture = false;
    depthSettings.Settings = DepthTextureSettings(renderSpaceWidth, renderSpaceHeight, DepthTextureSettings::Sizes::DTS_16, useMipmaps, settings);

    //Create the given number of slots.
    for (unsigned int i = 0; i < numbSlots; ++i)
    {
        //Create render target.
        unsigned int rendTargetID = RTManager.CreateRenderTarget(colorSettings, depthSettings);
        if (rendTargetID == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating render target: " + RTManager.GetError();
            return false;
        }

        //Add the slot to the collection.
        Slot slot;
        slot.RenderTargetID = rendTargetID;
        slot.String = "";
        slot.Width = renderSpaceWidth;
        slot.Height = renderSpaceHeight;
        slot.TextWidth = 0;
        slot.TextHeight = 0;
        slotCollection.insert(slotCollection.end(), slot);
    }

    return true;
}

bool TextRenderer::DoesSlotExist(FontSlot slot) const
{
    SlotCollectionLoc collLoc = fonts.find(slot.FontID);
    if (collLoc == fonts.end()) return false;

    return (collLoc != fonts.end() && collLoc->second.size() < slot.SlotIndex);
}

int TextRenderer::GetNumbSlots(unsigned int fontID) const
{
    SlotCollectionLoc loc;
    if (!TryFindSlotCollection(fontID, loc)) return -1;

    return (int)loc->second.size();
}
Vector2i TextRenderer::GetSlotRenderSize(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return Vector2i();

    return Vector2i((int)slotP->Width, (int)slotP->Height);
}
Vector2i TextRenderer::GetSlotBoundingSize(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return Vector2i();

    return Vector2i((int)slotP->TextWidth, (int)slotP->TextHeight);
}
const char * TextRenderer::GetString(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return 0;

    return slotP->String;
}
RenderObjHandle TextRenderer::GetRenderedString(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return 0;

    return RTManager[slotP->RenderTargetID]->GetColorTextures()[0];
}



bool TextRenderer::RenderString(FontSlot slot, std::string textToRender, unsigned int backBufferWidth, unsigned int backBufferHeight)
{
    //Find the slot to use.
    SlotCollectionLoc loc;
    if (!TryFindSlotCollection(slot.FontID, loc)) return false;
    Slot * slotP;
    if (!TryFindSlot(slot.SlotIndex, fonts[slot.FontID], slotP)) return false;

    //Render into the slot.
    if (RenderString(textToRender, slot.FontID, RTManager[slotP->RenderTargetID], backBufferWidth, backBufferHeight))
    {
        slotP->String = textToRender.c_str();
        return true;
    }

    return false;
}
bool TextRenderer::RenderString(std::string textToRender, unsigned int fontID, RenderTarget * targ, unsigned int bbWidth, unsigned int bbHeight)
{
    //Get texture/render target.
    if (targ == 0) { errorMsg = "Associated render target did not exist!"; return false; }
    textRendererParams.TextureUniforms[textSamplerName].Texture = tempTex.GetTextureHandle();


    //Set up rendering.
    targ->EnableDrawingInto();
    glViewport(0, 0, targ->GetColorSettings()[0].Settings.Width, targ->GetColorSettings()[0].Settings.Height);
    RenderingState(RenderingState::C_BACK,
                   RenderingState::BlendingExpressions::BE_SOURCE_COLOR, RenderingState::BlendingExpressions::BE_ONE_MINUS_SOURCE_COLOR,
                   false, false).EnableState();
    ScreenClearer(true, true, false, Vector4f(0.1f, 0.0f, 0.0f, 0.0f)).ClearScreen();


    //Render each character into the texture, then into the final render target.
    Vector2f pos = Vector2f(-1.0f, 1.0f);
    Vector2i size = Vector2i(), offset = Vector2i(), movement = Vector2i();
    Vector2f scaledSize = Vector2f(), scaledOffset = Vector2f(), scaledMovement = Vector2f();
    Vector2f invRendTargSize(2.0f / (float)targ->GetColorSettings()[0].Settings.Width,
                             -2.0f / (float)targ->GetColorSettings()[0].Settings.Height);
    for (unsigned int i = 0; i < textToRender.size(); ++i)
    {
        char ch = textToRender.c_str()[i];

        //Render the character into an array.
        if (!FreeTypeHandler::Instance.RenderChar(fontID, ch))
        {
            errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + FreeTypeHandler::Instance.GetError();
            targ->DisableDrawingInto(bbWidth, bbHeight);
            return false;
        }


        //Compute character layout data.
        size = FreeTypeHandler::Instance.GetGlyphSize(fontID);
        scaledSize = ToV2f(size).ComponentProduct(invRendTargSize);
        offset = FreeTypeHandler::Instance.GetGlyphOffset(fontID);
        scaledOffset = ToV2f(offset).ComponentProduct(invRendTargSize);
        movement = FreeTypeHandler::Instance.GetMoveToNextGlyph(fontID);
        scaledMovement = ToV2f(movement).ComponentProduct(invRendTargSize);


        //If the character is empty (i.e. a space), don't bother rendering it.
        if (FreeTypeHandler::Instance.GetChar().GetWidth() > 0 && FreeTypeHandler::Instance.GetChar().GetHeight() > 0)
        {
            //Render the array into the temp texture.
            FreeTypeHandler::Instance.GetChar(tempTex);

            //Set up the render quad size/location.
            textRendererQuad->SetBounds(pos, pos + scaledSize);
            textRendererQuad->IncrementPos(Vector2f(scaledOffset.x, -(1.0f + scaledOffset.y)));
            textRendererQuad->MakeSizePositive();

            //Render the character into the render target.
            if (!textRendererQuad->Render(RenderPasses::BaseComponents, textRendererInfo, textRendererParams, *textRenderer))
            {
                errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + textRenderer->GetErrorMsg();
                targ->DisableDrawingInto(bbWidth, bbHeight);
                return false;
            }
        }

        //Move the quad to the next position for the letter.
        pos += scaledMovement;
    }

    targ->DisableDrawingInto(bbWidth, bbHeight);
    return true;
}


bool TextRenderer::TryFindSlotCollection(unsigned int fontID, SlotCollectionLoc & outCollection) const
{
    outCollection = fonts.find(fontID);
    if (outCollection == fonts.end())
    {
        errorMsg = "Couldn't find font ID " + std::to_string(fontID);
        return false;
    }

    return true;
}
bool TextRenderer::TryFindSlot(unsigned int slotNumb, const std::vector<Slot> & slots, const Slot *& outSlot) const
{
    if (slotNumb >= slots.size())
    {
        errorMsg = "Couldn't find slot index " + std::to_string(slotNumb);
        return false;
    }

    outSlot = &slots[slotNumb];
    return true;
}
bool TextRenderer::TryFindSlot(unsigned int slotNumb, std::vector<Slot> & slots, Slot *& outSlot)
{
    if (slotNumb >= slots.size())
    {
        errorMsg = "Couldn't find slot index " + std::to_string(slotNumb);
        return false;
    }

    outSlot = &slots[slotNumb];
    return true;
}
bool TextRenderer::TryFindFontSlot(FontSlot slot, const Slot*& outSlot) const
{
    TextRenderer::SlotCollectionLoc collLoc;
    if (!TryFindSlotCollection(slot.FontID, collLoc)) return false;
    return TryFindSlot(slot.SlotIndex, collLoc->second, outSlot);
}
bool TextRenderer::TryFindFontSlot(FontSlot slot, Slot*& outSlot)
{
    TextRenderer::SlotCollectionLoc collLoc;
    if (!TryFindSlotCollection(slot.FontID, collLoc)) return false;
    return TryFindSlot(slot.SlotIndex, fonts[slot.FontID], outSlot);
}