#pragma once

#include "DataNode.h"
#include "../MaterialData.h"


//TOOD: Remove all unused vertex shader outputs.
//TODO: Allow nodes to define custom vertex shader outputs.

class Material;

//Generates GLSL code from DataNode DAGs.
class ShaderGenerator
{
public:

    //Gets the size of the data line input into the given rendering channel.
    static unsigned int GetChannelInputSize(RenderingChannels channel);
    //Gets whether or not the given channel is used, given the rendering mode/settings.
    static bool IsChannelUsed(RenderingChannels channel, RenderingModes mode, bool useLighting, const LightSettings & settings);
    //Gets all channels used in the given rendering mode/settings.
    static void GetUsedChannels(RenderingModes mode, bool useLighting, const LightSettings & settings, std::vector<RenderingChannels> & outChannels);

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