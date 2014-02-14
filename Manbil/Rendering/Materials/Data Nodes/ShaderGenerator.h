#pragma once

#include "DataNode.h"
#include "../MaterialData.h"


//Represents the node data needed for a shader generator


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

    //Generates a vertex and fragment shader given data nodes.
    static void GenerateShaders(std::string & outVShader, std::string & outFShader, UniformDictionary & outUniforms,
                                RenderingModes mode, bool useLighting, const LightSettings & settings,
                                std::unordered_map<RenderingChannels, DataLine> channels);
};