#pragma once

#include "../DataNode.h"

class InterpolateNode : public DataNode
{
public:

    enum InterpolationType
    {
        IT_Linear,
        IT_Smooth,
        IT_VerySmooth,
    };


    virtual std::string GetTypeName(void) const override { return "interpolation"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetInterpInput(void) const { return GetInputs()[2]; }

    InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type, std::string name = "");

    
protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outFuncs) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;


private:

    InterpolationType intType;
};