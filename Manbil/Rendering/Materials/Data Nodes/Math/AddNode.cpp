#include "AddNode.h"

std::string AddNode::WriteMyOutputConnection(unsigned int dataLine, std::string inputName, std::string outputName) const
{
    assert(dataLine == 0);

    const DataLine & data = GetInputs()[0];
    std::string dataType = Vector(data.GetDataLineSize()).GetGLSLType();

    if (data.IsConstant())
    {
        std::string ret = Vector(data.GetDataLineSize()).GetGLSLType() + inputName + " = " + dataType + "(";
    }
    else
    {

    }
}