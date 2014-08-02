#include "WhiteNoiseNode.h"



unsigned int WhiteNoiseNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return 1;
}
std::string WhiteNoiseNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_noiseVal";
}

WhiteNoiseNode::WhiteNoiseNode(const DataLine & seed, std::string name = "", DataLine randPeriodMultiplier = DataLine(43758.5453f))
    : DataNode(MakeVector(seed, randPeriodMultiplier),
               []() { return DataNodePtr(new WhiteNoiseNode(DataLine(5.31f))); },
               name)
{
    Assert(seed.GetSize() == randPeriodMultiplier.GetSize(),
           std::string() + "Seed and 'period multiplier' must be the same size!");
}

void WhiteNoiseNode::WriteMyOutputs(std::string & outCode) const
{
    std::string multiplier;
    const float mult1 = 12.232f,
                mult2 = 78.567f,
                mult3 = 1002.52f,
                mult4 = 0.5524f;

    switch (GetOutputSize(0))
    {
        case 1:
            multiplier = ToString(mult1);
            break;
        case 2:
            multiplier = std::string() + "vec2(" + ToString(mult1) + ", " + ToString(mult2) + ")";
            break;
        case 3:
            multiplier = std::string() + "vec3(" + ToString(mult1) + ", " + ToString(mult2) + ", " + ToString(mult3) + ")";
            break;
        case 4:
            multiplier = std::string() + "vec4(" + ToString(mult1) + ", " + ToString(mult2) + ", " + ToString(mult3) + ", " + ToString(mult4) + ")";
            break;

        default:
            Assert(false, "Invalid output size " + ToString(GetOutputSize(0)));
    }

    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType(),
                seed1 = GetInputs()[0].GetValue(),
                seed2 = GetInputs()[1].GetValue(),
                output = GetOutputName(0);

    outCode += "\t" + vecType + " " + output + " = fract(sin(dot(" + seed1 + ", " + seed2 + ")) * " + multiplier + ");\n";
}

std::string WhiteNoiseNode::GetInputDescription(unsigned int index) const
{
    Assert(index <= 1, "Invalid output index " + ToString(index));
    return (index == 0 ? "Seed" : "RandPeriodMultiplier");
}