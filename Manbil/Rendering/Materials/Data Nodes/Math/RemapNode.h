#pragma once

#include "../DataNode.h"



//Remapts an input from some range to some other range.
class RemapNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
#pragma warning(disable: 4100)
    virtual std::string GetOutputName(unsigned int index) const override { return GetName() + "_remapped"; }
#pragma warning(default: 4100)

    RemapNode(const DataLine & toRemap, const DataLine & srcMin, const DataLine & srcMax,
              DataLine destMin = DataLine(VectorF(0.0f)), DataLine destMax = DataLine(VectorF(1.0f)),
              std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outOutputs) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void AssertMyInputsValid(void) const override;


private:

    std::vector<DataLine> MakeVector(const DataLine & value, const DataLine & srcMin, const DataLine & srcMax,
                                     const DataLine & destMin, const DataLine & destMax);

    const DataLine & GetValueInput(void) const { return GetInputs()[0]; }
    const DataLine & GetSrcMinInput(void) const { return GetInputs()[1]; }
    const DataLine & GetSrcMaxInput(void) const { return GetInputs()[2]; }
    const DataLine & GetDestMinInput(void) const { return GetInputs()[3]; }
    const DataLine & GetDestMaxInput(void) const { return GetInputs()[4]; }


    ADD_NODE_REFLECTION_DATA_H(RemapNode)
};