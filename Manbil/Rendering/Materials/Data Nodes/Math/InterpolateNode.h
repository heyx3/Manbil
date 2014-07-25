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

    virtual std::string GetName(void) const override { return "interpolationNode"; }


    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetInterpInput(void) const { return GetInputs()[2]; }


    InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type);

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outFuncs) const override;

    
protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    InterpolationType intType;
};