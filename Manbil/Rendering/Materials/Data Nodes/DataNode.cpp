#include "DataNode.h"


//The bit of code that will actually generate the glsl code.
//Appends the code to the end of "outCode". Takes in a list of all nodes that have already added their code to "outCode".
void DataNode::WriteOutputs(std::string & outCode, std::vector<unsigned int> & writtenNodeIDs) const
{
    //First write out all the child data nodes that haven't been written out yet.
    for (int i = 0; i < inputs.size(); ++i)
    {
        //If the input is a constant value, no need to write it out.
        if (!inputs[i].IsConstant())
        {
            DataNodePtr input = inputs[i].GetDataNodeValue();

            if (std::find(writtenNodeIDs.begin(), writtenNodeIDs.end(), input->id) == writtenNodeIDs.end())
            {
                writtenNodeIDs.insert(writtenNodeIDs.end(), input->id);

                input->WriteOutputs(outCode, writtenNodeIDs);
            }
        }
    }

    //Now write out this node's code.
    outCode += "//Outputs for " + GetName() + std::to_string(id) + ":\n";
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