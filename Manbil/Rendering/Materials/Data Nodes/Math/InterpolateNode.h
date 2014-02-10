#pragma once

#include "../DataNode.h"

class InterpolateNode : public DataNode
{
public:

    enum InterpolationType
    {
        IT_Linear,
        IT_Smooth,
        IT_VerySmooth,
        IT_Power,
    };


    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetInterpInput(void) const { return GetInputs()[2]; }

    //Only valid if the interpolation type is "IT_Power".
    const DataLine & GetPowerInput(void) const { return GetInputs()[3]; }


    InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type)
        : DataNode(MakeVector(min, max, interp), MakeVector(min.GetDataLineSize(), max.GetDataLineSize(), interp.GetDataLineSize()))
    {
        assert(type != IT_Power);
        intType = type;
        
        unsigned int minS = min.GetDataLineSize(),
                     maxS = max.GetDataLineSize(),
                     intS = interp.GetDataLineSize();

        assert(minS == maxS && (intS == minS || intS == 1));
    }
    InterpolateNode(DataLine min, DataLine max, DataLine interp, DataLine power)
        : DataNode(MakeVector(min, max, interp, power), MakeVector(min.GetDataLineSize(), max.GetDataLineSize(), interp.GetDataLineSize()))
    {
        intType = InterpolationType::IT_Power;

        unsigned int minS = min.GetDataLineSize(),
                     maxS = max.GetDataLineSize(),
                     intS = interp.GetDataLineSize();

        assert(minS == maxS && (intS == minS || intS == 1));
    }

    virtual void GetFunctionDeclarations(std::vector<std::string> & outFuncs) override
    {
        if (intType != InterpolationType::IT_VerySmooth) return;

        std::string vType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();
        std::string vType2 = Vector(GetInputs()[2].GetDataLineSize()).GetGLSLType();
        std::string funcName = GetName() + "_verySmoothStep";
        bool floatInterp = (GetInputs()[2].GetDataLineSize() == 1);

        std::string str = vType + funcName + "(" + vType + " minV, " + vType + " maxV, " + vType2 + " interpolantV)\n\
                          {\n\
                                \tfloat interpValue = interpolant * interpolant * interpolant *\n\
                                        \t\t(10.0 + (interpolant * (-15.0 + (6.0 * interpolant))));\n\
                                 \treturn minV + (interpValue * (maxV - minV));\n\
                          }\n";
        
        outFuncs.insert(outFuncs.begin(), str);
    }

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string minMaxType = Vector(GetMinInput().GetDataLineSize()).GetGLSLType(),
                    interpType = Vector(GetInterpInput().GetDataLineSize()).GetGLSLType(),
                    returnType = Vector(BasicMath::Max(GetMinInput().GetDataLineSize(), GetInterpInput().GetDataLineSize())).GetGLSLType();

        switch (intType)
        {
        case IT_Linear:
            outCode += "\t" + returnType + " " + GetOutputName(0) + " = mix(" + GetMinInput().GetValue() + ", " +
                                                                                GetMaxInput().GetValue() + ", " +
                                                                                GetInterpInput().GetValue() + ");\n";
            break;

        case IT_Smooth:
            outCode += "\t" + returnType + " " + GetOutputName(0) + " = smoothstep(" + GetMinInput().GetValue() + ", " +
                                                                                       GetMaxInput().GetValue() + ", " +
                                                                                       GetInterpInput().GetValue() + ");\n";
            break;

        case IT_VerySmooth:
            outCode += "\t" + returnType + " " + GetOutputName(0) + " = " + GetName() + "_verySmoothStep(" + GetMinInput().GetValue() + ", " +
                                                                                                             GetMaxInput().GetValue() + ", " +
                                                                                                             GetInterpInput().GetValue() + ");\n";
            break;

        case IT_Power:
            outCode += "\t" + returnType + " " + GetOutputName(0) + " = mix(" + GetMinInput().GetValue() + ", " +
                                                                                GetMaxInput().GetValue() + ", " +
                                                                                "pow(" + GetInterpInput().GetValue() + ", " +
                                                                                         GetPowerInput().GetValue() + "));\n";
            break;

        default: assert(false);
        }
    }

private:

    InterpolationType intType;
    float it_power;
};