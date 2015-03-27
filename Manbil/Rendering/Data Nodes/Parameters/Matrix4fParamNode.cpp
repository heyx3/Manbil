#include "Matrix4fParamNode.h"


ADD_NODE_REFLECTION_DATA_CPP(Matrix4fParamNode, "", DataLine())


Matrix4fParamNode::Matrix4fParamNode(std::string uniformName,
                                     DataLine toMultiply, std::string name = "")
    : UniformName(uniformName), DataNode(MakeVector(toMultiply), name)
{

}
Matrix4fParamNode::Matrix4fParamNode(std::string uniformName,
                                     DataLine toMultiply1, DataLine toMultiply2,
                                     std::string name = "")
    : UniformName(uniformName), DataNode(MakeVector(toMultiply1, toMultiply2), name)
{

}
Matrix4fParamNode::Matrix4fParamNode(std::string uniformName,
                                     DataLine toMultiply1, DataLine toMultiply2, DataLine toMultiply3,
                                     std::string name = "")
    : UniformName(uniformName),
      DataNode(MakeVector(toMultiply1, toMultiply2, toMultiply3), name)
{

}
Matrix4fParamNode::Matrix4fParamNode(std::string uniformName,
                                     std::vector<DataLine> toMultiply, std::string name = "")
    : UniformName(uniformName), DataNode(toMultiply, name)
{

}


void Matrix4fParamNode::AssertMyInputsValid(void) const
{
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        Assert(GetInputs()[i].GetSize() == 4,
               "This matrix requires 4-dimensional vectors, but input #" +
                   ToString(i) + " is size " + ToString(GetInputs()[i].GetSize()));
    }
}

void Matrix4fParamNode::GetMyParameterDeclarations(UniformDictionary& outParams) const
{
    outParams.Matrices[UniformName] = UniformValueMatrix4f(Matrix4f(), UniformName);
}
void Matrix4fParamNode::WriteMyOutputs(std::string& outCode) const
{
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += "\tvec4 " + GetOutputName(i) + " = " +
                       UniformName + " * " + GetInputs()[i].GetValue() + ";\n";
    }
}

void Matrix4fParamNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteString(UniformName, "Uniform variable name");
}
void Matrix4fParamNode::ReadExtraData(DataReader* reader)
{
    reader->ReadString(UniformName);
}