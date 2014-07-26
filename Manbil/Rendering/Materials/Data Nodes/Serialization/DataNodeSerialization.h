#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"
#include "../../../../IO/XmlSerialization.h"


class DataNodeNames
{
public:

    static const std::string Name_Add,
                             Name_Subtract,
                             Name_Divide,
                             Name_Multiply,
                             Name_Combine,
                             Name_Log,
                             Name_Sign,
                             Name_Ceil,
                             Name_Floor,
                             Name_Abs,
                             Name_Sin,
                             Name_Cos,
                             Name_ASin,
                             Name_ACos,
                             Name_Normalize,
                             Name_Fract,
                             Name_OneMinus,
                             Name_Negative,
                             Name_ObjectPosToScreen,
                             Name_ObjectNormalToScreen,
                             Name_ObjectPosToWorld,
                             Name_ObjectNormalToWorld,
                             Name_WorldPosToScreen,
                             Name_WorldNormalToScreen,
                             Name_Clamp,
                             Name_Cross,
                             Name_Dot,
                             Name_Distance,
                             Name_GetLerpComponent,
                             Name_Lerp,
                             Name_SmoothLerp,
                             Name_SuperSmoothLerp,
                             Name_Max,
                             Name_Min,
                             Name_Modulo,
                             Name_Pow,
                             Name_Reflect,
                             Name_Refract,
                             Name_RotateAroundAxis,
                             Name_Remap,
                             Name_SplitComponents,
                             Name_SurfaceLightCalc,
                             Name_WhiteNoise,
                             Name_CustomExpression,
                             Name_Parameter,
                             Name_Texture2D,
                             Name_Texture3D,
                             Name_TextureCubemap,
                             Name_Swizzle;
};

//Represents a material comprised of vertex and fragment shaders.
//PRIORITY: Add geometry shader support.
class DataNodeSerialization : public ISerializable
{
public:

    static std::string ToString(RenderingChannels channel);
    static RenderingChannels ToChannel(std::string channel);


    std::unordered_map<RenderingChannels, DataLine> Channels;
    std::unordered_map<DataNode*, std::string> NodeNames;
    std::unordered_map<DataNode*, std::string> NodeTypeNames;


    virtual bool ReadData(DataReader * data, std::string & outError) override;
    virtual bool WriteData(DataWriter * data, std::string & outError) const override;
};