#include "GUIMaterials.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"


const std::string GUIMaterials::QuadDraw_Color = "u_color",
                  GUIMaterials::QuadDraw_Texture2D = "u_tex2D";
const std::string GUIMaterials::DynamicQuadDraw_TimeLerp = "u_timeLerp";


typedef DataNode::Ptr DNP;


ShaderGenerator::GeneratedMaterial GUIMaterials::GenerateStaticQuadDrawMaterial(UniformDictionary & params,
                                                                          ShaderInOutAttributes vertexAttrs,
                                                                          unsigned int posIndex,
                                                                          unsigned int uvIndex)
{
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
                                          QuadDraw_Texture2D, "GUIMat_texSamplerNode"));
    DNP texColor(new ParamNode(4, QuadDraw_Color, "GUIMat_texColor"));
    DNP texColored(new MultiplyNode(DataLine(texSample, TextureSample2DNode::GetOutputIndex(CO_AllChannels)),
                                    texColor, "GUIMat_finalColor"));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_Color4", texColored));

    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Transluscent);
    params.FloatUniforms[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    return genM;
}

ShaderGenerator::GeneratedMaterial GUIMaterials::GenerateDynamicQuadDrawMaterial(UniformDictionary & params,
                                                                                 DataLine endScale,
                                                                                 DataLine endColor,
                                                                                 DataLine endRotation,
                                                                                 ShaderInOutAttributes vertIns,
                                                                                 unsigned int posIndex,
                                                                                 unsigned int uvIndex)
{
    DataNode::ClearMaterialData();
    DataNode::VertexIns = vertIns;

    DNP vertexPosOut(new SpaceConverterNode(DataLine(VertexInputNode::GetInstance(), posIndex),
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                            SpaceConverterNode::DT_POSITION, "GUIMat_objToScreenPos"));
    DNP rotateScreen(new RotateAroundAxisNode(vertexPosOut, Vector3f(0.0f, 0.0f, 1.0f), endRotation, "GUIMat_rotate"));
    DNP screenScaleAmount(new CombineVectorNode(endScale, 1.0f, "GUIMat_scaleScreenAmt"));
    DNP scaleScreen(new MultiplyNode(rotateScreen, screenScaleAmount, "GUIMat_scaleScreen"));
    DNP finalPosOut(new CombineVectorNode(scaleScreen, 1.0f, "GUIMat_finalPosOut"));
    DataNode::MaterialOuts.VertexPosOutput = finalPosOut;

    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_UV",
                                                             DataLine(VertexInputNode::GetInstance(), uvIndex)));

    DNP texSample(new TextureSample2DNode(FragmentInputNode::GetInstance(),
                                          QuadDraw_Texture2D, "GUIMat_texSamplerNode"));
    DNP texColor(new ParamNode(4, QuadDraw_Color, "GUIMat_texColor"));
    DNP texColored(new MultiplyNode(DataLine(texSample, TextureSample2DNode::GetOutputIndex(CO_Red)),
                                    texColor, "GUIMat_finalColor"));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_Color4", texColored));

    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Transluscent);
    params.FloatUniforms[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    return genM;
}