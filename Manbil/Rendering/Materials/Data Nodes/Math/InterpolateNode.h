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
    };


    virtual std::string GetTypeName(void) const override { return "interpolation"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return BasicMath::Max(GetMaxInput().GetSize(), GetInterpInput().GetSize());
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_interpolated";
    }

    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetInterpInput(void) const { return GetInputs()[2]; }

    InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type, std::string name = "");

    
protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outFuncs) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        return (index == 0 ? "Min" : (index == 1 ? "Max" : "To Interpolate"));
    }

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override
    {
        unsigned int smoothLevel;
        switch (intType)
        {
            case IT_Linear:
                smoothLevel = 0;
                break;
            case IT_Smooth:
                smoothLevel = 1;
                break;
            case IT_VerySmooth:
                smoothLevel = 2;
                break;

            default:
                outError = "Unknown interpolation type '" + ToString(intType);
                return false;
        }

        if (!writer->WriteUInt(smoothLevel, "Smooth Level (0-2)", outError))
        {
            outError = "Error writing out smooth level of " + ToString(smoothLevel) + ": " + outError;
            return false;
        }

        return true;
    }
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override
    {
        MaybeValue<unsigned int> trySmoothLevel = reader->ReadUInt(outError);
        if (!trySmoothLevel.GetValue())
        {
            outError = "Error reading smooth level: " + outError;
            return false;
        }

        switch (trySmoothLevel.GetValue())
        {
            case 0:
                intType = IT_Linear;
                break;
            case 1:
                intType = IT_Smooth;
                break;
            case 2:
                intType = IT_VerySmooth;
                break;

            default:
                outError = "Smooth level must be between 0 and 2 inclusive; it was " + ToString(trySmoothLevel.GetValue());
                return false;
        }

        return true;
    }


private:

    InterpolationType intType;
};