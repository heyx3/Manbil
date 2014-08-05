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

WhiteNoiseNode::WhiteNoiseNode(const DataLine & seed, std::string name, DataLine randPeriodMultiplier)
    : DataNode(MakeVector(seed, randPeriodMultiplier),
               []() { return Ptr(new WhiteNoiseNode(DataLine(5.31f))); },
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
            multiplier = std::to_string(mult1);
            break;
        case 2:
            multiplier = "vec2(" + std::to_string(mult1) + ", " + std::to_string(mult2) + ")";
            break;
        case 3:
            multiplier = "vec3(" + std::to_string(mult1) + ", " + std::to_string(mult2) + ", " + std::to_string(mult3) + ")";
            break;
        case 4:
            multiplier = "vec4(" + std::to_string(mult1) + ", " + std::to_string(mult2) + ", " + std::to_string(mult3) + ", " + std::to_string(mult4) + ")";
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