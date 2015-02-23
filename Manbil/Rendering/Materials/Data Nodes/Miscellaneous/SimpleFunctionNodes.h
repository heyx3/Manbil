#pragma once

#include "../DataNode.h"


#define SFNCLASS(funcName, className, outputName) \
class className : public DataNode \
    { \
    public: \
        className(const DataLine& input, std::string name = "") \
            : DataNode(MakeVector(input), name) { } \
        \
        \
        virtual unsigned int GetOutputSize(unsigned int index) const override \
        { \
            Assert(index == 0, std::string() + "Invalid output index " + ToString(index)); \
            return GetInputs()[0].GetSize(); \
        } \
        virtual std::string GetOutputName(unsigned int index) const override \
        { \
            Assert(index == 0, std::string() + "Invalid output index " + ToString(index)); \
            return GetName() + "_" + #outputName ; \
        } \
    \
    protected: \
        virtual void WriteMyOutputs(std::string& outCode) const override \
        { \
            std::string vecType = VectorF(GetInputs()[0].GetSize(), 0).GetGLSLType(); \
            outCode += "\t" + vecType + " " + GetOutputName(0) + " = (" + \
                            #funcName + "(" + GetInputs()[0].GetValue() + "));\n"; \
        } \
        \
        ADD_NODE_REFLECTION_DATA_H(className) \
    };


SFNCLASS(sign, SignNode, sign)

SFNCLASS(ceil, CeilNode, ceil)
SFNCLASS(floor, FloorNode, floor)

SFNCLASS(abs, AbsNode, abs)

SFNCLASS(sin, SineNode, sin)
SFNCLASS(cos, CosineNode, cos)

SFNCLASS(asin, InverseSineNode, invSine)
SFNCLASS(acos, InverseCosineNode, invCosine)

SFNCLASS(normalize, NormalizeNode, normalize)

SFNCLASS(fract, FractNode, fract)

SFNCLASS(1.0 - , OneMinusNode, oneMinus)

SFNCLASS(-, NegativeNode, negative)