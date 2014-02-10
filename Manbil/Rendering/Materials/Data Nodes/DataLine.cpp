#include "DataLine.h"

#include "DataNode.h"


unsigned int DataLine::GetDataLineSize(void) const
{
    return (isConstantValue ? constantValue.GetSize() :
                              nonConstantValue->GetOutputs()[nonConstantValueIndex]);
}

std::string DataLine::GetValue(ShaderHandler::Shaders shaderType) const
{
    if (isConstantValue)
    {
        std::string ret = constantValue.GetGLSLType() + "(";
        for (int i = 0; i < constantValue.GetSize(); ++i)
        {
            if (i > 0) ret += ", ";
            ret += std::to_string(constantValue.GetValue()[i]);
        }
        ret += ")";
        return ret;
    }
    else
    {
        return nonConstantValue->GetOutputName(nonConstantValueIndex, shaderType);
    }
}