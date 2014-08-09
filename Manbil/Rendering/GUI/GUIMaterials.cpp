#include "GUIMaterials.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"


const std::string GUIMaterials::QuadDraw_Color = "u_color",
                  GUIMaterials::QuadDraw_Texture2D = "u_tex2D";

ShaderGenerator::GeneratedMaterial GUIMaterials::GenerateQuadDrawMaterial(UniformDictionary & params,
                                                                          ShaderInOutAttributes vertexAttrs,
                                                                          unsigned int posIndex, unsigned int uvIndex)
{
    typedef DataNode::Ptr DNP;

    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetAttributeData();

    DNP vertexPosOut(new SpaceConverterNode(DataLine(VertexInputNode::GetInstance(), posIndex),
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                            SpaceConverterNode::DT_POSITION, "GUIMat_objToScreenPos"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(vertexPosOut, 1);

    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_UV",
                                                             DataLine(VertexInputNode::GetInstance(), uvIndex)));

    DNP texSample(new TextureSample2DNode(FragmentInputNode::GetInstance(),
                                          QuadDraw_Texture2D, "GUIMat_textSamplerNode"));
    DNP texColor(new ParamNode(4, QuadDraw_Color, "GUIMat_textColor"));
    DNP texColored(new MultiplyNode(DataLine(texSample, TextureSample2DNode::GetOutputIndex(CO_Red)),
                                    texColor, "GUIMat_finalColor"));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_Color4", texColored));

    return ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Transluscent);
}