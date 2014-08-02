#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"


//Represents the value of vertex shader outputs into the geometry shader.
//NOTE: This node is only usable in the geometry shader!
//This class is a singleton.
//Each output index corresponds to that vertex output.
class GeometryInputNode : public DataNode
{
public:

    static DataNodePtr GetInstance(void) { return instance; }


    virtual std::string GetTypeName(void) const override { return "geometryInputs"; }

    virtual unsigned int GetNumbOutputs(void) const override;

    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override;
    virtual std::string GetOutputName(unsigned int outputIndex) const override;


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    static DataNodePtr instance;


    void AssertGeoShader(void) const;

    GeometryInputNode(void);
};