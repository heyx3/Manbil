#include "CustomExpressionNode.h"


void CustomExpressionNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\t" + VectorF(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) + " = " + expression + ";\n";
}

std::string CustomExpressionNode::InterpretExpression(std::string expr, std::vector<DataLine> input)
{
    std::string newStr = expr;

    for (unsigned int i = 0; i < input.size(); ++i)
    {
        std::string variable = "'" + std::to_string(i) + "'",
                    value = "(" + input[i].GetValue() + ")";

        std::string::size_type index = newStr.find(variable);
        unsigned int numbInstances = 0;

        while (index != std::string::npos)
        {
            numbInstances += 1;
            newStr.replace(index, variable.size(), value);
            index = newStr.find(variable);
        }

        Assert(numbInstances > 0, "Couldn't find any instances of input index " + std::to_string(i) + " in the expression \"" + expr + "\"");
    }

    return newStr;
}