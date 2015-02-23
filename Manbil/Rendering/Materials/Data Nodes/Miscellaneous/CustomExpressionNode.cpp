#include "CustomExpressionNode.h"


ADD_NODE_REFLECTION_DATA_CPP(CustomExpressionNode, "1.0f", 1)


CustomExpressionNode::CustomExpressionNode(std::string expr, unsigned int outSize, std::string name)
    : CustomExpressionNode(expr, outSize, std::vector<DataLine>(), name)
{

}
CustomExpressionNode::CustomExpressionNode(std::string expr, unsigned int outSize, DataLine in1, std::string name)
    : CustomExpressionNode(expr, outSize, MakeVector(in1), name)
{

}
CustomExpressionNode::CustomExpressionNode(std::string expr, unsigned int outSize, DataLine in1, DataLine in2, std::string name)
    : CustomExpressionNode(expr, outSize, MakeVector(in1, in2), name)
{

}
CustomExpressionNode::CustomExpressionNode(std::string expr, unsigned int outSize, DataLine in1, DataLine in2, DataLine in3, std::string name)
    : CustomExpressionNode(expr, outSize, MakeVector(in1, in2, in3), name)
{

}
CustomExpressionNode::CustomExpressionNode(std::string expr, unsigned int outSize, const std::vector<DataLine> & ins, std::string name)
    : expression(expr), expressionOutputSize(outSize),
      DataNode(ins, name)
{

}


void CustomExpressionNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\t" + VectorF(expressionOutputSize, 0).GetGLSLType() + " " +
                    GetOutputName(0) + " = " + InterpretExpression() + ";\n";
}

std::string CustomExpressionNode::InterpretExpression() const
{
    std::string newStr = expression;

    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        std::string variable = "'" + std::to_string(i) + "'",
                    value = "(" + GetInputs()[i].GetValue() + ")";

        std::string::size_type index = newStr.find(variable);
        unsigned int numbInstances = 0;

        while (index != std::string::npos)
        {
            numbInstances += 1;
            newStr.replace(index, variable.size(), value);
            index = newStr.find(variable);
        }

        Assert(numbInstances > 0,
               "Couldn't find any instances of input index " + ToString(i) +
                    " in the expression \"" + expression + "\"");
    }

    return newStr;
}


void CustomExpressionNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteString(expression, "GLSL expression");
    writer->WriteUInt(expressionOutputSize, "Output size");
}
void CustomExpressionNode::ReadExtraData(DataReader* reader)
{
    reader->ReadString(expression);
    reader->ReadUInt(expressionOutputSize);
}