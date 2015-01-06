#include "TextRenderer.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"
#include "../../ScreenClearer.h"


Material * TextRenderer::textRenderer = 0;
DrawingQuad * TextRenderer::textRendererQuad = 0;
UniformDictionary TextRenderer::textRendererParams = UniformDictionary();
RenderInfo TextRenderer::textRendererInfo = RenderInfo(0, 0, 0, 0, 0, 0);
Camera TextRenderer::textRendererCam = Camera();
TransformObject TextRenderer::textRendererTransform = TransformObject();
Matrix4f TextRenderer::viewMat = Matrix4f(),
         TextRenderer::projMat = Matrix4f(),
         TextRenderer::worldMat = Matrix4f();
MTexture2D TextRenderer::tempTex = MTexture2D(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_8U_GREYSCALE, false);

const char * textSamplerName = "u_charSampler";


std::string TextRenderer::InitializeSystem(SFMLOpenGLWorld * world)
{
    if (textRenderer != 0) return "System was already initialized.";

    ClearAllRenderingErrors();

    textRendererQuad = new DrawingQuad();
    tempTex.Create();

    std::string tError = GetCurrentRenderingError();
    if (!tError.empty())
        return "Error initializing temp tex: " + tError;


    //Transform matrices and render info.

    worldMat.SetAsIdentity();
    textRendererCam.GetViewTransform(viewMat);
    textRendererCam.GetOrthoProjection(projMat);

    textRendererInfo = RenderInfo(world, &textRendererCam, &textRendererTransform, &worldMat, &viewMat, &projMat);


    //Material.

    textRendererParams.ClearUniforms();
    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetAttributeData();

    DataNode::Ptr textSampler(new TextureSample2DNode(DataLine(FragmentInputNode::GetInstanceName()),
                                                      textSamplerName, "textSampler"));
    DataNode::Ptr objPosToWorld(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                                       SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                                       SpaceConverterNode::DT_POSITION, "objPosToWorld"));
    DataNode::Ptr vertexPosOut(new CombineVectorNode(DataLine(objPosToWorld->GetName()), DataLine(1.0f)));
    DataNode::Ptr textR(new SwizzleNode(DataLine(textSampler->GetName(),
                                                 TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllChannels)),
                                        SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X,
                                        "swizzleTextSample"));

    DataNode::MaterialOuts.VertexPosOutput = DataLine(vertexPosOut->GetName(), 0);

    std::vector<ShaderOutput> & vertOuts = DataNode::MaterialOuts.VertexOutputs,
                              & fragOuts = DataNode::MaterialOuts.FragmentOutputs;
    vertOuts.insert(vertOuts.end(), ShaderOutput("out_UV", DataLine(VertexInputNode::GetInstanceName(), 1)));
    fragOuts.insert(fragOuts.end(), ShaderOutput("out_FinalColor", DataLine(textR->GetName())));

    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(textRendererParams, RenderingModes::RM_Opaque);
    if (!genM.ErrorMessage.empty())
    {
        delete textRendererQuad;
        return "Error generating text renderer material: " + genM.ErrorMessage;
    }
    textRenderer = genM.Mat;

    return "";
}
void TextRenderer::DestroySystem(void)
{
    if (textRenderer == 0) return;

    delete textRendererQuad;
    delete textRenderer;
    textRendererQuad = 0;
    textRenderer = 0;
    textRendererParams.ClearUniforms();
    tempTex.DeleteIfValid();
}


TextRenderer::~TextRenderer(void)
{
    //Delete every font and every slot's render target.
    for (auto font = fonts.begin(); font != fonts.end(); ++font)
    {
        FreeTypeHandler::Instance.DeleteFont(font->first);

        for (auto slot = font->second.begin(); slot != font->second.end(); ++slot)
        {
            RTManager.DeleteRenderTarget(slot->RenderTargetID);
            delete slot->ColorTex;
        }
    }
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
bool TextRenderer::CreateTextRenderSlots(unsigned int fontID, unsigned int renderSpaceWidth, unsigned int renderSpaceHeight,
                                         bool useMipmaps, const TextureSampleSettings2D & settings,
                                         unsigned int numbSlots)
{
    SlotCollectionLoc loc;
    if (!TryFindSlotCollection(fontID, loc)) return false;
    std::vector<Slot> & slotCollection = fonts[fontID];

    //Create the given number of slots.
    for (unsigned int i = 0; i < numbSlots; ++i)
    {
        //Create render target.
        unsigned int rendTargetID = RTManager.CreateRenderTarget(PS_16U_DEPTH);
        if (rendTargetID == RenderTargetManager::ERROR_ID)
        {
            errorMsg = "Error creating render target: " + RTManager.GetError();
            return false;
        }

        //Add the slot to the collection.
        Slot slot;
        slot.RenderTargetID = rendTargetID;
        slot.ColorTex = new MTexture2D(settings, PS_8U_GREYSCALE, true);
        slot.String = "";
        slot.TextWidth = 0;
        slot.TextHeight = 0;
        slotCollection.insert(slotCollection.end(), slot);

        //Update the render target to use the color texture.
        slot.ColorTex->Create(settings, useMipmaps, PS_8U_GREYSCALE);
        slot.ColorTex->ClearData(renderSpaceWidth, renderSpaceHeight);
        if (!RTManager[rendTargetID]->SetColorAttachment(RenderTargetTex(slot.ColorTex), true))
        {
            errorMsg = "Error attaching color texture to render target: " + RTManager[rendTargetID]->GetErrorMessage();
            slotCollection.erase(slotCollection.end() - 1);
            delete slot.ColorTex;
            return false;
        }

        //Check that the render target is good.
        if (!RTManager[rendTargetID]->IsUseable())
        {
            errorMsg = "Render target is not usable: " + RTManager[rendTargetID]->GetErrorMessage();
            slotCollection.erase(slotCollection.end() - 1);
            delete slot.ColorTex;
            return false;
        }
    }

    return true;
}

bool TextRenderer::DoesSlotExist(FontSlot slot) const
{
    SlotCollectionLoc collLoc = fonts.find(slot.FontID);
    if (collLoc == fonts.end()) return false;

    //PRIORITY: Shouldn't this be "slot.SlotIndex < collLoc->second.size()"?
    return (collLoc != fonts.end() && collLoc->second.size() < slot.SlotIndex);
}

int TextRenderer::GetNumbSlots(FreeTypeHandler::FontID fontID) const
{
    SlotCollectionLoc loc;
    if (!TryFindSlotCollection(fontID, loc)) return -1;

    return (int)loc->second.size();
}
Vector2i TextRenderer::GetSlotRenderSize(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return Vector2i();

    return Vector2i((int)slotP->ColorTex->GetWidth(), (int)slotP->ColorTex->GetHeight());
}
Vector2i TextRenderer::GetSlotBoundingSize(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return Vector2i();

    return Vector2i((int)slotP->TextWidth, (int)slotP->TextHeight);
}
std::string TextRenderer::GetString(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return 0;

    return slotP->String;
}
MTexture2D * TextRenderer::GetRenderedString(FontSlot slot) const
{
    const Slot * slotP;
    if (!TryFindFontSlot(slot, slotP)) return 0;

    return slotP->ColorTex;
}



bool TextRenderer::RenderString(FontSlot slot, std::string textToRender, unsigned int backBufferWidth, unsigned int backBufferHeight)
{
    //Find the slot to use.
    SlotCollectionLoc loc;
    if (!TryFindSlotCollection(slot.FontID, loc)) return false;
    Slot * slotP;
    if (!TryFindSlot(slot.SlotIndex, fonts[slot.FontID], slotP)) return false;
   
    //Render into the slot.
    if (RenderString(textToRender, slot.FontID, RTManager[slotP->RenderTargetID],
                     slotP->TextWidth, slotP->TextHeight, backBufferWidth, backBufferHeight))
    {
        slotP->String = textToRender;
        return true;
    }

    return false;
}

bool TextRenderer::RenderString(std::string textToRender, unsigned int fontID, RenderTarget * targ,
                                unsigned int & outTextWidth, unsigned int & outTextHeight,
                                unsigned int bbWidth, unsigned int bbHeight)
{
    //Get texture/render target.
    if (targ == 0) { errorMsg = "Associated render target did not exist!"; return false; }
    textRendererParams.Texture2DUniforms[textSamplerName].Texture = tempTex.GetTextureHandle();

    //Set up rendering.
    targ->EnableDrawingInto();
    RenderingState(RenderingState::C_BACK,
                   RenderingState::BE_SOURCE_COLOR, RenderingState::BE_ONE_MINUS_SOURCE_COLOR,
                   false, false).EnableState();
    ScreenClearer(true, true, false, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)).ClearScreen();


    //Render each character into the texture, then into the final render target.
    outTextWidth = 0;
    outTextHeight = 0;
    Vector2f pos = Vector2f(-1.0f, 1.0f);
    Vector2i size = Vector2i(), offset = Vector2i(), movement = Vector2i();
    Vector2f scaledSize = Vector2f(), scaledOffset = Vector2f(), scaledMovement = Vector2f();
    Vector2f invRendTargSize(2.0f / (float)targ->GetWidth(), -2.0f / (float)targ->GetHeight());
    for (unsigned int i = 0; i < textToRender.size(); ++i)
    {
        char ch = textToRender.c_str()[i];

        //Render the character into an array.
        if (FreeTypeHandler::Instance.RenderChar(fontID, ch) == FreeTypeHandler::CharRenderType::CRT_ERROR)
        {
            errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + FreeTypeHandler::Instance.GetError();
            targ->DisableDrawingInto(bbWidth, bbHeight, true);
            return false;
        }


        //Compute character layout data.
        size = FreeTypeHandler::Instance.GetGlyphSize(fontID);
        scaledSize = ToV2f(size).ComponentProduct(invRendTargSize);
        offset = FreeTypeHandler::Instance.GetGlyphOffset(fontID);
        scaledOffset = ToV2f(offset).ComponentProduct(invRendTargSize);
        movement = FreeTypeHandler::Instance.GetMoveToNextGlyph(fontID);
        scaledMovement = ToV2f(movement).ComponentProduct(invRendTargSize);

        outTextWidth += movement.x;
        outTextHeight = Mathf::Max(size.y, (int)outTextHeight);


        //If the character is empty (i.e. a space), don't bother rendering it.
        if (FreeTypeHandler::Instance.GetGlyphSize(fontID).x > 0 && FreeTypeHandler::Instance.GetGlyphSize(fontID).y > 0)
        {
            //Render the array into the temp texture.
            FreeTypeHandler::Instance.GetChar(tempTex);

            //Set up the render quad size/location.
            textRendererQuad->SetBounds(pos, pos + scaledSize);
            textRendererQuad->IncrementPos(Vector2f(scaledOffset.x, -(1.0f + scaledOffset.y)));
            textRendererQuad->MakeSizePositive();

            //Render the character into the render target.
            if (!textRendererQuad->Render(textRendererInfo, textRendererParams, *textRenderer))
            {
                errorMsg = std::string() + "Error rendering character #" + std::to_string(i) + ", '" + ch + "': " + textRenderer->GetErrorMsg();
                targ->DisableDrawingInto(bbWidth, bbHeight, true);
                return false;
            }
        }

        //Move the quad to the next position for the letter.
        pos += scaledMovement;
    }

    outTextWidth -= movement.x;
    outTextWidth += size.x;

    targ->DisableDrawingInto(bbWidth, bbHeight, true);
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
bool TextRenderer::TryFindSlot(unsigned int slotNumb,
                               const std::vector<Slot> & slots, const Slot *& outSlot) const
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