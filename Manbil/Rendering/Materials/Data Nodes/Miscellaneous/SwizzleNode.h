#pragma once

#include "../DataNode.h"


//Outputs a combination of an input's components.
class SwizzleNode : public DataNode
{
public:

    enum Components { C_X, C_Y, C_Z, C_W, };

    virtual std::string GetName(void) const override { return "swizzleNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));
        return GetName() + std::to_string(GetUniqueID()) + "_swiz";
    }

    SwizzleNode(const DataLine & in, Components newX)
        : DataNode(MakeVector(in), MakeVector(1)), nComps(1)
    {
        comps[0] = newX;
    }
    SwizzleNode(const DataLine & in, Components newX, Components newY)
        : DataNode(MakeVector(in), MakeVector(2)), nComps(2)
    {
        comps[0] = newX;
        comps[1] = newY;
    }
    SwizzleNode(const DataLine & in, Components newX, Components newY, Components newZ)
        : DataNode(MakeVector(in), MakeVector(3)), nComps(3)
    {
        comps[0] = newX;
        comps[1] = newY;
        comps[2] = newZ;
    }
    SwizzleNode(const DataLine & in, Components newX, Components newY, Components newZ, Components newW)
        : DataNode(MakeVector(in), MakeVector(4)), nComps(4)
    {
        comps[0] = newX;
        comps[1] = newY;
        comps[2] = newZ;
        comps[3] = newW;
    }

protected:

    virtual void WriteMyOutputs(std::string & outStr) const override
    {
        outStr += "\t" + VectorF(nComps).GetGLSLType() + " " + GetOutputName(0) + " = " + GetInput().GetValue() + ".";
        for (unsigned int i = 0; i < nComps; ++i)
        {
            outStr += ToString(comps[i]);
        }
        outStr += ";\n";
    }

private:

    const DataLine & GetInput(void) const { return GetInputs()[0]; }

    static std::string ToString(Components comp)
    {
        switch (comp)
        {
            case Components::C_X: return "x";
            case Components::C_Y: return "y";
            case Components::C_Z: return "z";
            case Components::C_W: return "w";
            default: assert(false);
        }

        return "invalid";
    }

    Components comps[4];
    unsigned int nComps;
};