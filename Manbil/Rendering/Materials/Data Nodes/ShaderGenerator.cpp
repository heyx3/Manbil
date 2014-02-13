#include "ShaderGenerator.h"

#include "DataNodeIncludes.h"


typedef ShaderGenerator SG;
typedef RenderingChannels RC;


unsigned int SG::GetChannelInputSize(RC channel)
{
    switch (channel)
    {
        case RC::RC_Diffuse:
        case RC::RC_Normal:
        case RC::RC_ObjectVertexOffset:
        case RC::RC_WorldVertexOffset:
            return 3;

        case RC::RC_Distortion:
            return 2;

        case RC::RC_DiffuseIntensity:
        case RC::RC_Opacity:
        case RC::RC_Specular:
        case RC::RC_SpecularIntensity:
            return 1;

        default: assert(false);
    }
}

bool SG::IsChannelUsed(RC channel, RenderingModes mode, bool useLighting, const LightSettings & settings)
{
    switch (channel)
    {
        case RC::RC_Diffuse:
        case RC::RC_DiffuseIntensity:
        case RC::RC_ObjectVertexOffset:
        case RC::RC_WorldVertexOffset:
            return true;

        case RC::RC_Normal:
        case RC::RC_Specular:
        case RC::RC_SpecularIntensity:
            return useLighting;

        case RC::RC_Distortion:
        case RC::RC_Opacity:
            return IsModeTransparent(mode);

        default: assert(false);
    }
}

void SG::GetUsedChannels(RenderingModes mode, bool useLighting, const LightSettings & settings, std::vector<RenderingChannels> & outChannels)
{
    outChannels.insert(outChannels.end(), RC::RC_Diffuse);
    outChannels.insert(outChannels.end(), RC::RC_DiffuseIntensity);
    outChannels.insert(outChannels.end(), RC::RC_ObjectVertexOffset);
    outChannels.insert(outChannels.end(), RC::RC_WorldVertexOffset);

    if (useLighting)
    {
        outChannels.insert(outChannels.end(), RC::RC_Normal);
        outChannels.insert(outChannels.end(), RC::RC_Specular);
        outChannels.insert(outChannels.end(), RC::RC_SpecularIntensity);
    }

    if (IsModeTransparent(mode))
    {
        outChannels.insert(outChannels.end(), RC::RC_Distortion);
        outChannels.insert(outChannels.end(), RC::RC_Opacity);
    }
}

void SG::GenerateShaders(std::string & outVShader, std::string & outFShader,
                         RenderingModes mode, bool useLighting, const LightSettings & settings,
                         std::unordered_map<RenderingChannels, DataLine> channels)
{
    //First, make sure the channels are all correctly set up.
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        //Make sure each channel is actually used in the current rendering state.
        assert(IsChannelUsed(iterator->first, mode, useLighting, settings));

        //Make sure each channel has a valid input data line size.
        assert(GetChannelInputSize(iterator->first) == iterator->second.GetDataLineSize());
    }


    //If an expected channel input doesn't exist, create it and use a default value.
    std::vector<RC> validChannels;
    GetUsedChannels(mode, useLighting, settings, validChannels);
    for (int i = 0; i < validChannels.size(); ++i)
    {
        if (channels.find(validChannels[i]) == channels.end())
        {
            switch (validChannels[i])
            {
            case RC::RC_Diffuse:
                channels[RC::RC_Diffuse] = DataLine(DataNodePtr(new ObjectColorNode()), 0);
                break;
            case RC::RC_DiffuseIntensity:
                channels[RC::RC_DiffuseIntensity] = DataLine(Vector(1.0f));
                break;
            case RC::RC_Distortion:
                channels[RC::RC_Distortion] = DataLine(Vector2f(0.0f, 0.0f));
                break;
            case RC::RC_Normal:
                channels[RC::RC_Normal] = DataLine(DataNodePtr(new WorldNormalNode()));
                break;

                //TODO: Finish.

            default: assert(false);
            }
        }
    }
}