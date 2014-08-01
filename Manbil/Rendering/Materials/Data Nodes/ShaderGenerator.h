#pragma once

#include "../MaterialData.h"
#include "GeometryShaderInfo.h"
#include "DataLine.h"

class Material;



//Generates GLSL code from DataNode DAGs.
class ShaderGenerator
{
public:



    //Generates a geometry shader, or an error message beginning with "ERROR:" if there was an error.
    //Uses the material data stored statically by the DataNode class.
    static std::string GenerateGeometryShader(void);

    //Generates the GLSL definitions for the given set of uniforms.
    static std::string GenerateUniformDeclarations(const UniformDictionary & uniforms);


    //Generates a vertex and fragment shader given data nodes.
    //Returns an error message, or an empty string if there was no error.
    //Uses the material data stored statically by the DataNode class.
    static std::string GenerateVertFragShaders(std::string & outVShader, std::string & outFShader,
                                               UniformDictionary & outUniforms, RenderingModes mode);


    //The return type for GenerateMaterial.
    //If the data nodes generated a shader successfully, the material will be a heap-allocated object and the error message will be empty.
    //Otherwise, the material will be 0 and the error message will be non-empty.
    struct GeneratedMaterial
    {
    public:
        Material * Mat;
        std::string ErrorMessage;
        GeneratedMaterial(Material * mat) : Mat(mat), ErrorMessage("") { }
        GeneratedMaterial(std::string errorMsg) : Mat(0), ErrorMessage(errorMsg) { }
    };
    //Generates the shaders and heap-allocates a new material from them.
    //You are responsible for the material's memory management after it's created.
    //Uses the material data stored statically by the DataNode class.
    static GeneratedMaterial GenerateMaterial(UniformDictionary & outUniforms, RenderingModes mode);
};