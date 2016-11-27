#include "WaterRendering.h"


ADD_NODE_REFLECTION_DATA_CPP(WaterNode, Vector3f(), Vector3f())


const char* WaterNode::UniformName_DP_TSC_H_P = "dropoffPoints_timesSinceCreated_heights_periods";
const char* WaterNode::UniformName_sXY_SP = "sourcesXY_speeds";
const char* WaterNode::UniformName_D_A_P = "direction_amplitude_period";
const char* WaterNode::UniformName_TSC = "timesSinceCreated";


#pragma warning(disable: 4100)
unsigned int WaterNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
std::string WaterNode::GetOutputName(unsigned int i) const
{
    return GetName() + (i == 0 ? "_waterHeightOffset" : "_waterNormal");
}
#pragma warning(default: 4100)

WaterNode::WaterNode(const DataLine& vertexObjPosInput, const DataLine& fragmentObjPosInput,
                     unsigned int maxRipples_Directional, unsigned int maxRipples_Circular,
					 std::string name)
    : DataNode(MakeVector(vertexObjPosInput, fragmentObjPosInput), name),
      maxDirectional(maxRipples_Directional), maxCircular(maxRipples_Circular)
{

}


bool WaterNode::UsesInput(unsigned int inputIndex) const
{
    switch (CurrentShader)
    {
        case SH_VERTEX:
            return &GetInputs()[inputIndex] == &GetObjectPosVInput();

        case SH_FRAGMENT:
            return &GetInputs()[inputIndex] == &GetObjectPosVOutput();

        default:
            Assert(false, std::string("Unknown shader type ") + ToString(CurrentShader));
            return DataNode::UsesInput(inputIndex);
    }
}
bool WaterNode::UsesInput(unsigned int inputIndex, unsigned int outputIndex) const
{
    switch (CurrentShader)
    {
        case SH_VERTEX:
            return (inputIndex == 0);

        case SH_FRAGMENT:
            return (inputIndex == 1);

        default:
            Assert(false, std::string("Unknown shader type ") + ToString(CurrentShader));
            return DataNode::UsesInput(inputIndex, outputIndex);
    }
}


void WaterNode::GetMyParameterDeclarations(UniformList& outUniforms) const
{
    if (maxCircular > 0)
    {
        //Create good default values.
		std::vector<Vector4f> dp_tsc_h_p;
		dp_tsc_h_p.resize(maxCircular);
		std::vector<Vector3f> sXY_sp;
		sXY_sp.resize(maxCircular);
        for (unsigned int i = 0; i < maxCircular; ++i)
        {
            dp_tsc_h_p[i] = Vector4f(0.0001f, 0.0f, 0.0f, 1.0f);
            sXY_sp[i] = Vector3f(0.0f, 0.0f, 0.0001f);
        }

        outUniforms.push_back(Uniform(UniformName_DP_TSC_H_P, UT_VALUE_F_ARRAY));
        outUniforms[outUniforms.size() - 1].FloatArray().SetData((float*)dp_tsc_h_p.data(),
																 maxCircular, 4);

        outUniforms.push_back(Uniform(UniformName_sXY_SP, UT_VALUE_F_ARRAY));
        outUniforms[outUniforms.size() - 1].FloatArray().SetData((float*)sXY_sp.data(),
																 maxCircular, 3);
    }
    if (maxDirectional > 0)
    {
		//Create good default values.
		std::vector<Vector4f> d_a_p;
		d_a_p.resize(maxDirectional);
		std::vector<float> tsc;
		tsc.resize(maxDirectional);
        for (unsigned int i = 0; i < maxDirectional; ++i)
        {
            d_a_p[i] = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
            tsc[i] = 0.0001f;
        }

        outUniforms.push_back(Uniform(UniformName_D_A_P, UT_VALUE_F_ARRAY));
        outUniforms[outUniforms.size() - 1].FloatArray().SetData((float*)d_a_p.data(),
																 maxDirectional, 4);

        outUniforms.push_back(Uniform(UniformName_TSC, UT_VALUE_F_ARRAY));
        outUniforms[outUniforms.size() - 1].FloatArray().SetData(tsc.data(), maxDirectional, 1);
    }
}
void WaterNode::GetMyFunctionDeclarations(std::vector<std::string>& outDecls) const
{
    std::string func =
"float getWaveHeight(vec2 horizontalPos)\n\
{\n\
    float offset = 0.0f;                                                                            \n";
    if (maxCircular > 0)
    {
        std::string dptschp = std::string(UniformName_DP_TSC_H_P) + "[i]",
                    sxysp = std::string(UniformName_sXY_SP) + "[i]";
        func +=
"    //Circular ripples.                                                                            \n\
    for (int i = 0; i < " + std::to_string(maxCircular) + "; ++i)                                   \n\
    {                                                                                               \n\
        float dropoffPoint = " + dptschp + ".x;                                                     \n\
        float timeSinceCreated = " + dptschp + ".y;                                                 \n\
        float height = " + dptschp + ".z;                                                           \n\
        float period = " + dptschp + ".w;                                                           \n\
        vec2 source = " + sxysp + ".xy;                                                             \n\
        float speed = " + sxysp + ".z;                                                              \n\
                                                                                                    \n\
        float dist = distance(source, horizontalPos);                                               \n\
        float heightScale = max(0, mix(0.0, 1.0, 1.0 - (dist / dropoffPoint)));                     \n\
        heightScale = pow(heightScale, 3.0); //TODO: turn exponent into a uniform.                  \n\
                                                                                                    \n\
        float cutoff = period * speed * timeSinceCreated;                                           \n\
        cutoff = max(0, (cutoff - dist) / cutoff);                                                  \n\
                                                                                                    \n\
        float innerVal = (dist / period) + (-timeSinceCreated * speed);                             \n\
        float waveScale = height * heightScale * cutoff;                                            \n\
                                                                                                    \n\
        float heightOffset = sin(innerVal);                                                         \n\
        heightOffset = -1.0 + 2.0 * pow(0.5 + (0.5 * heightOffset), 2.0); //TODO: Turn exponent into a uniform.\n\
        offset += waveScale * heightOffset;                                                         \n\
    }\n";
    }
    if (maxDirectional > 0)
    {
        std::string dap = std::string(UniformName_D_A_P) + "[i]",
                    tsc = std::string(UniformName_TSC) + "[i]";
        func +=
"    //Directional ripples.                                                                        \n\
    for (int i = 0; i < " + std::to_string(maxDirectional) + "; ++i)                               \n\
    {                                                                                              \n\
        vec2 flowDir = " + dap + ".xy;                                                             \n\
        float speed = length(flowDir);                                                             \n\
        flowDir /= speed;                                                                          \n\
        float amplitude = " + dap + ".z;                                                           \n\
        float period = " + dap + ".w;                                                              \n\
        float timeSinceCreated = " + tsc + ";                                                      \n\
                                                                                                   \n\
        float dist = dot(flowDir, horizontalPos);                                                  \n\
																								   \n\
        float innerVal = (dist / period) + (-timeSinceCreated * speed);                               \n\
        float waveScale = amplitude;                                                               \n\
																								   \n\
        float heightOffset = sin(innerVal);                                                        \n\
        heightOffset = -1.0 + 2.0 * pow(0.5 + (0.5 * heightOffset), 2.0); //TODO: Turn exponent into a uniform.\n\
        offset += waveScale * heightOffset;                                                        \n\
    }\n";
    }
    func +=
"    return offset;                                                                                \n\
}\n";
    outDecls.push_back(func);

    func =
"struct NormalData                                                                      \n\
{                                                                                       \n\
    vec3 normal, tangent, bitangent;                                                    \n\
};                                                                                      \n\
NormalData getWaveNormal(vec2 horizontalPos)                                            \n\
{//TODO: Use derivative of summed sine waves instead of this.                           \n\
    NormalData dat;                                                                     \n\
    dat.normal = vec3(0.0, 0.0, 0.001);                                                 \n\
    dat.tangent = vec3(0.001, 0.0, 0.0);                                                \n\
    dat.bitangent = vec3(0.0, 0.001, 0.0);                                              \n\
                                                                                        \n\
    vec3 epsilon = vec3(0.00001, -0.00001, 0.0);                                                \n\
                                                                                        \n\
    //Get the height at nearby vertices and compute the normal via cross-product.       \n\
                                                                                        \n\
    vec2 one_zero = horizontalPos + epsilon.xz,                                         \n\
         nOne_zero = horizontalPos + epsilon.yz,                                        \n\
         zero_one = horizontalPos + epsilon.zx,                                         \n\
         zero_nOne = horizontalPos + epsilon.zy;                                        \n\
                                                                                        \n\
    vec3 p_zero_zero = vec3(horizontalPos, getWaveHeight(horizontalPos));               \n\
    vec3 p_one_zero = vec3(one_zero, getWaveHeight(one_zero)),                          \n\
         p_nOne_zero = vec3(nOne_zero, getWaveHeight(nOne_zero)),                       \n\
         p_zero_one = vec3(zero_one, getWaveHeight(zero_one)),                          \n\
         p_zero_nOne = vec3(zero_nOne, getWaveHeight(zero_nOne));                       \n\
                                                                                        \n\
    vec3 norm1 = cross(normalize(p_one_zero - p_zero_zero),                             \n\
                       normalize(p_zero_one - p_zero_zero)),                            \n\
         norm2 = cross(normalize(p_nOne_zero - p_zero_zero),                            \n\
                       normalize(p_zero_nOne - p_zero_zero)),                           \n\
         normFinal = normalize((norm1 * sign(norm1.z)) + (norm2 * sign(norm2.z)));      \n\
                                                                                        \n\
    dat.normal = normFinal;                                                             \n\
    return dat;                                                                         \n\
}                                                                                       \n\
";
    outDecls.push_back(func);
}
void WaterNode::WriteMyOutputs(std::string& outCode) const
{
    std::string posOutput = GetOutputName(GetVertexPosOutputIndex()),
			    normalOutput = GetOutputName(GetSurfaceNormalOutputIndex());

    switch (CurrentShader)
    {
        case SH_VERTEX:
            outCode += "\tvec3 " + posOutput + " = vec3(" + GetObjectPosVInput().GetValue() +
                            ".xy, getWaveHeight(" + GetObjectPosVInput().GetValue() + ".xy));\n";
            outCode += "\tvec3 " + normalOutput + " = getWaveNormal(" +
                            GetObjectPosVInput().GetValue() + ".xy).normal;\n";
            break;

        case SH_FRAGMENT:
            outCode += "\tvec3 " + posOutput + " = vec3(" + GetObjectPosVOutput().GetValue() +
                            ".xy, getWaveHeight(" + GetObjectPosVOutput().GetValue() + ".xy));\n";
            outCode += "\tvec3 " + normalOutput + " = getWaveNormal(" +
                            GetObjectPosVOutput().GetValue() + ".xy).normal;\n";
            break;

		case SH_GEOMETRY:
			Assert(false, "WaterNode doesn't work in the geometry shader");
			break;

        default: assert(false);
    }
}

std::string WaterNode::GetInputDescription(unsigned int index) const
{
    return std::string("Object-space postion of the element in the ") +
                (index == 0 ? "Vertex" : "Fragment") + " shader";
}

void WaterNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteUInt(maxDirectional, "MaxDirectionalRipples");
    writer->WriteUInt(maxCircular, "MaxCirclarRipples");
}
void WaterNode::ReadExtraData(DataReader* reader)
{
    reader->ReadUInt(maxDirectional);
    reader->ReadUInt(maxCircular);
}

void WaterNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 3,
           "vertex shader object-space position input must have size 3; has size " +
               ToString(GetInputs()[0].GetSize()));
    Assert(GetInputs()[1].GetSize() == 3,
           "fragment shader object-space position input must have size 3; has size " +
               ToString(GetInputs()[1].GetSize()));
}