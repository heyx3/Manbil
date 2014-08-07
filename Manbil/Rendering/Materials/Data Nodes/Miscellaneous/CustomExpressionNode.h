#pragma once

#include "../DataNode.h"


//Allows the user to enter a custom expression instead of chaining together long strings of simple nodes.
//Each DataLine variable in the expression should be represented as '0', '1', '2', etc.
//For example, you could pass in "'0' * sin('1' + ('2' / '3'))" along with four DataLines.
class CustomExpressionNode : public DataNode
{
public:

    const std::string & GetExpression(void) const { return expression; }
    unsigned int GetExpressionOutputSize(void) const { return expressionOutputSize; }

#pragma warning(disable: 4100)
    virtual unsigned int GetOutputSize(unsigned int index) const override { return expressionOutputSize; }
    virtual std::string GetOutputName(unsigned int index) const override { return GetName() + "_result"; }
#pragma warning(default: 4100)

    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, std::string name = "");
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, DataLine in1, std::string name = "");
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, DataLine in1, DataLine in2, std::string name = "");
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, DataLine in1, DataLine in2, DataLine in3, std::string name = "");
    CustomExpressionNode(std::string _expression, unsigned int expressionOutSize, const std::vector<DataLine> & inputs, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;


private:

    std::string expression;
    unsigned int expressionOutputSize;

    //Converts variables like '0', '1', etc. in the expression string into the actual inputs' output values.
    std::string InterpretExpression() const;

    MAKE_NODE_READABLE_H(CustomExpressionNode)
};