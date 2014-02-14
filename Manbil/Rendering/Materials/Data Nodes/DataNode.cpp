#include "DataNode.h"

void DataNode::GetParameterDeclarations(UniformDictionary & outUniforms, std::vector<unsigned int> & writtenNodeIDs) const
{
    //Exit if this node has already been used.
    if (std::find(writtenNodeIDs.begin(), writtenNodeIDs.end(), GetUniqueID()) != writtenNodeIDs.end())
        return;

    //First get the parameter declarations for all the child nodes.
    for (int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant())
        {
            const DataNodePtr & dataN = inputs[i].GetDataNodeValue();
            dataN->GetParameterDeclarations(outUniforms, writtenNodeIDs);
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
    for (int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant())
        {
            const DataNodePtr & dataN = inputs[i].GetDataNodeValue();
            dataN->GetFunctionDeclarations(outDecls, writtenNodeIDs);
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
    for (int i = 0; i < inputs.size(); ++i)
    {
        if (!inputs[i].IsConstant())
        {
            const DataNodePtr & dataN = inputs[i].GetDataNodeValue();
            dataN->WriteOutputs(outCode, writtenNodeIDs);
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
std::vector<DataLine> DataNode::MakeVector(const DataLine & dat, std::vector<DataLine>::const_iterator wherePut, const std::vector<DataLine> & moreDats)
{
    std::vector<DataLine> cpy = moreDats;

    cpy.insert(wherePut, dat);

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