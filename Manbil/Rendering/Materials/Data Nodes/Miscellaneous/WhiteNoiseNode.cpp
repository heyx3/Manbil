#include "WhiteNoiseNode.h"

void WhiteNoiseNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    std::string vecType = Vector(GetOutputs()[0]).GetGLSLType(),
                seed1 = GetInputs()[0].GetValue(shaderType),
                seed2 = GetInputs()[1].GetValue(shaderType),
                output = GetOutputName(0, shaderType);

    outCode += "\t" + vecType + " " + output + " = fract(sin(dot(" + seed1 + ", " + seed2 + ")) * 43758.5453);\n";
}

std::vector<DataLine> WhiteNoiseNode::makeInputs(const DataLine & seed)
{
    const float seed1 = 1251.12312f,
                seed2 = 1421.13512f,
                seed3 = 2511.13512f,
                seed4 = 1541.35125f;

    std::vector<DataLine> dat;
    dat.insert(dat.begin(), seed);

    switch (seed.GetDataLineSize())
    {
        case 1: dat.insert(dat.begin(), DataLine(Vector(seed1))); break;
        case 2: dat.insert(dat.begin(), DataLine(Vector(Vector2f(seed1, seed2)))); break;
        case 3: dat.insert(dat.begin(), DataLine(Vector(Vector3f(seed1, seed2, seed3)))); break;
        case 4: dat.insert(dat.begin(), DataLine(Vector(Vector4f(seed1, seed2, seed3, seed4)))); break;
        default: assert(false);
    }

    return dat;
}