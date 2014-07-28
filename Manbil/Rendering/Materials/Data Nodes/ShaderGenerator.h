#pragma once

#include "../MaterialData.h"
#include "GeometryShaderInfo.h"
#include "DataLine.h"

class Material;


//An output of a shader.
struct ShaderOutput
{
public:
    std::string Name;
    DataLine Value;
    ShaderOutput(std::string name = "", DataLine value = DataLine()) : Name(name), Value(value) { }
};

//The set of outputs that together describes a vertex, and fragment shader.
struct MaterialOutputs
{
public:

    //Should be size 4. Unless the material has a geometry shader,
    //   this DataLine should output NDC coordinates (i.e. screen-space coordinates before the Z divide).
    DataLine VertexPosOutput;

    //The outputs for the vertex/fragment shaders. Fragment outputs should all be size 4.
    std::vector<ShaderOutput> VertexOutputs, FragmentOutputs;
};

//Generates GLSL code from DataNode DAGs.
class ShaderGenerator
{
public:



    //Generates a geometry shader with the given information, or an error message beginning with "ERROR:" if there was an error.
    static std::string GenerateGeometryShader(const MaterialOutputs & vertexOuts, const GeoShaderData & data);

    //Generates the GLSL definitions for the given set of uniforms.
    static std::string GenerateUniformDeclarations(const UniformDictionary & uniforms);


    //Generates a vertex and fragment shader given data nodes.
    //Returns an error message, or an empty string if there was no error.
    static std::string GenerateVertFragShaders(std::string & outVShader, std::string & outFShader, UniformDictionary & outUniforms,
                                               RenderingModes mode, const ShaderInOutAttributes & vertexInputs, const MaterialOutputs & materialData,
                                               GeoShaderData geoShaderData = GeoShaderData());


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
    //TODO: Instead of using a bare pointer for the material, use a shared_ptr. Remove the second line from this function summary.
    static GeneratedMaterial GenerateMaterial(const MaterialOutputs & materialData, UniformDictionary & outUniforms,
                                              const ShaderInOutAttributes & vertexIns, GeoShaderData geometryShader = GeoShaderData());
};