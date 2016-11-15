#include "MultiDepthTestNode.h"

#include "../Math/Lower Math/FastRand.h"


typedef MultiDepthTestNode MDTN;


MDTN::MultiDepthTestNode(std::string _samplerName, DataLine depthTexUV, DataLine testDepth,
                         std::string name)
    : DataNode(MakeVector(depthTexUV, testDepth), name),
      samplerName(_samplerName)
{

}

std::string MDTN::GetOutputName(unsigned int outputIndex) const
{
    return GetName() + "_result";
}

void MDTN::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 2, "UV input for depth tex must be size 2!");
    Assert(GetInputs()[1].GetSize() == 1, "Depth value to test must be size 1!");
}

void MDTN::GetMyParameterDeclarations(UniformList& outUniforms) const
{
    outUniforms.push_back(Uniform(samplerName, UniformTypes::UT_VALUE_SAMPLER2D));
}
void MDTN::WriteMyOutputs(std::string& outCode) const
{
    const unsigned int nOffsets = 4;
    Vector2f randOffsets[nOffsets];
    FastRand fr(64634);
    for (unsigned int i = 0; i < nOffsets; ++i)
    {
        randOffsets[i] = Vector2f(Mathf::Lerp(-1.0f, 1.0f, fr.GetZeroToOne()),
                                  Mathf::Lerp(-1.0f, 1.0f, fr.GetZeroToOne()));
        randOffsets[i].Normalize();
    }
    
    const float searchScale = 0.00018f;


    std::string texUV = GetInputs()[0].GetValue(),
                testDepth = GetInputs()[1].GetValue();
    outCode += "\n\
    const float depthEpsilon = -0.0001;\n\
    vec2 searchScale = vec2(" + ToString(searchScale) + ");\n\
    float outTest = ";
    for (int i = 0; i < nOffsets; ++i)
    {
        outCode += "step(depthEpsilon, (texture2D(" + samplerName + ", " + texUV + " + " +
                                                 "vec2(" + ToString(randOffsets[i].x) + ", " +
                                                           ToString(randOffsets[i].y) + ") * " +
                                                 "searchScale).r - " + testDepth + "))";
        if (i < nOffsets - 1)
        {
            outCode += " +\n\t\t";
        }
        else
        {
            outCode += ";\n";
        }
    }
    outCode += "\tfloat " + GetOutputName(0) + " = outTest / " + ToString(nOffsets) + ".0;\n\n";
}

void MDTN::WriteExtraData(DataWriter* writer) const
{
    writer->WriteString(samplerName, "Depth Tex Uniform Name");
}
void MDTN::ReadExtraData(DataReader* reader)
{
    reader->ReadString(samplerName);
}

std::string MDTN::GetInputDescription(unsigned int index) const
{
    if (index == 0)
    {
        return "Depth texture UV";
    }
    else
    {
        return "Depth value to test";
    }
}