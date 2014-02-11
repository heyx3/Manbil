#pragma once

#include "../DataNode.h"


//Keeps a value inside a max and a min.
class ClampNode : DataNode
{
public:

    virtual std::string GetName(void) const override { return "clampNode"; }

    ClampNode(const DataLine & min, const DataLine & max, const DataLine & value)
        : DataNode(MakeVector(min, max, value), MakeVector(min.GetDataLineSize()))
    {
        assert(min.GetDataLineSize() == max.GetDataLineSize() && min.GetDataLineSize() == value.GetDataLineSize());
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType)
    {
        std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();
        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) +
                    " = clamp(" + GetValueInput().GetValue(shaderType) + ", " +
                                  GetMinInput().GetValue(shaderType) + ", " +
                                  GetMaxInput().GetValue(shaderType) + ");\n";
    }


private:

    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetValueInput(void) const { return GetInputs()[2]; }
};