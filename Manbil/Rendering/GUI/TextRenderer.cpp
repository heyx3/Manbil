#include "TextRenderer.h"

#include "../Data Nodes/DataNodes.hpp"
#include "../../Rendering/Basic Rendering/RenderingState.h"
#include "../Basic Rendering/BlendMode.h"
#include "../Data Nodes/ShaderGenerator.h"
#include "../../Rendering/Basic Rendering/ScreenClearer.h"


typedef TextRenderer TR;


Material* TR::textRenderer = 0;
UniformDictionary TR::textRendererParams = UniformDictionary();
RenderInfo TR::textRendererInfo = RenderInfo(0, 0, 0, 0);
Camera TR::textRendererCam = Camera();
Matrix4f TR::viewMat = Matrix4f(),
         TR::projMat = Matrix4f();
MTexture2D TR::tempTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP),
                       PS_8U_GREYSCALE, false);

const char* textSamplerName = "u_charSampler";


std::string TR::InitializeSystem(void)
{
    if (textRenderer != 0)
    {
        return "System was already initialized.";
    }

    ClearAllRenderingErrors();

    tempTex.Create();


    //Transform matrices and render info.

    textRendererCam.GetViewTransform(viewMat);
    textRendererCam.GetOrthoProjection(projMat);

    textRendererInfo = RenderInfo(0.0f, &textRendererCam, &viewMat, &projMat);


    //Material.

    textRendererParams.ClearUniforms();
    SerializedMaterial matData(DrawingQuad::GetVertexInputData());

    //Use a simple vertex shader that just uses world position -- in other words,
    //    the visible range in world space is just the volume from {-1, -1, -1} to {1, 1, 1}.
    //It also outputs UVs for the fragment shader to use.
    DataLine vIn_Pos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1);
    DataNode::Ptr objPosToWorld(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                                       SpaceConverterNode::ST_OBJECT,
                                                       SpaceConverterNode::ST_WORLD,
                                                       SpaceConverterNode::DT_POSITION,
                                                       "objPosToWorld"));
    DataNode::Ptr vertexPosOut(new CombineVectorNode(DataLine(objPosToWorld->GetName()),
                                                     DataLine(1.0f)));
    matData.MaterialOuts.VertexPosOutput = vertexPosOut;

    //The fragment shader just samples from the texture containing the text char
    //    and uses its "red" value because it's a grayscale texture.
    matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_UV", vIn_UV));
    DataLine fIn_UV(FragmentInputNode::GetInstance(), 0);
    DataNode::Ptr textSampler(new TextureSample2DNode(fIn_UV, textSamplerName, "textSampler"));
    DataLine textSamplerRGBA(textSampler, TextureSample2DNode::GetOutputIndex(CO_AllChannels));
    DataNode::Ptr textColor(new SwizzleNode(textSamplerRGBA,
                                            SwizzleNode::C_X, SwizzleNode::C_X,
                                            SwizzleNode::C_X, SwizzleNode::C_X,
                                            "swizzleTextSample"));
    matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", textColor));

    BlendMode blending = BlendMode::GetTransparent();
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(matData,
                                                                                textRendererParams,
                                                                                blending);
    if (!genM.ErrorMessage.empty())
    {
        return "Error generating text renderer material: " + genM.ErrorMessage;
    }
    textRenderer = genM.Mat;

    return "";
}
void TR::DestroySystem(void)
{
    if (textRenderer == 0)
    {
        return;
    }

    delete textRenderer;
    textRenderer = 0;
    textRendererParams.ClearUniforms();
    tempTex.DeleteIfValid();
}


TR::~TR(void)
{
    //Delete every font.
    while (!fonts.empty())
    {
        bool tryDelete = DeleteFont(fonts.begin()->first);
        assert(tryDelete);
    }
}


unsigned int TR::CreateAFont(std::string fontPath, std::string& errorMsg,
                             unsigned int pixelWidth, unsigned int pixelHeight)
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
    fonts[fontID] = std::unordered_map<unsigned int, Slot>();
    return fontID;
}
bool TR::DeleteFont(FreeTypeHandler::FontID font)
{
    auto found = fonts.find(font);
    if (found != fonts.end())
    {
        //Delete the render slots.
        SlotCollection& slots = found->second;
        while (!slots.empty())
        {
            bool b = DeleteTextRenderSlot(FontSlot(font, slots.begin()->first));
            assert(b);
        }

        //Remove the font from this instance and from FreeType.
        fonts.erase(found);
        bool b = FreeTypeHandler::Instance.DeleteFont(font);
        assert(b);

        return true;
    }
    return false;
}

TR::FontSlot TR::CreateTextRenderSlot(FreeTypeHandler::FontID fontID, std::string& errorMsg,
                                      unsigned int renderSpaceWidth, unsigned int renderSpaceHeight,
                                      bool useMipmaps, TextureSampleSettings2D settings)
{
    //Make sure the font exists.
    SlotCollection* slots = TryFindSlotCollection(fontID);
    if (slots == 0)
    {
        errorMsg = "Couldn't find font ID " + std::to_string(fontID);
        return TR::FontSlot();
    }

    //Create render target.
    unsigned int rendTargetID = RTManager.CreateRenderTarget(PS_16U_DEPTH, errorMsg);
    if (rendTargetID == RenderTargetManager::ERROR_ID)
    {
        errorMsg = "Error creating render target: " + errorMsg;
        return TR::FontSlot();
    }

    //Find the first unused slot ID.
    unsigned int slotID = 0;
    while (slots->find(slotID) != slots->end())
    {
        slotID += 1;
    }
    //Add the slot to the collection.
    Slot& slot = slots->operator[](slotID);
    slot.RenderTargetID = rendTargetID;
    slot.ColorTex = new MTexture2D(settings, PS_8U_GREYSCALE, true);
    slot.String = "";
    slot.TextWidth = 0;
    slot.TextHeight = 0;

    //Update the render target to use the color texture.
    slot.ColorTex->Create(settings, useMipmaps, PS_8U_GREYSCALE);
    slot.ColorTex->ClearData(renderSpaceWidth, renderSpaceHeight);
    if (!RTManager[rendTargetID]->SetColorAttachment(RenderTargetTex(slot.ColorTex), true))
    {
        errorMsg = "Error attaching color texture to render target; it may be too big.";
        slots->erase(slotID);
        delete slot.ColorTex;
        return TR::FontSlot();
    }

    //Check that the render target is good.
    std::string err;
    if (!RTManager[rendTargetID]->IsUseable(err))
    {
        errorMsg = "Render target is not usable: " + err;
        slots->erase(slotID);
        delete slot.ColorTex;
        return TR::FontSlot();
    }

    return TR::FontSlot(fontID, slotID);
}
bool TR::DeleteTextRenderSlot(FontSlot slot)
{
    if (DoesSlotExist(slot))
    {
        Slot* slt = TryFindSlot(slot.SlotID, TryFindSlotCollection(slot.FontID));

        delete slt->ColorTex;
        RTManager.DeleteRenderTarget(slt->RenderTargetID);

        fonts[slot.FontID].erase(slot.SlotID);
        return true;
    }
    return false;
}

bool TR::DoesSlotExist(FontSlot slot) const
{
    auto slotCollection = fonts.find(slot.FontID);
    if (slotCollection == fonts.end())
    {
        return false;
    }

    return slotCollection->second.find(slot.SlotID) != slotCollection->second.end();
}

int TR::GetNumbSlots(FreeTypeHandler::FontID fontID) const
{
    const SlotCollection* slots = TryFindSlotCollection(fontID);
    if (slots == 0)
    {
        return -1;
    }

    return (int)slots->size();
}
Vector2i TR::GetSlotRenderSize(FontSlot slot) const
{
    const Slot* slotP = TryFindFontSlot(slot);
    if (slotP == 0)
    {
        return Vector2i();
    }

    return Vector2i((int)slotP->ColorTex->GetWidth(), (int)slotP->ColorTex->GetHeight());
}
Vector2i TR::GetSlotBoundingSize(FontSlot slot) const
{
    const Slot* slotP = TryFindFontSlot(slot);
    if (slotP == 0)
    {
        return Vector2i();
    }

    return Vector2i((int)slotP->TextWidth, (int)slotP->TextHeight);
}

Vector2u TR::GetMaxCharacterSize(FreeTypeHandler::FontID id) const
{
    return FreeTypeHandler::Instance.GetGlyphMaxSize(id);
}
std::string TR::GetString(FontSlot slot) const
{
    const Slot* slotP = TryFindFontSlot(slot);
    if (slotP == 0)
    {
        return 0;
    }

    return slotP->String;
}
MTexture2D* TR::GetRenderedString(FontSlot slot) const
{
    const Slot* slotP = TryFindFontSlot(slot);
    if (slotP == 0)
    {
        return 0;
    }

    return slotP->ColorTex;
}



bool TR::RenderString(FontSlot slot, std::string textToRender)
{
    //Find the slot to use.
    SlotCollection* slots = TryFindSlotCollection(slot.FontID);
    if (slots == 0)
    {
        return false;
    }
    Slot* slotP = TryFindSlot(slot.SlotID, slots);
    if (slotP == 0)
    {
        return false;
    }
   
    //Render into the slot.
    if (RenderString(textToRender, slot.FontID, RTManager[slotP->RenderTargetID],
                     slotP->TextWidth, slotP->TextHeight))
    {
        slotP->String = textToRender;
        return true;
    }

    return false;
}

bool TR::RenderString(std::string textToRender, unsigned int fontID, RenderTarget* targ,
                      unsigned int& outTextWidth, unsigned int& outTextHeight)
{
    //Get texture/render target.
    if (targ == 0)
    {
        return false;
    }
    textRendererParams.Texture2Ds[textSamplerName].Texture = tempTex.GetTextureHandle();

    //Set up rendering.
    targ->EnableDrawingInto();
    RenderingState(RenderingState::C_BACK, false, false).EnableState();
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
        if (FreeTypeHandler::Instance.RenderChar(fontID, ch) ==
            FreeTypeHandler::CharRenderType::CRT_ERROR)
        {
            targ->DisableDrawingInto(true);
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
        Vector2i glyphSize = FreeTypeHandler::Instance.GetGlyphSize(fontID);
        if (glyphSize.x > 0 && glyphSize.y > 0)
        {
            //Render the array into the temp texture.
            FreeTypeHandler::Instance.GetChar(tempTex);

            //Set up the render quad size/location.
            DrawingQuad::GetInstance()->SetBounds(pos, pos + scaledSize);
            DrawingQuad::GetInstance()->IncrementPos(Vector2f(scaledOffset.x,
                                                              -(1.0f + scaledOffset.y)));
            DrawingQuad::GetInstance()->MakeSizePositive();

            //Render the character into the render target.
            DrawingQuad::GetInstance()->Render(textRendererInfo, textRendererParams, *textRenderer);
        }

        //Move the quad to the next position for the letter.
        pos += scaledMovement;
    }

    outTextWidth -= movement.x;
    outTextWidth += size.x;

    targ->DisableDrawingInto(true);
    return true;
}


const TR::SlotCollection* TR::TryFindSlotCollection(FreeTypeHandler::FontID fontID) const
{
    auto tryFind = fonts.find(fontID);
    if (tryFind == fonts.end())
    {
        return 0;
    }

    return &tryFind->second;
}
const TR::Slot* TR::TryFindSlot(unsigned int slotID, const SlotCollection* slots) const
{
    auto tryFind = slots->find(slotID);
    if (tryFind == slots->end())
    {
        return 0;
    }

    return &tryFind->second;
}
const TR::Slot* TR::TryFindFontSlot(FontSlot slot) const
{
    const TR::SlotCollection* slots = TryFindSlotCollection(slot.FontID);
    if (slots == 0)
    {
        return 0;
    }

    return TryFindSlot(slot.SlotID, slots);
}

TR::SlotCollection* TR::TryFindSlotCollection(FreeTypeHandler::FontID fontID)
{
    auto tryFind = fonts.find(fontID);
    if (tryFind == fonts.end())
    {
        return 0;
    }

    return &tryFind->second;
}
TR::Slot* TR::TryFindSlot(unsigned int slotID, SlotCollection* slots)
{
    auto tryFind = slots->find(slotID);
    if (tryFind == slots->end())
    {
        return 0;
    }

    return &tryFind->second;
}
TR::Slot* TR::TryFindFontSlot(FontSlot slot)
{
    TR::SlotCollection* slots = TryFindSlotCollection(slot.FontID);
    if (slots == 0)
    {
        return 0;
    }

    return TryFindSlot(slot.SlotID, slots);
}