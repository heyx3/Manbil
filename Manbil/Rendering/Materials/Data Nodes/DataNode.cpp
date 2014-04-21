#include "DataNode.h"


unsigned int DataNode::nextID = 0;
DataNode::Shaders DataNode::shaderType = DataNode::Shaders::SH_Vertex_Shader;

int DataNode::EXCEPTION_ASSERT_FAILED = 1;


void DataNode::SetFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    assert(outputIndex < outputs.size());

    for (unsigned int input = 0; input < inputs.size(); ++input)
    {
        if (!inputs[input].IsConstant() && UsesInput(input, outputIndex))
        {
            try
            {
                inputs[input].GetDataNodeValue()->SetFlags(flags, inputs[input].GetDataNodeLineIndex());
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with setting flags for input #" + std::to_string(input + 1) + ", " +
                            inputs[input].GetDataNodeValue()->GetName() + ": " + inputs[input].GetDataNodeValue()->errorMsg;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    SetMyFlags(flags, outputIndex);
}
void DataNode::GetParameterDeclarations(UniformDictionary & outUniforms, std::vector<unsigned int> & writtenNodeIDs) const
{
    //Exit if this node has already been used.
    if (std::find(writtenNodeIDs.begin(), writtenNodeIDs.end(), GetUniqueID()) != writtenNodeIDs.end())
        return;

    //First get the parameter declarations for all the child nodes.
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant())
        {
            try
            {
                const DataNodePtr & dataN = inputs[i].GetDataNodeValue();
                dataN->GetParameterDeclarations(outUniforms, writtenNodeIDs);
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with declaring uniforms for input #" + std::to_string(i + 1) + ", " +
                           inputs[i].GetDataNodeValue()->GetName() + ": " + inputs[i].GetDataNodeValue()->errorMsg;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    //Now get this node's own parameter declarations.
    writtenNodeIDs.insert(writtenNodeIDs.end(), GetUniqueID());
    GetMyParameterDeclarations(outUniforms);
}
void DataNode::GetFunctionDeclarations(std::vector<std::string> & outDecls, std::vector<unsigned int> & writtenNodeIDs) const
{
    //Exit if this node has already been used.
    if (std::find(writtenNodeIDs.begin(), writtenNodeIDs.end(), GetUniqueID()) != writtenNodeIDs.end())
        return;

    //First get the function declarations for all the child nodes.
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant())
        {
            try
            {
                const DataNodePtr & dataN = inputs[i].GetDataNodeValue();
                dataN->GetFunctionDeclarations(outDecls, writtenNodeIDs);
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with declaring functions for input #" + std::to_string(i + 1) + ", " +
                           inputs[i].GetDataNodeValue()->GetName() + ": " + inputs[i].GetDataNodeValue()->errorMsg;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    //Now get this node's own function declarations.
    writtenNodeIDs.insert(writtenNodeIDs.end(), GetUniqueID());
    GetMyFunctionDeclarations(outDecls);
}
void DataNode::WriteOutputs(std::string & outCode, std::vector<unsigned int> & writtenNodeIDs) const
{
    //Exit if this node has already been used.
    if (std::find(writtenNodeIDs.begin(), writtenNodeIDs.end(), GetUniqueID()) != writtenNodeIDs.end())
        return;

    //First get the outputs for all the child nodes.
    for (unsigned int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant())
        {
            try
            {
                const DataNodePtr & dataN = inputs[i].GetDataNodeValue();
                dataN->WriteOutputs(outCode, writtenNodeIDs);
            }
            catch (int ex)
            {
                assert(ex == EXCEPTION_ASSERT_FAILED);
                errorMsg = "Error with writing outputs for input #" + std::to_string(i + 1) + ", " +
                           inputs[i].GetDataNodeValue()->GetName() + ": " + inputs[i].GetDataNodeValue()->errorMsg;
                throw EXCEPTION_ASSERT_FAILED;
            }
        }
    }

    //Now get this node's own outputs.
    writtenNodeIDs.insert(writtenNodeIDs.end(), GetUniqueID());
    WriteMyOutputs(outCode);
}


std::vector<DataLine> DataNode::MakeVector(const DataLine & dat)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, const DataLine & dat2)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, const DataLine & dat2, const DataLine & dat3, const DataLine & dat4)
{
    std::vector<DataLine> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    dats.insert(dats.end(), dat4);
    return dats;
}
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, unsigned int wherePut, const std::vector<DataLine> & moreDats)
{
    std::vector<DataLine> cpy = moreDats;

    cpy.insert(cpy.begin() + wherePut, dat);

    return cpy;
}

std::vector<unsigned int> DataNode::MakeVector(unsigned int dat)
{
    std::vector<unsigned int> dats;
    dats.insert(dats.end(), dat);
    return dats;
}
std::vector<unsigned int> DataNode::MakeVector(unsigned int dat, unsigned int dat2)
{
    std::vector<unsigned int> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    return dats;
}
std::vector<unsigned int> DataNode::MakeVector(unsigned int dat, unsigned int dat2, unsigned int dat3)
{
    std::vector<unsigned int> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    return dats;
}
std::vector<unsigned int> DataNode::MakeVector(unsigned int dat, unsigned int dat2, unsigned int dat3, unsigned int dat4)
{
    std::vector<unsigned int> dats;
    dats.insert(dats.end(), dat);
    dats.insert(dats.end(), dat2);
    dats.insert(dats.end(), dat3);
    dats.insert(dats.end(), dat4);
    return dats;
}