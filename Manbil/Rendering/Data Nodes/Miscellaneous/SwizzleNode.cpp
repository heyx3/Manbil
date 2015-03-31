#include "SwizzleNode.h"


ADD_NODE_REFLECTION_DATA_CPP(SwizzleNode, Vector2f(), SwizzleNode::C_X)


#pragma warning(disable: 4100)
std::string SwizzleNode::GetOutputName(unsigned int index) const
{
    std::string outStr = GetInputs()[0].GetValue();

    outStr += ".";

    for (unsigned int i = 0; i < nComps; ++i)
    {
        Components iComp = comps[i];
        switch (iComp)
        {
            case C_X:
                outStr += "x";
                break;
            case C_Y:
                outStr += "y";
                break;
            case C_Z:
                outStr += "z";
                break;
            case C_W:
                outStr += "w";
                break;

            default: Assert(false, "Unknown component '" + ToString(comps[i]));
        }
    }

    return outStr;
}
#pragma warning(default: 4100)

SwizzleNode::SwizzleNode(const DataLine & in, Components x, std::string name)
    : DataNode(MakeVector(in), name), nComps(1)
{
    comps[0] = x;
}
SwizzleNode::SwizzleNode(const DataLine & in, Components x, Components y, std::string name)
    : DataNode(MakeVector(in), name), nComps(2)
{
    comps[0] = x;
    comps[1] = y;
}
SwizzleNode::SwizzleNode(const DataLine & in, Components x, Components y, Components z, std::string name)
    : DataNode(MakeVector(in), name), nComps(3)
{
    comps[0] = x;
    comps[1] = y;
    comps[2] = z;
}
SwizzleNode::SwizzleNode(const DataLine & in, Components x, Components y, Components z, Components w, std::string name)
    : DataNode(MakeVector(in), name), nComps(4)
{
    comps[0] = x;
    comps[1] = y;
    comps[2] = z;
    comps[3] = w;
}

std::string SwizzleNode::GetInputDescription(unsigned int index) const
{
    Assert(index == 0, "Invalid input index " + ToString(index));
    return "toSwizzle";
}

#pragma warning(disable: 4100)
void SwizzleNode::WriteMyOutputs(std::string & outStr) const
{
    //No need to write anything.
}
#pragma warning(default: 4100)

void SwizzleNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteUInt(nComps, "Number of swizzle components");

    for (unsigned int i = 0; i < nComps; ++i)
    {
        std::string outStr;
        switch (comps[i])
        {
            case C_X: outStr = "x"; break;
            case C_Y: outStr = "y"; break;
            case C_Z: outStr = "z"; break;
            case C_W: outStr = "w"; break;
            default: Assert(false, "Unknown component '" + ToString(comps[i]));
        }

        std::string outName;
        switch (i)
        {
            case 0: outName = "x"; break;
            case 1: outName = "y"; break;
            case 2: outName = "z"; break;
            case 3: outName = "w"; break;
            default: Assert(false, "Unknown component '" + ToString(comps[i]));
        }

        writer->WriteString(outStr, "out" + outName);
    }
}
void SwizzleNode::ReadExtraData(DataReader* reader)
{
    reader->ReadUInt(nComps);

    for (unsigned int i = 0; i < nComps; ++i)
    {
        std::string outName;
        switch (i)
        {
            case 0: outName = "x"; break;
            case 1: outName = "y"; break;
            case 2: outName = "z"; break;
            case 3: outName = "w"; break;
            default: Assert(false, "Unknown component '" + ToString(comps[i]));
        }

        std::string component;
        reader->ReadString(component);
        if (component.compare("x") == 0 || component.compare("r") == 0)
        {
            comps[i] = C_X;
        }
        else if (component.compare("y") == 0 || component.compare("g") == 0)
        {
            comps[i] = C_Y;
        }
        if (component.compare("z") == 0 || component.compare("b") == 0)
        {
            comps[i] = C_Z;
        }
        else if (component.compare("w") == 0 || component.compare("a") == 0)
        {
            comps[i] = C_W;
        }
        else
        {
            reader->ErrorMessage = "Unknown component in output '" + outName + "': " + component;
            throw DataReader::EXCEPTION_FAILURE;
        }
    }
}