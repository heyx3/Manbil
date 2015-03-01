#include "DataLine.h"

#include "DataNode.h"
#include "../../IO/Serialization.h"



DataLine::DataLine(std::shared_ptr<DataNode> nodePtr, unsigned int outputLineIndex)
    : DataLine(nodePtr->GetName(), outputLineIndex) { }

unsigned int DataLine::GetSize(void) const
{
    if (isConstantValue)
    {
        return constantValue.GetSize();
    }
    else
    {
        DataNode* found = GetNode();
        if (found == 0)
        {
            return 0;
        }
        return found->GetOutputSize(nonConstantOutputIndex);
    }
}

std::string DataLine::GetValue(void) const
{
    if (isConstantValue)
    {
        std::string ret = "";
        if (constantValue.GetSize() > 1)
        {
            ret += constantValue.GetGLSLType() + "(";
        }

        for (unsigned int i = 0; i < constantValue.GetSize(); ++i)
        {
            if (i > 0)
            {
                ret += ", ";
            }
            ret += std::to_string(constantValue.GetValue()[i]);
        }

        if (constantValue.GetSize() > 1)
        {
            ret += ")";
        }

        return ret;
    }
    else
    {
        DataNode* found = GetNode();
        if (found == 0)
        {
            return "ERROR_CouldntFindNodeNamed_" + nonConstantValue;
        }
        return found->GetOutputName(nonConstantOutputIndex);
    }
}

DataNode* DataLine::GetNode(void) const
{
    assert(!isConstantValue);
    return DataNode::GetNode(nonConstantValue);
}
std::string DataLine::GetNonConstantValue(void) const
{
    assert(!isConstantValue);
    return nonConstantValue;
}
unsigned int DataLine::GetNonConstantOutputIndex(void) const
{
    assert(!isConstantValue);
    return nonConstantOutputIndex;
}

void DataLine::WriteData(DataWriter* writer) const
{
    writer->WriteBool(isConstantValue, "Is a constant VectorF value?");

    if (isConstantValue)
    {
        writer->WriteDataStructure(VectorF_Writable(constantValue), "Value");
    }
    else
    {
        writer->WriteString(nonConstantValue, "nodeName");
        writer->WriteUInt(nonConstantOutputIndex, "Output index");
    }
}
void DataLine::ReadData(DataReader* reader)
{
    reader->ReadBool(isConstantValue);

    if (isConstantValue)
    {
        reader->ReadDataStructure(VectorF_Readable(constantValue));
    }
    else
    {
        reader->ReadString(nonConstantValue);
        if (DataNode::GetNode(nonConstantValue) == 0)
        {
            reader->ErrorMessage = "Node named '" + nonConstantValue + "' doesn't exist";
            throw DataReader::EXCEPTION_FAILURE;
        }

        reader->ReadUInt(nonConstantOutputIndex);
    }
}