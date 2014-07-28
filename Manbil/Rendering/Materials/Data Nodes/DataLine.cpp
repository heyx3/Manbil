#include "DataLine.h"

#include "DataNode.h"
#include "../../../DebugAssist.h"


unsigned int DataLine::GetSize(void) const
{
    if (isConstantValue)
    {
        return constantValue.GetSize();
    }
    else
    {
        DataNode * found = GetNode();
        if (found == 0) return 0;
        return found->GetOutputs()[nonConstantOutputIndex];
    }
}

std::string DataLine::GetValue(void) const
{
    if (isConstantValue)
    {
        std::string ret = "";
        if (constantValue.GetSize() > 1)
            ret += constantValue.GetGLSLType() + "(";

        for (unsigned int i = 0; i < constantValue.GetSize(); ++i)
        {
            if (i > 0) ret += ", ";
            ret += std::to_string(constantValue.GetValue()[i]);
        }

        if (constantValue.GetSize() > 1)
            ret += ")";

        return ret;
    }
    else
    {
        DataNode* found = GetNode();
        if (found == 0) return "ERROR_CouldntFindNodeNamed_" + nonConstantValue;
        return found->GetOutputName(nonConstantOutputIndex);
    }
}

DataNode* DataLine::GetNode(void) const
{
    assert(!isConstantValue);
    return DataNode::GetNode(nonConstantValue);
}

bool DataLine::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteBool(IsConstant(), "isConstant", outError))
    {
        outError = "Error writing whether this data line is constant " + std::string() + 
                      "(is is" + (IsConstant() ? "" : "n't") + "): " + outError;
        return false;
    }

    if (IsConstant())
    {
        if (!writer->WriteDataStructure(constantValue, "value", outError))
        {
            outError = "Error writing constant value of '" + DebugAssist::ToString(constantValue) + "': " + outError;
            return false;
        }
    }
    else
    {
        if (!writer->WriteString(nonConstantValue, "nodeName", outError))
        {
            outError = "Error writing node's name '" + nonConstantValue + "': " + outError;
            return false;
        }
        if (!writer->WriteUInt(nonConstantOutputIndex, "nodeOutput", outError))
        {
            outError = "Error writing node's output index " + std::to_string(nonConstantOutputIndex) + ": " + outError;
            return false;
        }
    }

    return true;
}
bool DataLine::ReadData(DataReader * reader, std::string & outError)
{
    MaybeValue<bool> tryConstant = reader->ReadBool(outError);
    if (!tryConstant.HasValue())
    {
        outError = "Error reading whether this data line is constant: " + outError;
        return false;
    }
    isConstantValue = tryConstant.GetValue();

    if (IsConstant())
    {
        if (!reader->ReadDataStructure(constantValue, outError))
        {
            outError = "Error reading constant data line value: " + outError;
            return false;
        }
    }
    else
    {
        MaybeValue<std::string> nodeName = reader->ReadString(outError);
        if (!nodeName.HasValue())
        {
            outError = "Error reading name of data line's node value: " + outError;
            return false;
        }
        if (DataNode::GetNode(nodeName.GetValue()) == 0)
        {
            outError = "The node named '" + nodeName.GetValue() + "' doesn't exist!";
            return false;
        }
        nonConstantValue = nodeName.GetValue();

        MaybeValue<unsigned int> tryIndex = reader->ReadUInt(outError);
        if (!tryIndex.HasValue())
        {
            outError = "Error reading output index for data line's input node '" + nodeName.GetValue() + "': " + outError;
            return false;
        }
        nonConstantOutputIndex = tryIndex.GetValue();
    }

    return true;
}