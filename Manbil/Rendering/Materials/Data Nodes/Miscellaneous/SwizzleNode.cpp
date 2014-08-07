#include "SwizzleNode.h"


MAKE_NODE_READABLE_CPP(SwizzleNode, Vector2f(), C_X)


std::string SwizzleNode::GetOutputName(unsigned int index) const
{
    std::string outStr = GetInputs()[0].GetValue();

    if (GetOutputSize(0) > 1)
    {
        outStr += ".";

        for (unsigned int i = 0; i < nComps; ++i)
        {
            switch (comps[i])
            {
                case C_X: outStr += "x";
                case C_Y: outStr += "y";
                case C_Z: outStr += "z";
                case C_W: outStr += "w";

                default: Assert(false, "Unknown component '" + ToString(comps[i]));
            }
        }
    }

    return outStr;
}

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

bool SwizzleNode::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteUInt(nComps, "numberOfSwizzleComponents", outError))
    {
        outError = "Error writing out the number of swizzle components (" + ToString(nComps) + "): " + outError;
        return false;
    }
    for (unsigned int i = 0; i < nComps; ++i)
    {
        std::string outStr;
        switch (comps[i])
        {
            case C_X: outStr = "x";
            case C_Y: outStr = "y";
            case C_Z: outStr = "z";
            case C_W: outStr = "w";
            default: Assert(false, "Unknown component '" + ToString(comps[i]));
        }

        std::string outName;
        switch (i)
        {
            case 0: outName = "x";
            case 1: outName = "y";
            case 2: outName = "z";
            case 3: outName = "w";
            default: Assert(false, "Unknown component '" + ToString(comps[i]));
        }

        if (!writer->WriteString(outStr, "out" + outName, outError))
        {
            outError = "Error writing the out" + outName + " component value '" + outStr + "': " + outError;
            return false;
        }
    }

    return true;
}
bool SwizzleNode::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> tryNComps = reader->ReadUInt(outError);
    if (!tryNComps.HasValue())
    {
        outError = "Error reading the number of components: " + outError;
        return false;
    }
    nComps = tryNComps.GetValue();

    for (unsigned int i = 0; i < nComps; ++i)
    {
        std::string outName;
        switch (i)
        {
            case 0: outName = "x";
            case 1: outName = "y";
            case 2: outName = "z";
            case 3: outName = "w";
            default: Assert(false, "Unknown component '" + ToString(comps[i]));
        }

        MaybeValue<std::string> tryCmp = reader->ReadString(outError);
        if (!tryCmp.HasValue())
        {
            outError = "Error reading component output '" + outName + "': " + outError;
            return false;
        }
        if (tryCmp.GetValue().compare("x") == 0 || tryCmp.GetValue().compare("r") == 0)
            comps[i] = C_X;
        else if (tryCmp.GetValue().compare("y") == 0 || tryCmp.GetValue().compare("g") == 0)
            comps[i] = C_Y;
        if (tryCmp.GetValue().compare("z") == 0 || tryCmp.GetValue().compare("b") == 0)
            comps[i] = C_Z;
        else if (tryCmp.GetValue().compare("w") == 0 || tryCmp.GetValue().compare("a") == 0)
            comps[i] = C_W;
        else
        {
            outError = "Unknown component in output '" + outName + "': " + tryCmp.GetValue();
            return false;
        }
    }

    return true;
}