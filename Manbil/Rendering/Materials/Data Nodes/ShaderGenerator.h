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
    //Returns 0 is the given channel is a vertex output.
    static unsigned int GetChannelInputSize(RenderingChannels channel);
    //Gets all channels used in the given rendering mode/settings.
    static void GetUsedChannels(RenderingModes mode, std::vector<RenderingChannels> & outChannels);

    //Removes any unnecessary channels.
    static void RemoveUnusedChannels(std::unordered_map<RenderingChannels, DataLine> & channels, RenderingModes mode, bool useLighting, const LightSettings & settings);
    //Adds default inputs to any missing channels.
    static void AddMissingChannels(std::unordered_map<RenderingChannels, DataLine> & channels, RenderingModes mode, bool useLighting, const LightSettings & settings);

    //Generates a vertex and fragment shader given data nodes.
    static void GenerateShaders(std::string & outVShader, std::string & outFShader, UniformDictionary & outUniforms,
                                RenderingModes mode, bool useLighting, const LightSettings & settings,
                                std::unordered_map<RenderingChannels, DataLine> & channels);

    //Generates the shaders and allocates a new material from them.
    static Material * GenerateMaterial(std::unordered_map<RenderingChannels, DataLine> & channels,
                                       UniformDictionary & outUniforms,
                                       RenderingModes mode, bool useLighting, const LightSettings & settings);
};