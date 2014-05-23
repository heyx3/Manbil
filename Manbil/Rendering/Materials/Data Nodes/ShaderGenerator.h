#pragma once

#include "DataNode.h"
#include "../MaterialData.h"

class Material;


//Generates GLSL code from DataNode DAGs.
class ShaderGenerator
{
public:

    //Gets which shader the given channel input is calculated in.
    static DataNode::Shaders GetShaderType(RenderingChannels channel);

    //Gets the size of the data line input into the given rendering channel.
    //Returns 0 is the given channel is a vertex or color output.
    static unsigned int GetChannelInputSize(RenderingChannels channel);

    //Adds default inputs to any missing channels.
    static void AddMissingChannels(std::unordered_map<RenderingChannels, DataLine> & channels, RenderingModes mode, bool useLighting, const LightSettings & settings);

    //Generates a vertex and fragment shader given data nodes.
    //Returns an error message, or an empty string if there was no error.
    static std::string GenerateShaders(std::string & outVShader, std::string & outFShader, UniformDictionary & outUniforms,
                                       RenderingModes mode, bool useLighting, const LightSettings & settings, const VertexAttributes & attribs,
                                       std::unordered_map<RenderingChannels, DataLine> & channels);


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
    static GeneratedMaterial GenerateMaterial(std::unordered_map<RenderingChannels, DataLine> & channels,
                                              UniformDictionary & outUniforms, const VertexAttributes & attribs,
                                              RenderingModes mode, bool useLighting, const LightSettings & settings);
};