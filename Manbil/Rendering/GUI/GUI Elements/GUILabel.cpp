#include "GUILabel.h"

#include "../../Materials/Data Nodes/DataNodeIncludes.h"




const std::string GUILabel::TextColor = "u_textColor",
                  GUILabel::textSampler = "u_textSampler";

ShaderGenerator::GeneratedMaterial GUILabel::GenerateLabelMaterial(UniformDictionary & params)
{
    typedef DataNode::Ptr DNP;

    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetAttributeData();

    DNP vertexPosOut(new SpaceConverterNode(DataLine(VertexInputNode::GetInstance(), 0),
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                            SpaceConverterNode::DT_POSITION, "GUILabel_objToScreenPos"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(vertexPosOut, 1);
    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_UV", DataLine(VertexInputNode::GetInstance(), 1)));
    DNP textSample(new TextureSample2DNode(FragmentInputNode::GetInstance(), textSampler, "GUILabel_textSamplerNode"));
    DNP textColor(new ParamNode(4, TextColor, "GUILabel_textColor"));
    DNP textColored(new MultiplyNode(DataLine(textSample, TextureSample2DNode::GetOutputIndex(CO_Red)),
                                     textColor));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_Color4", textColored));

    return ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Transluscent);
}


bool GUILabel::SetText(std::string newText)
{
    if (TextRender->RenderString(TextRenderSlot, newText))
    {
        text = newText;
        return true;
    }
    else return false;
}

Vector2i GUILabel::GetCollisionDimensions(void) const
{
    return Vector2f((float)dimensions.x, (float)dimensions.y).ComponentProduct(Scale).CastToInt() + Vector2i(1, 1);
}
std::string GUILabel::Render(float elapsedTime, const RenderInfo & info)
{
    Vector2i textSize = TextRender->GetSlotBoundingSize(TextRenderSlot);

    Vector2i textOffset;
    switch (OffsetHorz)
    {
        case HO_LEFT:
            textOffset.x = 0;
            break;
        case HO_CENTER:
            textOffset.x = textSize.x / 2;
            break;
        case HO_RIGHT:
            textOffset.x = textSize.x;
            break;
        default: assert(false);
    }
    switch (OffsetVert)
    {
        case VO_TOP:
            textOffset.y = 0;
            break;
        case VO_CENTER:
            textOffset.y = textSize.y / 2;
            break;
        case VO_BOTTOM:
            textOffset.y = textSize.y;
            break;
        default: assert(false);
    }

    float invWidth = 1.0f / info.Cam->Info.Width,
          invHeight = 1.0f / info.Cam->Info.Height;

    Vector2f pos(invWidth, invHeight);
    pos.MultiplyComponents(Vector2f(center.x, center.y));
    Vector2f scale(invWidth, invHeight);
    scale.MultiplyComponents(Scale.ComponentProduct(Vector2f(textSize.x, textSize.y)));

    GetQuad()->SetPos(pos);
    GetQuad()->SetSize(scale * 0.5f);
    GetQuad()->SetRotation(0.0f);

    Params.Texture2DUniforms[textSampler].Texture = TextRender->GetRenderedString(TextRenderSlot)->GetTextureHandle();

    if (GetQuad()->Render(info, Params, *RenderMat)) return "";
    return "Error rendering label with text '" + text + "': " + RenderMat->GetErrorMsg();
}