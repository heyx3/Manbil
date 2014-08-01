#include "RemapNode.h"



unsigned int RemapNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + index);
    return BasicMath::Max(GetInputs()[0].GetSize(),
                          BasicMath::Max(GetInputs()[1].GetSize(),
                          BasicMath::Max(GetInputs()[2].GetSize(),
                          BasicMath::Max(GetInputs()[3].GetSize(),
                          GetInputs()[4].GetSize()))));
}

RemapNode::RemapNode(const DataLine & toRemap, const DataLine & srcMin, const DataLine & srcMax,
                     DataLine destMin, DataLine destMax, std::string name)
    : DataNode(MakeVector(toRemap, srcMin, srcMax, destMin, destMax),
               [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new RemapNode(ins[0], ins[1], ins[2], ins[3], ins[4], _name)); },
               name)
{
    unsigned int size = GetOutputSize(0);

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
    outOutputs += "\t" + VectorF(GetOutputSize(0)).GetGLSLType() + " " + GetOutputName(0) + " = " +
        dMi + " + ((" + val + " - " + sMi + ") * (" + dMa + " - " + dMi + ") /\n\t\t(" + sMa + " - " + sMi + "));\n";
}

std::string RemapNode::GetInputDescription(unsigned int index) const
{
    switch (index)
    {
        case 0: return "Value To Remap";
        case 1: return "Source Min";
        case 2: return "Source Max";
        case 3: return "Destination Min";
        case 4: return "Destination Max";

        default:
            Assert(false, "Unexpected input index " + ToString(index));
            return "BAD_INPUT_INDEX_" + ToString(index);
    }
}

std::vector<DataLine> RemapNode::MakeVector(const DataLine & val, const DataLine & srcMin, const DataLine & srcMax, const DataLine & destMin, const DataLine & destMax)
{
    std::vector<DataLine> dls;
    dls.insert(dls.end(), val);
    dls.insert(dls.end(), srcMin);
    dls.insert(dls.end(), srcMax);
    dls.insert(dls.end(), destMin);
    dls.insert(dls.end(), destMax);
}