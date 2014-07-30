#include "WhiteNoiseNode.h"

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