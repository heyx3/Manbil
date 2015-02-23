#pragma once

#include "../DataNode.h"


//Interpolates between a min input and a max output.
//Has different levels of smoothness; "Very Smooth" has worse performance than "Linear" and "Smooth".
//If using "Smooth", the interpolation must always be from a smaller value to a larger value.
class InterpolateNode : public DataNode
{
public:

    enum InterpolationType
    {
        IT_Linear,
        IT_Smooth,
        IT_VerySmooth,
    };


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

    virtual void WriteExtraData(DataWriter* writer) const override;
    virtual void ReadExtraData(DataReader* reader) override;

    virtual void AssertMyInputsValid(void) const override;


private:

    InterpolationType intType;

    ADD_NODE_REFLECTION_DATA_H(InterpolateNode)
};