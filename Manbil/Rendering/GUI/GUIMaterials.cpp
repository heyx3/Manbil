#include "GUIMaterials.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"


const std::string GUIMaterials::QuadDraw_Color = "u_color",
                  GUIMaterials::QuadDraw_Texture2D = "u_tex2D";
const std::string GUIMaterials::DynamicQuadDraw_TimeLerp = "u_timeLerp";


typedef DataNode::Ptr DNP;


ShaderGenerator::GeneratedMaterial GUIMaterials::GenerateStaticQuadDrawMaterial(UniformDictionary & params,
                                                                          bool greyscale,
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
                                          QuadDraw_Texture2D, "GUIMat_texSampler"));
    DataLine finalTexCol;
    DNP extraNode;
    if (greyscale)
    {
        extraNode = DNP(new SwizzleNode(DataLine(texSample, TextureSample2DNode::GetOutputIndex(CO_AllChannels)),
                                        SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_W,
                                        "GUIMat_swizzleTex"));
        finalTexCol = extraNode;
    }
    else
    {
        finalTexCol = DataLine(texSample, TextureSample2DNode::GetOutputIndex(CO_AllChannels));
    }
    DNP paramColor(new ParamNode(4, QuadDraw_Color, "GUIMat_texColor"));
    DNP finalColor(new MultiplyNode(finalTexCol, paramColor, "GUIMat_finalColor"));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_Color4", finalColor));

    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Transluscent);
    params.FloatUniforms[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    return genM;
}

ShaderGenerator::GeneratedMaterial GUIMaterials::GenerateDynamicQuadDrawMaterial(UniformDictionary & params,
                                                                                 bool greyscale,
                                                                                 DataLine endScale,
                                                                                 DataLine endColor,
                                                                                 ShaderInOutAttributes vertIns,
                                                                                 unsigned int posIndex,
                                                                                 unsigned int uvIndex)
{
    if (endScale.GetSize() != 2) return ShaderGenerator::GeneratedMaterial("'endScale' parameter isn't size 2");
    if (endColor.GetSize() != 4) return ShaderGenerator::GeneratedMaterial("'endColor' parameter isn't size 4");

    DataNode::ClearMaterialData();
    DataNode::VertexIns = vertIns;

    DNP screenScaleAmount(new CombineVectorNode(endScale, 1.0f, "GUIMat_scaleScreenAmt"));
    DNP scalePos(new MultiplyNode(DataLine(VertexInputNode::GetInstance(), posIndex),
                                  screenScaleAmount, "GUIMat_scaleElement"));
    DNP vertexPosOut(new SpaceConverterNode(scalePos,
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                            SpaceConverterNode::DT_POSITION, "GUIMat_objToScreenPos"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(vertexPosOut, 1);

    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_UV",
                                                             DataLine(VertexInputNode::GetInstance(), uvIndex)));

    DNP paramColor(new ParamNode(4, QuadDraw_Color, "GUIMat_texColor"));
    DNP texSample(new TextureSample2DNode(FragmentInputNode::GetInstance(),
                                         QuadDraw_Texture2D, "GUIMat_texSamplerNode"));DataLine finalTexCol;
    DNP extraNode;
    if (greyscale)
    {
        extraNode = DNP(new SwizzleNode(DataLine(texSample, TextureSample2DNode::GetOutputIndex(CO_AllChannels)),
                                        SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X,
                                        "GUIMat_swizzleTex"));
        finalTexCol = extraNode;
    }
    else
    {
        finalTexCol = DataLine(texSample, TextureSample2DNode::GetOutputIndex(CO_AllChannels));
    }
    DNP finalColor(new MultiplyNode(finalTexCol, paramColor, endColor, "GUIMat_finalColor"));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_Color4", finalColor));

    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Transluscent);
    params.FloatUniforms[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    return genM;
}