#include "DataNode.h"

std::string Vector::GetGLSLType(void) const
{
    switch (size)
    {
    case 0: return "invalid_type_name";
    case 1: return "float";
    case 2: return "vec2";
    case 3: return "vec3";
    case 4: return "vec4";
    default: assert(false);
    }
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

std::vector<unsigned int> DataNode::MakeVector(unsigned int dat)
{
    std::vector<unsigned int> dats;
    dats.insert(dats.end(), dat);
    return dats;
}
std::vector<unsigned int> DataNode::MakeVector(unsigned int dat, unsigned int dat2, unsigned int dat3)
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