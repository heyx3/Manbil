#include "RemapNode.h"

RemapNode::RemapNode(const DataLine & toRemap, const DataLine & srcMin, const DataLine & srcMax,
                     DataLine destMin, DataLine destMax, std::string name)
    : DataNode(MakeVector(toRemap, srcMin, srcMax, destMin, destMax), name)
{
    unsigned int size = GetOutputs()[0];

    Assert(toRemap.GetSize() == 1 || toRemap.GetSize() == size,
           "'toRemap' input value isn't size 1 or " + std::to_string(size) + "!");
    Assert(srcMin.GetSize() == 1 || srcMin.GetSize() == size,
           "'srcMin' input value isn't size 1 or " + std::to_string(size) + "!");
    Assert(srcMax.GetSize() == 1 || srcMax.GetSize() == size,
           "'srcMax' input value isn't size 1 or " + std::to_string(size) + "!");
    Assert(destMin.GetSize() == 1 || destMin.GetSize() == size,
           "'destMin' input value isn't size 1 or " + std::to_string(size) + "!");
    Assert(destMax.GetSize() == 1 || destMax.GetSize() == size,
           "'destMax' input value isn't size 1 or " + std::to_string(size) + "!");
}

void RemapNode::WriteMyOutputs(std::string & outOutputs) const
{
    std::string val = GetValueInput().GetValue(),
                sMi = GetSrcMinInput().GetValue(),
                sMa = GetSrcMaxInput().GetValue(),
                dMi = GetDestMinInput().GetValue(),
                dMa = GetDestMaxInput().GetValue();
    outOutputs += "\t" + VectorF(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) + " = " +
        dMi + " + ((" + val + " - " + sMi + ") * (" + dMa + " - " + dMi + ") /\n\t\t(" + sMa + " - " + sMi + "));\n";
}

void RemapNode::ResetOutputs(std::vector<unsigned int> & newOuts) const
{
    newOuts.insert(newOuts.end(), BasicMath::Max(GetInputs()[0].GetSize(),
                                            BasicMath::Max(GetInputs()[1].GetSize(),
                                                      BasicMath::Max(GetInputs()[2].GetSize(),
                                                                BasicMath::Max(GetInputs()[3].GetSize(),
                                                                          GetInputs()[4].GetSize())))));
}

std::vector<DataLine> RemapNode::MakeVector(DataLineR val, DataLineR srcMin, DataLineR srcMax, DataLineR destMin, DataLineR destMax)
{
    std::vector<DataLine> dls;
    dls.insert(dls.end(), val);
    dls.insert(dls.end(), srcMin);
    dls.insert(dls.end(), srcMax);
    dls.insert(dls.end(), destMin);
    dls.insert(dls.end(), destMax);
}