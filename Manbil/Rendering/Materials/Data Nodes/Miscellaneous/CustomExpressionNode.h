#pragma once

#include "../DataNode.h"


//Allows the user to enter a custom expression instead of chaining together long strings of simple nodes.
//Each DataLine variable in the expression should be represented as '0', '1', '2', etc.
//For example, you could pass in "'0' * sin('1' + ('2' / '3'))" along with four DataLines.
class CustomExpressionNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "expressionNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Output index must be 0");
        return GetName() + std::to_string(GetUniqueID()) + "_result";
    }

    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize)
        : DataNode(std::vector<DataLine>(), MakeVector(expressionOutSize)), expression(expression)
    {

    }
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, DataLine in1)
        : DataNode(MakeVector(in1), MakeVector(expressionOutSize)), expression(InterpretExpression(_expression, MakeVector(in1)))
    {

    }
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, DataLine in1, DataLine in2)
        : DataNode(MakeVector(in1, in2), MakeVector(expressionOutSize)), expression(InterpretExpression(_expression, MakeVector(in1, in2)))
    {

    }
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, DataLine in1, DataLine in2, DataLine in3)
        : DataNode(MakeVector(in1, in2, in3), MakeVector(expressionOutSize)), expression(InterpretExpression(_expression, MakeVector(in1, in2, in3)))
    {

    }
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, const std::vector<DataLine> & inputs)
        : DataNode(inputs, MakeVector(expressionOutSize)), expression(InterpretExpression(_expression, inputs))
    {

    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    std::string expression;

    std::string InterpretExpression(std::string expression, std::vector<DataLine> inputs);
};