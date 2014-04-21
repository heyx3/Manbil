#include "DataLine.h"

#include "DataNode.h"


unsigned int DataLine::GetDataLineSize(void) const
{
    return (isConstantValue ? constantValue.GetSize() :
                              nonConstantValue->GetOutputs()[nonConstantValueIndex]);
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
        return nonConstantValue->GetOutputName(nonConstantValueIndex);
    }
}