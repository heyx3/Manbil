#pragma once

#include "../DataNode.h"



//Remapts an input from some range to some other range.
class RemapNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "remap"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_remapped";
    }

    RemapNode(const DataLine & toRemap, const DataLine & srcMin, const DataLine & srcMax,
              DataLine destMin = DataLine(VectorF(0.0f)), DataLine destMax = DataLine(VectorF(1.0f)),
              std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outOutputs) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;


private:

    std::vector<DataLine> MakeVector(const DataLine & value, const DataLine & srcMin, const DataLine & srcMax,
                                     const DataLine & destMin, const DataLine & destMax);

    const DataLine & GetValueInput(void) const { return GetInputs()[0]; }
    const DataLine & GetSrcMinInput(void) const { return GetInputs()[1]; }
    const DataLine & GetSrcMaxInput(void) const { return GetInputs()[2]; }
    const DataLine & GetDestMinInput(void) const { return GetInputs()[3]; }
    const DataLine & GetDestMaxInput(void) const { return GetInputs()[4]; }
};