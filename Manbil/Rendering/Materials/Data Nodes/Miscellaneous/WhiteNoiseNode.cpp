#include "WhiteNoiseNode.h"

void WhiteNoiseNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputs()[0]).GetGLSLType(),
                seed1 = GetInputs()[0].GetValue(),
                seed2 = GetInputs()[1].GetValue(),
                multiplier = GetInputs()[2].GetValue(),
                output = GetOutputName(0);

    outCode += "\t" + vecType + " " + output + " = fract(sin(dot(" + seed1 + ", " + seed2 + ")) * " + multiplier + ");\n";
}

std::vector<DataLine> WhiteNoiseNode::makeInputs(const DataLine & seed, DataLine randPeriodMultiplier)
{
    std::vector<DataLine> dat;
    dat.insert(dat.end(), seed);

    const float seed1 = 12.232f,
                seed2 = 78.567f,
                seed3 = 1002.52f,
                seed4 = 0.5524f;
    VectorF seedVal;
    switch (seed.GetDataLineSize())
    {
        case 1: seedVal = VectorF(seed1); break;
        case 2: seedVal = VectorF(Vector2f(seed1, seed2)); break;
        case 3: seedVal = VectorF(Vector3f(seed1, seed2, seed3)); break;
        case 4: seedVal = VectorF(Vector4f(seed1, seed2, seed3, seed4)); break;
        default: assert(false);
    }
    dat.insert(dat.end(), DataLine(seedVal));

    dat.insert(dat.end(), randPeriodMultiplier);

    return dat;
}