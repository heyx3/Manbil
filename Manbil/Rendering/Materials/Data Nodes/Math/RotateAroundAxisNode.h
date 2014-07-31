#pragma once

#include "../DataNode.h"


//Rotates a vector around an axis.
class RotateAroundAxisNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "rotateAroundAxis"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + index);
        return 3;
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + index);
        return GetName() + "_rotated";
    }

    RotateAroundAxisNode(const DataLine & toRotate, const DataLine & rotateAxis, const DataLine & rotateAmount, std::string name = "");


protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

private:

    const DataLine & GetToRotateInput(void) const { return GetInputs()[0]; }
    const DataLine & GetRotateAxisInput(void) const { return GetInputs()[1]; }
    const DataLine & GetRotateAmountInput(void) const { return GetInputs()[2]; }
};