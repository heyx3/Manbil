#pragma once

#include "../DataNode.h"


#define SFNCLASS(funcName, className, name, outputName) \
class className : public DataNode \
    { \
    public: \
        className(const DataLine & input) : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize())) { } \
        virtual std::string GetName(void) const override { return #name ; } \
        virtual std::string GetOutputName(unsigned int index) const override \
        { \
            Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index)); \
            return GetName() + std::to_string(GetUniqueID()) + "_" + #outputName ; \
        } \
    \
    protected: \
        virtual void WriteMyOutputs(std::string & outCode) const override \
        { \
            std::string vecType = VectorF(GetOutputs()[0]).GetGLSLType(); \
            outCode += "\t" + vecType + " " + GetOutputName(0) + " = (" + #funcName + "(" + GetInputs()[0].GetValue() + "));\n"; \
        } \
    };


SFNCLASS(sign, SignNode, signNode, sign)

SFNCLASS(ceil, CeilNode, ceilNode, ceil)
SFNCLASS(floor, FloorNode, floorNode, floor)

SFNCLASS(abs, AbsNode, absNode, abs)

SFNCLASS(sin, SineNode, sinNode, sin)
SFNCLASS(cos, CosineNode, cosNode, cos)

SFNCLASS(asin, InverseSineNode, invSineNode, invSine)
SFNCLASS(acos, InverseCosineNode, invCosNode, invCosine)

SFNCLASS(normalize, NormalizeNode, normalizeNode, normalize)

SFNCLASS(fract, FractNode, fractNode, fract)

SFNCLASS(1.0 - , OneMinusNode, oneMinusNode, oneMinus)

SFNCLASS(-, NegativeNode, negativeNode, negative)