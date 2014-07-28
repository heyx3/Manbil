#pragma once

#include "../DataNode.h"


//Rotates a vector around an axis.
class RotateAroundAxisNode : public DataNode
{
public:

    RotateAroundAxisNode(const DataLine & toRotate, const DataLine & rotateAxis, const DataLine & rotateAmount, std::string name = "");

protected:

    virtual void ResetOutputs(std::vector<unsigned int> & outOuts) const override
    {
        outOuts.insert(outOuts.end(), 3);
    }

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    const DataLine & GetToRotateInput(void) const { return GetInputs()[0]; }
    const DataLine & GetRotateAxisInput(void) const { return GetInputs()[1]; }
    const DataLine & GetRotateAmountInput(void) const { return GetInputs()[2]; }
};