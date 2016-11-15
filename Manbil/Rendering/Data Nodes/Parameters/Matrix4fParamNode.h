#pragma once

#include "../DataNode.h"


//TODO: Handle different-sized matrices?

//A 4x4 matrix uniform parameter.
//Takes as input the vectors to multiply by this matrix.
//Outputs the result of each input multiplied with this matrix.
//Uses pre-multiplication (Matrix x Vector), not post-multiplication (Vector x Matrix).
//Only supports 4-dimensional vectors.
class Matrix4fParamNode : public DataNode
{
public:

    std::string UniformName;
    

    Matrix4fParamNode(std::string uniformName,
                      DataLine toMultiply, std::string name = "");
    Matrix4fParamNode(std::string uniformName,
                      DataLine toMultiply1, DataLine toMultiply2,
                      std::string name = "");
    Matrix4fParamNode(std::string uniformName,
                      DataLine toMultiply1, DataLine toMultiply2, DataLine toMultiply3,
                      std::string name = "");
    Matrix4fParamNode(std::string uniformName,
                      std::vector<DataLine> toMultiply, std::string name = "");
    
    
    virtual unsigned int GetNumbOutputs(void) const override { return GetInputs().size(); }

    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override { return 4; }


protected:

    virtual void AssertMyInputsValid(void) const override;

    virtual void GetMyParameterDeclarations(UniformList& outUniforms) const override;
    virtual void WriteMyOutputs(std::string& outCode) const override;

    virtual void WriteExtraData(DataWriter* writer) const override;
    virtual void ReadExtraData(DataReader* reader) override;


private:


    ADD_NODE_REFLECTION_DATA_H(MatrixParamNode)
};