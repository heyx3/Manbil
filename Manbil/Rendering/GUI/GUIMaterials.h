#pragma once

#include "../Data Nodes/ShaderGenerator.h"
#include "../Primitives/DrawingQuad.h"


//A static class that provides definitions for GUI system materials,
//    as well as methods to easily generate useful GUI materials.
class GUIMaterials : public ShaderGenerator
{
public:

    typedef ShaderGenerator::GeneratedMaterial GenMat;

    //Different kinds of texture input.
    enum TextureTypes
    {
        //A color texture.
        TT_COLOR,
        //A greyscale texture.
        TT_GREYSCALE,
        //Rendered text.
        TT_TEXT,
    };


    //The "color" parameter input for a GUI material.
    static const std::string QuadDraw_Color;
    //The "texture" parameter input for a GUI material.
    static const std::string QuadDraw_Texture2D;
    
    //The "time lerp" parameter input for a dynamic GUI material.
    //A value of 0 usually represents the base state of the object;
    //    a value of 1 usually represents the fully active state of the object.
    //The value should always be between 0 and 1.
    static const std::string DynamicQuadDraw_TimeLerp;


    //Generates a material that draws a texture multiplied by the QuadDraw_Color vec4.
    static GenMat GenerateStaticQuadDrawMaterial(UniformDictionary& params, TextureTypes textureType,
                                                 RenderIOAttributes vertexAttrs =
                                                    DrawingQuad::GetVertexInputData(),
                                                 unsigned int vertexPosIndex = 0,
                                                 unsigned int vertexUVIndex = 1);

    //Generates a material that dynamically changes its properties based on a time lerp uniform.
    static GenMat GenerateDynamicQuadDrawMaterial(UniformDictionary& params, TextureTypes texType,
                                                  DataLine endScale = Vector2f(1.0f, 1.0f),
                                                  DataLine endColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                                                  RenderIOAttributes vertexAttrs =
                                                    DrawingQuad::GetVertexInputData(),
                                                  unsigned int vertexPosIndex = 0,
                                                  unsigned int vertexUVIndex = 1);
};