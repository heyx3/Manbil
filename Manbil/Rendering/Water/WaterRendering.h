#pragma once

#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Materials/Data Nodes/SerializedMaterial.h"



//TODO: Try optionally using displacement mapping instead of actually moving the vertices, and see if there is a performance difference.



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