#include "GUIMaterials.h"

#include "../Data Nodes/DataNodes.hpp"


const std::string GUIMaterials::QuadDraw_Color = "u_color",
                  GUIMaterials::QuadDraw_Texture2D = "u_tex2D";
const std::string GUIMaterials::DynamicQuadDraw_TimeLerp = "u_timeLerp";


typedef DataNode::Ptr DNP;


typedef GUIMaterials::GenMat GenM;
GenM GUIMaterials::GenerateStaticQuadDrawMaterial(UniformDictionary& params, TextureTypes texType,
                                                  RenderIOAttributes vertexAttrs,
                                                  unsigned int posIndex, unsigned int uvIndex,
                                                  bool useTex, bool isOpaque)
{
    BlendMode blendMode = (isOpaque ? BlendMode::GetOpaque() : BlendMode::GetTransparent());

    SerializedMaterial matData(vertexAttrs);

    //For the vertex shader, output screen position and UVs.
    DNP vertexPosOut(new SpaceConverterNode(DataLine(VertexInputNode::GetInstance(), posIndex),
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                            SpaceConverterNode::DT_POSITION,
                                            "GUIMat_objToScreenPos"));
    matData.MaterialOuts.VertexPosOutput = DataLine(vertexPosOut, 1);
    matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_UV",
                                                              DataLine(VertexInputNode::GetInstance(),
                                                                       uvIndex)));

    //For the fragment shader, multiply the texture parameter by the color parameter and output that.
    if (useTex)
    {
        DNP texSample(new TextureSample2DNode(FragmentInputNode::GetInstance(),
                                              QuadDraw_Texture2D, "GUIMat_texSampler"));
        DataLine texRGBA(texSample, TextureSample2DNode::GetOutputIndex(CO_AllChannels));
        //Depending on the type of thing being rendered (color, greyscale, or text),
        //    change how the texture is sampled.
        DataLine finalTexCol;
        DNP extraNode1, extraNode2;
        switch (texType)
        {
            case TT_COLOR:
                finalTexCol = texRGBA;
                break;
            case TT_GREYSCALE:
                extraNode1 = DNP(new SwizzleNode(texRGBA,
                                                 SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X,
                                                 "GUIMat_swizzleTex"));
                extraNode2 = DNP(new CombineVectorNode(extraNode1, 1.0f, "GUIMat_finalTexColor"));
                finalTexCol = extraNode2;
                break;
            case TT_TEXT:
                extraNode1 = DNP(new SwizzleNode(texRGBA,
                                                 SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X,
                                                 SwizzleNode::C_X,
                                                 "GUIMat_swizzleTex"));
                finalTexCol = extraNode1;
                break;

            default:
                assert(false);
                return GenM("Unknown texture type");
        }
        DNP paramColor(new ParamNode(4, QuadDraw_Color, "GUIMat_texColor"));
        DNP finalColor(new MultiplyNode(finalTexCol, paramColor, "GUIMat_finalColor"));
        
        matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_FinalColor", finalColor));

        //Try to generate the material.
        GenM genM = ShaderGenerator::GenerateMaterial(matData, params, blendMode);
        params.Floats[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        return genM;
    }
    else
    {
        DNP finalColor(new ParamNode(4, QuadDraw_Color, "GUIMat_color"));
        
        matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_FinalColor", finalColor));

        //Try to generate the material.
        GenM genM = ShaderGenerator::GenerateMaterial(matData, params, blendMode);
        params.Floats[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        return genM;
    }
}

GenM GUIMaterials::GenerateDynamicQuadDrawMaterial(UniformDictionary& params, TextureTypes texType,
                                                   DataLine endScale, DataLine endColor,
                                                   RenderIOAttributes vertIns,
                                                   unsigned int posIndex, unsigned int uvIndex,
                                                   bool useTex, bool isOpaque)
{
    if (endScale.GetSize() != 2)
    {
        return GenM("'endScale' parameter isn't size 2");
    }
    if (endColor.GetSize() != 4)
    {
        return GenM("'endColor' parameter isn't size 4");
    }

    BlendMode blendMode = (isOpaque ? BlendMode::GetOpaque() : BlendMode::GetTransparent());
    
    SerializedMaterial matData(vertIns);

    //For the vertex shader, scale up the element based on the time lerp
    //    and output screen position and UVs.
    DNP screenScaleAmount(new CombineVectorNode(endScale, 1.0f, "GUIMat_scaleScreenAmt"));
    DNP scalePos(new MultiplyNode(DataLine(VertexInputNode::GetInstance(), posIndex),
                                  screenScaleAmount, "GUIMat_scaleElement"));
    DNP vertexPosOut(new SpaceConverterNode(scalePos,
                                            SpaceConverterNode::ST_OBJECT,
                                            SpaceConverterNode::ST_SCREEN,
                                            SpaceConverterNode::DT_POSITION, "GUIMat_objToScreenPos"));
    matData.MaterialOuts.VertexPosOutput = DataLine(vertexPosOut, 1);
    matData.MaterialOuts.VertexOutputs.insert(matData.MaterialOuts.VertexOutputs.end(),
                                              ShaderOutput("vOut_UV",
                                                           DataLine(VertexInputNode::GetInstance(),
                                                                    uvIndex)));
    
    //For the fragment shader, combine the color parameter, texture parameter, and animated color.
    if (useTex)
    {
        DNP texSample(new TextureSample2DNode(FragmentInputNode::GetInstance(),
                                              QuadDraw_Texture2D, "GUIMat_texSamplerNode"));
        DataLine texRGBA(texSample, TextureSample2DNode::GetOutputIndex(CO_AllChannels));
        //Depending on the type of thing being rendered (color, greyscale, or text),
        //    change how the texture is sampled.
        DataLine finalTexCol;
        DNP extraNode1, extraNode2;
        switch (texType)
        {
            case TT_COLOR:
                finalTexCol = texRGBA;
                break;
            case TT_GREYSCALE:
                extraNode1 = DNP(new SwizzleNode(texRGBA,
                                                 SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X,
                                                 "GUIMat_swizzleTex_Grey"));
                extraNode2 = DNP(new CombineVectorNode(extraNode1, 1.0f, "GUIMat_finalTexColor"));
                finalTexCol = extraNode2;
                break;
            case TT_TEXT:
                extraNode1 = DNP(new SwizzleNode(texRGBA,
                                                 SwizzleNode::C_X, SwizzleNode::C_X, SwizzleNode::C_X,
                                                 SwizzleNode::C_X,
                                                 "GUIMat_swizzleTex_Text"));
                finalTexCol = extraNode1;
                break;

            default:
                assert(false);
                return GenM("Unknown texture type");
        }
        DNP paramColor(new ParamNode(4, QuadDraw_Color, "GUIMat_texColor"));
        DNP finalColor(new MultiplyNode(finalTexCol, paramColor, endColor, "GUIMat_finalColor"));
        
        matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color4", finalColor));

        GenM genM = ShaderGenerator::GenerateMaterial(matData, params, blendMode);
        params.Floats[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        return genM;
    }
    else
    {
        DNP finalColor(new ParamNode(4, QuadDraw_Color, "GUIMat_color"));
        
        matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color4", finalColor));

        GenM genM = ShaderGenerator::GenerateMaterial(matData, params, blendMode);
        params.Floats[QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        return genM;
    }
}