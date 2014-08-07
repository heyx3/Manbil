#pragma once

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Materials/Data Nodes/SerializedMaterial.h"



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


    virtual unsigned int GetNumbOutputs(void) const override { return 2; }

    virtual unsigned int GetOutputSize(unsigned int i) const override;
    virtual std::string GetOutputName(unsigned int i) const override;

    //Takes as input the input for object-space vertex/fragment position (one for each shader, Vertex and Fragment).
    //Also takes in the uniform values for ripples/flows.
    WaterNode(const DataLine & objPos_VertexShader, const DataLine & objPos_FragmentShader, std::string name = "",
              unsigned int _maxRipples = 0, unsigned int _maxFlows = 0);

protected:

    virtual bool UsesInput(unsigned int inputIndex) const override;
    virtual bool UsesInput(unsigned int inputIndex, unsigned int outputIndex) const override;

    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const override;
    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;

    virtual void AssertMyInputsValid(void) const override;


private:

    unsigned int maxRipples, maxFlows;

    const DataLine & GetObjectPosVInput(void) const { return GetInputs()[0]; }
    const DataLine & GetObjectPosVOutput(void) const { return GetInputs()[1]; }

    MAKE_NODE_READABLE_H(WaterNode);
};

/*
//A DataNode that outputs an offset for UV coordinates that creates an interesting water surface distortion effect.
//NOTE: This node only works in fragment shaders!
class WaterSurfaceDistortNode : public DataNode
{
public:

    //Gets a value for the "seedIn" argument for this node's constructor that works well with the Water object.
    //It uses a value that was stored in the WaterVertex::RandSeeds input, so it needs to know the fragment input that has it.
    static DataLine GetWaterSeedIn(const ShaderInOutAttributes & fragmentInputs, int randSeedInputIndex);
    //Gets a value for the "timeValue" argument for this node's constructor that works well with the Water object.
    //It uses a value stored in the WaterVertex::RandSeeds input, so it needs to know the fragment input that has it.
    static DataLine GetTimeIn(const ShaderInOutAttributes & fragmentInputs, int randSeedInputIndex);


    virtual std::string GetTypeName(void) const override { return "waterSurfaceDistortNode"; }
    virtual std::string GetOutputName(unsigned int index) const override;


    //Takes in a seed value. By default, uses an input that will work well for a Water object.
    //Also takes in the amplitude and period of the random shifting around of the surface.
    //Note that the water mesh puts random values into the vertex color, so it is useful
    //   for offsetting the elapsed time or seeding the distortion.
    WaterSurfaceDistortNode(DataLine seedIn = GetWaterSeedIn(ShaderInOutAttributes(), -1),
                            DataLine shiftAmplitude = DataLine(VectorF(0.01f)),
                            DataLine shiftPeriod = DataLine(VectorF(0.5f)),
                            DataLine timeValue = GetTimeIn(ShaderInOutAttributes(), -1));


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override;

private:
    const DataLine & GetSeedInput(void) const { return GetInputs()[0]; }
    const DataLine & GetAmplitudeInput(void) const { return GetInputs()[1]; }
    const DataLine & GetPeriodInput(void) const { return GetInputs()[2]; }
    const DataLine & GetTimeInput(void) const { return GetInputs()[3]; }
};

Ptr GenerateGoodWaterSurfaceDistortion(SerializedMaterial & matNodes)

*/