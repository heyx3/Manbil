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
        IT_Power,
    };

    virtual std::string GetName(void) const override { return "interpolationNode"; }


    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetInterpInput(void) const { return GetInputs()[2]; }

    //Only valid if the interpolation type is "IT_Power".
    const DataLine & GetPowerInput(void) const { return GetInputs()[3]; }


    InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type);
    InterpolateNode(DataLine min, DataLine max, DataLine interp, DataLine power);

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outFuncs, Shaders shaderType) const override;

    
protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;


private:

    InterpolationType intType;
    float it_power;
};