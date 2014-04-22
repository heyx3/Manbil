#pragma once

#include "../Materials/Data Nodes/DataNodeIncludes.h"



//TODO: Try optionally using displacement mapping instead of actually moving the vertices, and see if there is a performance difference.
//TODO: Sample a "water floor" texture and for every water pixel cast a ray down to the ocean floor.
//TODO: Take several "seed maps" and use them for things like horiontalPos offset, base height offset, etc.
//TODO: Add support for seeded heightmap.
//TODO: Continue to tweak/improve these nodes.



//Calculates water properties.
//Outputs:
//0: The object-space vertex position.
//1: The object-space vertex normal.
//TODO: Move some of this stuff into .cpp file.
class WaterNode : public DataNode
{
public:

    static unsigned int GetVertexPosOutputIndex(void) { return 0; }
    static unsigned int GetSurfaceNormalOutputIndex(void) { return 1; }


    virtual std::string GetName(void) const override { return "waterNode"; }
    virtual std::string GetOutputName(unsigned int i) const override
    {
        Assert(i < 2, std::string() + "Invalid output index " + ToString(i));
        return GetName() + std::to_string(GetUniqueID()) + (i == 0 ? "_waterHeightOffset" : "waterNormal");
    }

    //Takes as input the vertex output channel for object-space fragment position.
    //Also takes in the uniform values for ripples/flows.
    WaterNode(const DataLine & vertexObjPosInput, const DataLine & fragmentObjPosInput,
              unsigned int _maxRipples = 0, unsigned int _maxFlows = 0)
        : DataNode(MakeVector(vertexObjPosInput, fragmentObjPosInput),
                   MakeVector(3, 3)),
          maxRipples(_maxRipples), maxFlows(_maxFlows)
    {
        Assert(vertexObjPosInput.GetDataLineSize() == 3,
               std::string() + "vertex shader object-space position input must have size 3; has size " + ToString(vertexObjPosInput.GetDataLineSize()));
        Assert(fragmentObjPosInput.GetDataLineSize() == 3,
               std::string() + "fragment shader object-space position input must have size 3; has size " + ToString(fragmentObjPosInput.GetDataLineSize()));
    }

protected:

    virtual bool UsesInput(unsigned int inputIndex) const override
    {
        switch (GetShaderType())
        {
            case Shaders::SH_Vertex_Shader:
                return &GetInputs()[inputIndex] == &GetObjectPosVInput();

            case Shaders::SH_Fragment_Shader:
                return &GetInputs()[inputIndex] == &GetObjectPosVOutput();

            default:
                Assert(false, std::string() + "Unknown shader type " + ToString(GetShaderType()));
                return DataNode::UsesInput(inputIndex);
        }
    }
    virtual bool UsesInput(unsigned int inputIndex, unsigned int outputIndex) const
    {
        switch (GetShaderType())
        {
            case Shaders::SH_Vertex_Shader:
                return (inputIndex == 0);

            case Shaders::SH_Fragment_Shader:
                return (inputIndex == 1);

            default:
                Assert(false, std::string() + "Unknown shader type " + ToString(GetShaderType()));
                return DataNode::UsesInput(inputIndex, outputIndex);
        }
    }

    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const override;
    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    unsigned int maxRipples, maxFlows;


    const DataLine & GetObjectPosVInput(void) const { return GetInputs()[0]; }
    const DataLine & GetObjectPosVOutput(void) const { return GetInputs()[1]; }
};



//A DataNode that outputs an offset for UV coordinates that creates an interesting water surface distortion effect.
//NOTE: This node only works in fragment shaders!
class WaterSurfaceDistortNode : public DataNode
{
public:

    //Gets a value for the "seedIn" argument for this node's constructor that works well with the Water object.
    //It uses a value that was stored in the vertex color, so it needs to know the vertex output channel that stores vertex color.
    static DataLine GetWaterSeedIn(RenderingChannels colorVertexOut)
    {
        assert(IsChannelVertexOutput(colorVertexOut, false));
        return DataLine(DataNodePtr(new VectorComponentsNode(DataLine(DataNodePtr(new VertexOutputNode(colorVertexOut, 4)), 0))), 2);
    }
    //Gets a value for the "timeValue" argument for this node's constructor that works well with the Water object.
    //It uses a value stored in the vertex color, so it needs toknow the vertex output channel that stores vertex color.
    static DataLine GetTimeIn(RenderingChannels colorVertexOut)
    {
        return DataLine(DataNodePtr(new AddNode(DataLine(DataNodePtr(new TimeNode()), 0),
                                                DataLine(DataNodePtr(new VectorComponentsNode(DataLine(DataNodePtr(new VertexOutputNode(colorVertexOut, 4)), 0))), 3))), 0);
    }


    unsigned int GetUVOffsetOutputIndex(void) const { return 0; }

    virtual std::string GetName(void) const override { return "waterSurfaceDistortNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return GetName() + std::to_string(GetUniqueID()) + "_uvOffset";
    }

    //Takes in a seed value. By default, uses an input that will work well for a Water object.
    //Also takes in the amplitude and period of the random shifting around of the surface.
    //Note that the water mesh puts random values into the vertex color, so it is useful
    //   for offsetting the elapsed time or seeding the distortion.
    WaterSurfaceDistortNode(DataLine seedIn = GetWaterSeedIn(RenderingChannels::RC_VERTEX_OUT_INVALID),
                            DataLine shiftAmplitude = DataLine(VectorF(0.01f)),
                            DataLine shiftPeriod = DataLine(VectorF(0.5f)),
                            DataLine timeValue = GetTimeIn(RenderingChannels::RC_VERTEX_OUT_INVALID))
        : DataNode(MakeVector(seedIn, shiftAmplitude, shiftPeriod, timeValue), MakeVector(2))
    {
        Assert(seedIn.GetDataLineSize() == 1, std::string() + "seedIn input needs size 1, has size " + ToString(seedIn.GetDataLineSize()));
        Assert(shiftAmplitude.GetDataLineSize() == 1, std::string() + "shiftAmplitude input needs size 1, has size " + ToString(shiftAmplitude.GetDataLineSize()));
        Assert(shiftPeriod.GetDataLineSize() == 1, std::string() + "shiftPeriod input needs size 1, has size " + ToString(shiftPeriod.GetDataLineSize()));
    }


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        Assert(GetShaderType() == Shaders::SH_Fragment_Shader,
               std::string() + "This node is only applicable in the fragment shader, but is being used in " + ToString(GetShaderType()));
        strOut += "\tvec2 " + GetOutputName(0) + " = vec2(" + GetSeedInput().GetValue() + " * " +
                                                              GetAmplitudeInput().GetValue() + " * " +
                                                              "sin(" + GetPeriodInput().GetValue() + " * " +
                                                                       GetTimeInput().GetValue() + "));\n";
    }

private:
    const DataLine & GetSeedInput(void) const { return GetInputs()[0]; }
    const DataLine & GetAmplitudeInput(void) const { return GetInputs()[1]; }
    const DataLine & GetPeriodInput(void) const { return GetInputs()[2]; }
    const DataLine & GetTimeInput(void) const { return GetInputs()[3]; }
};