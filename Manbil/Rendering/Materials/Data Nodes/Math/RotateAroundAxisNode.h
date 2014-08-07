#pragma once

#include "../DataNode.h"


//Rotates a vector around an axis.
class RotateAroundAxisNode : public DataNode
{
public:

#pragma warning(disable: 4100)
    virtual unsigned int GetOutputSize(unsigned int index) const override { return 3; }
    virtual std::string GetOutputName(unsigned int index) const override { return GetName() + "_rotated"; }
#pragma warning(default: 4100)

    RotateAroundAxisNode(const DataLine & toRotate, const DataLine & rotateAxis, const DataLine & rotateAmount, std::string name = "");


protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

private:

    const DataLine & GetToRotateInput(void) const { return GetInputs()[0]; }
    const DataLine & GetRotateAxisInput(void) const { return GetInputs()[1]; }
    const DataLine & GetRotateAmountInput(void) const { return GetInputs()[2]; }

    virtual void AssertMyInputsValid(void) const override;


    MAKE_NODE_READABLE_H(RotateAroundAxisNode)
};