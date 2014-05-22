#pragma once

#include "../Materials/Data Nodes/DataNodeIncludes.h"



//TODO: Try optionally using displacement mapping instead of actually moving the vertices, and see if there is a performance difference.
//TODO: Sample a "water floor" texture and for every water pixel cast a ray down to the ocean floor.
//TODO: Take several "seed maps" and use them for things like horiontalPos offset, base height offset, etc.
//TODO: Add support for seeded heightmap.
//TODO: Continue to tweak/parameterize these nodes.



//Calculates water properties.
//Outputs:
//0: The object-space vertex position.
//1: The object-space vertex normal.
class WaterNode : public DataNode
{
public:

    static unsigned int GetVertexPosOutputIndex(void) { return 0; }
    static unsigned int GetSurfaceNormalOutputIndex(void) { return 1; }


    virtual std::string GetName(void) const override { return "waterNode"; }
    virtual std::string GetOutputName(unsigned int i) const override;

    //Takes as input the input for object-space vertex/fragment position (one for each shader, Vertex and Fragment).
    //Also takes in the uniform values for ripples/flows.
    WaterNode(const DataLine & vertexObjPosInput, const DataLine & fragmentObjPosInput,
              unsigned int _maxRipples = 0, unsigned int _maxFlows = 0);

protected:

    virtual bool UsesInput(unsigned int inputIndex) const override;
    virtual bool UsesInput(unsigned int inputIndex, unsigned int outputIndex) const override;

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
    static DataLine GetWaterSeedIn(RenderingChannels colorVertexOut);
    //Gets a value for the "timeValue" argument for this node's constructor that works well with the Water object.
    //It uses a value stored in the vertex color, so it needs toknow the vertex output channel that stores vertex color.
    static DataLine GetTimeIn(RenderingChannels colorVertexOut);


    virtual std::string GetName(void) const override { return "waterSurfaceDistortNode"; }
    virtual std::string GetOutputName(unsigned int index) const override;


    //Takes in a seed value. By default, uses an input that will work well for a Water object.
    //Also takes in the amplitude and period of the random shifting around of the surface.
    //Note that the water mesh puts random values into the vertex color, so it is useful
    //   for offsetting the elapsed time or seeding the distortion.
    WaterSurfaceDistortNode(DataLine seedIn = GetWaterSeedIn(RenderingChannels::RC_VERTEX_OUT_INVALID),
                            DataLine shiftAmplitude = DataLine(VectorF(0.01f)),
                            DataLine shiftPeriod = DataLine(VectorF(0.5f)),
                            DataLine timeValue = GetTimeIn(RenderingChannels::RC_VERTEX_OUT_INVALID));


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override;

private:
    const DataLine & GetSeedInput(void) const { return GetInputs()[0]; }
    const DataLine & GetAmplitudeInput(void) const { return GetInputs()[1]; }
    const DataLine & GetPeriodInput(void) const { return GetInputs()[2]; }
    const DataLine & GetTimeInput(void) const { return GetInputs()[3]; }
};