#pragma once

#include "../DataNode.h"



//Remapts an input from some range to some other range.
class RemapNode : public DataNode
{
public:

    RemapNode(const DataLine & toRemap, const DataLine & srcMin, const DataLine & srcMax,
              DataLine destMin = DataLine(VectorF(0.0f)), DataLine destMax = DataLine(VectorF(1.0f)),
              std::string name = "");


protected:

    virtual void ResetOutputs(std::vector<unsigned int> & newOuts) const override;
    virtual void WriteMyOutputs(std::string & outOutputs) const override;


private:

    typedef const DataLine & DataLineR;

    std::vector<DataLine> MakeVector(DataLineR value, DataLineR srcMin, DataLineR srcMax, DataLineR destMin, DataLineR destMax);

    const DataLine & GetValueInput(void) const { return GetInputs()[0]; }
    const DataLine & GetSrcMinInput(void) const { return GetInputs()[1]; }
    const DataLine & GetSrcMaxInput(void) const { return GetInputs()[2]; }
    const DataLine & GetDestMinInput(void) const { return GetInputs()[3]; }
    const DataLine & GetDestMaxInput(void) const { return GetInputs()[4]; }
};