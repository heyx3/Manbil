#pragma once

#include "../DataNode.h"


//Remapts an input from some range to some other range.
class RemapNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "remapNode"; }

    RemapNode(const DataLine & toRemap, const DataLine & srcMin, const DataLine & srcMax,
              DataLine destMin = DataLine(VectorF(0.0f)), DataLine destMax = DataLine(VectorF(1.0f)))
              : DataNode(MakeVector(toRemap, srcMin, srcMax, destMin, destMax), DataNode::MakeVector(GetSize(toRemap, srcMin, srcMax, destMin, destMax)))
    {
        unsigned int size = GetOutputs()[0];

        Assert(toRemap.GetDataLineSize() == 1 || toRemap.GetDataLineSize() == size,
               "'toRemap' input value isn't size 1 or " + std::to_string(size) + "!");
        Assert(srcMin.GetDataLineSize() == 1 || srcMin.GetDataLineSize() == size,
               "'srcMin' input value isn't size 1 or " + std::to_string(size) + "!");
        Assert(srcMax.GetDataLineSize() == 1 || srcMax.GetDataLineSize() == size,
               "'srcMax' input value isn't size 1 or " + std::to_string(size) + "!");
        Assert(destMin.GetDataLineSize() == 1 || destMin.GetDataLineSize() == size,
               "'destMin' input value isn't size 1 or " + std::to_string(size) + "!");
        Assert(destMax.GetDataLineSize() == 1 || destMax.GetDataLineSize() == size,
               "'destMax' input value isn't size 1 or " + std::to_string(size) + "!");
    }

#pragma warning(disable: 4100)
    virtual std::string GetOutputName(unsigned int output) const override { assert(output == 0); return GetName() + std::to_string(GetUniqueID()) + "_remapped"; }
#pragma warning(default: 4100)

protected:

    virtual void WriteMyOutputs(std::string & outOutputs) const override
    {
        std::string val = GetValueInput().GetValue(),
                    sMi = GetSrcMinInput().GetValue(),
                    sMa = GetSrcMaxInput().GetValue(),
                    dMi = GetDestMinInput().GetValue(),
                    dMa = GetDestMaxInput().GetValue();
        outOutputs += "\t" + VectorF(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) + " = " +
                        dMi + " + ((" + val + " - " + sMi + ") * (" + dMa + " - " + dMi + ") /\n\t\t(" + sMa + " - " + sMi + "));\n";
    }


private:

    typedef const DataLine & DataLineR;

    std::vector<DataLine> MakeVector(DataLineR value, DataLineR srcMin, DataLineR srcMax, DataLineR destMin, DataLineR destMax)
    {
        std::vector<DataLine> dls;

        dls.insert(dls.end(), value);
        dls.insert(dls.end(), srcMin);
        dls.insert(dls.end(), srcMax);
        dls.insert(dls.end(), destMin);
        dls.insert(dls.end(), destMax);

        return dls;
    }
    unsigned int GetSize(DataLineR value, DataLineR srcMin, DataLineR srcMax, DataLineR destMin, DataLineR destMax)
    {
        return BasicMath::Max(value.GetDataLineSize(),
                              BasicMath::Max(srcMin.GetDataLineSize(),
                                             BasicMath::Max(srcMax.GetDataLineSize(),
                                                            BasicMath::Max(destMin.GetDataLineSize(),
                                                                           destMax.GetDataLineSize()))));
    }

    const DataLine & GetValueInput(void) const { return GetInputs()[0]; }
    const DataLine & GetSrcMinInput(void) const { return GetInputs()[1]; }
    const DataLine & GetSrcMaxInput(void) const { return GetInputs()[2]; }
    const DataLine & GetDestMinInput(void) const { return GetInputs()[3]; }
    const DataLine & GetDestMaxInput(void) const { return GetInputs()[4]; }
};