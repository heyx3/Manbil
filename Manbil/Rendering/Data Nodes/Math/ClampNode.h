#pragma once

#include "../DataNode.h"


//Keeps a value inside a max and a min.
class ClampNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    ClampNode(const DataLine & min, const DataLine & max, const DataLine & value, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void AssertMyInputsValid(void) const override;


private:

    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetValueInput(void) const { return GetInputs()[2]; }

    ADD_NODE_REFLECTION_DATA_H(ClampNode)
};