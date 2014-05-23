#include "ShaderGenerator.h"

#include "DataNodeIncludes.h"
#include "../../../Math/Higher Math/Lighting.h"
#include "../../../Material.h"


typedef ShaderGenerator SG;
typedef RenderingChannels RC;
typedef ShaderHandler::Shaders Shaders;
typedef std::unordered_map<RenderingChannels, DataLine> RenderChannels;



DataNode::Shaders SG::GetShaderType(RC channel)
{
    if (IsChannelVertexOutput(channel, false))
        return DataNode::Shaders::SH_Vertex_Shader;
    if (IsChannelColorOutput(channel, false))
        return DataNode::Shaders::SH_Fragment_Shader;

    switch (channel)
    {
        case RC::RC_ScreenVertexPosition:
            return DataNode::Shaders::SH_Vertex_Shader;

        case RC::RC_Color:
        case RC::RC_Opacity:
            return DataNode::Shaders::SH_Fragment_Shader;

        default:
            assert(false);
            return DataNode::Shaders::SH_Vertex_Shader;
    }
}

unsigned int SG::GetChannelInputSize(RC channel)
{
    switch (channel)
    {
        case RC::RC_ScreenVertexPosition:
            return 4;
            
        case RC::RC_Color:
            return 3;

        case RC::RC_Opacity:
            return 1;

        default:
            assert(IsChannelVertexOutput(channel, true) ||
                   IsChannelColorOutput(channel, false));
            return 0;
    }
}

void SG::AddMissingChannels(RenderChannels & channels, RenderingModes mode, bool useLighting, const LightSettings & settings)
{
    if (channels.find(RC::RC_Color) == channels.end())
        channels[RC::RC_Color] = DataLine(Vector3f(1.0f, 0.0f, 1.0f));
    if (channels.find(RC::RC_ScreenVertexPosition) == channels.end())
        assert(false);
    if (channels.find(RC::RC_Opacity) == channels.end())
        channels[RC::RC_Opacity] = DataLine(1.0f);
}

SG::GeneratedMaterial SG::GenerateMaterial(std::unordered_map<RenderingChannels, DataLine> & channels,
                                           UniformDictionary & uniforms, const VertexAttributes & attribs,
                                           RenderingModes mode, bool useLighting, const LightSettings & settings)
{
    std::string vs, fs;
    std::string error = GenerateShaders(vs, fs, uniforms, mode, useLighting, settings, attribs, channels);

    if (!error.empty()) return GeneratedMaterial(error);
    else return GeneratedMaterial(new Material(vs, fs, uniforms, mode, useLighting, settings));
}

std::string SG::GenerateShaders(std::string & outVShader, std::string & outFShader, UniformDictionary & outUniforms,
                                RenderingModes mode, bool useLighting, const LightSettings & settings, const VertexAttributes & attribs,
                                std::unordered_map<RenderingChannels, DataLine> & channels)
{
    AddMissingChannels(channels, mode, useLighting, settings);

    //First, make sure the channels are all correctly set up.
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        //Make sure each channel has a valid input size. Vertex outputs can be any size.
        bool isChanVertOut = IsChannelVertexOutput(iterator->first, false);
        unsigned int channelSize = GetChannelInputSize(iterator->first),
                     inputSize = iterator->second.GetDataLineSize();
        if (!isChanVertOut && channelSize != inputSize)
            return std::string() + "Channel " + ChannelToString(iterator->first) + "'s input must be size " +
                                    std::to_string(channelSize) + ", but is size " + std::to_string(inputSize);
    }

    //Make sure color outputs are defined correctly.
    if (channels.find(RenderingChannels::RC_COLOR_OUT_4) != channels.end())
        if (channels.find(RenderingChannels::RC_COLOR_OUT_3) == channels.end())
            return "Color output 4 was set, but not color output 3!";
    if (channels.find(RenderingChannels::RC_COLOR_OUT_3) != channels.end())
        if (channels.find(RenderingChannels::RC_COLOR_OUT_2) == channels.end())
            return "Color output 3 was set, but not color output 2!";


    //Get information about what external data each shader uses.
    MaterialUsageFlags vertFlags, fragFlags;
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        if (iterator->second.IsConstant()) continue;

        try
        {
            switch (GetShaderType(iterator->first))
            {
                case DataNode::Shaders::SH_Vertex_Shader:
                    DataNode::SetShaderType(Shaders::SH_Vertex_Shader);
                    iterator->second.GetDataNodeValue()->SetFlags(vertFlags, iterator->second.GetDataNodeLineIndex());
                    break;
                case DataNode::Shaders::SH_Fragment_Shader:
                    DataNode::SetShaderType(Shaders::SH_Fragment_Shader);
                    iterator->second.GetDataNodeValue()->SetFlags(fragFlags, iterator->second.GetDataNodeLineIndex());
                    break;

                default: assert(false);
            }
        }
        catch (int ex)
        {
            assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
            return std::string() + "Error setting flags for channel " + ChannelToString(iterator->first) +
                       ", node " + iterator->second.GetDataNodeValue()->GetName() + ": " +
                       iterator->second.GetDataNodeValue()->GetError();
        }
    }


    //Generate the vertex output/fragment input declarations.
    std::string vertOutput, fragInput;
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        if (IsChannelVertexOutput(iterator->first, false))
        {
            std::string name = MaterialConstants::VertexOutNameBase + std::to_string(GetVertexOutputNumber(iterator->first));
            vertOutput += "out " + VectorF(iterator->second.GetDataLineSize()).GetGLSLType() + " " + name + ";\n";
            fragInput += "in " + VectorF(iterator->second.GetDataLineSize()).GetGLSLType() + " " + name + ";\n";
        }
    }

    //Generate the color outputs.
    std::string fragOutput;
    std::unordered_map<RenderingChannels, std::string> outputNames;
    if (channels.find(RenderingChannels::RC_COLOR_OUT_2) == channels.end())
    {
        fragOutput = "out vec4 " + MaterialConstants::FragmentOutName + ";\n";
        outputNames[RenderingChannels::RC_Color] = MaterialConstants::FragmentOutName;
    }
    else
    {
        for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
        {
            if (!IsChannelColorOutput(iterator->first, true)) continue;

            unsigned int numb = GetColorOutputNumber(iterator->first);
            fragOutput += "layout (location = " + std::to_string(numb) + ") out vec4 " +
                          MaterialConstants::FragmentOutName + std::to_string(numb + 1) + ";\n";
            outputNames[iterator->first] = MaterialConstants::FragmentOutName + std::to_string(numb + 1);
        }
    }


    //Generate the headers for each shader.
    std::string vertShader = MaterialConstants::GetVertexHeader(vertOutput, attribs, vertFlags),
                fragShader = MaterialConstants::GetFragmentHeader(fragInput, fragOutput, fragFlags);


    //Generate uniforms, functions, and output calculations.

    UniformDictionary vertexUniformDict, fragmentUniformDict;
    std::vector<std::string> vertexFunctionDecls, fragmentFunctionDecls;
    std::string vertexCode, fragmentCode;
    std::vector<unsigned int> vertexUniforms, fragmentUniforms,
                              vertexFunctions, fragmentFunctions,
                              vertexCodes, fragmentCodes;

    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        const DataLine & data = iterator->second;
        if (!data.IsConstant())
        {
            std::string oldVCode = vertexCode,
                        oldFCode = fragmentCode;
            try
            {
                switch (GetShaderType(iterator->first))
                {
                    case DataNode::Shaders::SH_Vertex_Shader:
                        DataNode::SetShaderType(DataNode::Shaders::SH_Vertex_Shader);
                        data.GetDataNodeValue()->GetParameterDeclarations(vertexUniformDict, vertexUniforms);
                        data.GetDataNodeValue()->GetFunctionDeclarations(vertexFunctionDecls, vertexFunctions);
                        data.GetDataNodeValue()->WriteOutputs(vertexCode, vertexCodes);
                        break;

                    case DataNode::Shaders::SH_Fragment_Shader:
                        DataNode::SetShaderType(DataNode::Shaders::SH_Fragment_Shader);
                        data.GetDataNodeValue()->GetParameterDeclarations(fragmentUniformDict, fragmentUniforms);
                        data.GetDataNodeValue()->GetFunctionDeclarations(fragmentFunctionDecls, fragmentFunctions);
                        data.GetDataNodeValue()->WriteOutputs(fragmentCode, fragmentCodes);
                        break;

                    default: assert(false);
                }
            }
            catch (int ex)
            {
                assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
                return std::string() + "Error setting parameters, functions, and outputs for channel " +
                    ChannelToString(iterator->first) + ", node " + data.GetDataNodeValue()->GetName() + ": " +
                    data.GetDataNodeValue()->GetError();
            }

            if (oldVCode != vertexCode) vertexCode += "\n";
            if (oldFCode != fragmentCode) fragmentCode += "\n";
        }
    }


    //Add in the uniforms to the shader code.

    if (vertexUniformDict.FloatUniforms.size() + vertexUniformDict.FloatArrayUniforms.size() + vertexUniformDict.MatrixUniforms.size() + vertexUniformDict.TextureUniforms.size() > 0)
    {
        vertShader += "//Uniforms.\n";
        for (auto iterator = vertexUniformDict.FloatUniforms.begin(); iterator != vertexUniformDict.FloatUniforms.end(); ++iterator)
            vertShader += iterator->second.GetDeclaration() + "\n";
        for (auto iterator = vertexUniformDict.FloatArrayUniforms.begin(); iterator != vertexUniformDict.FloatArrayUniforms.end(); ++iterator)
            vertShader += iterator->second.GetDeclaration() + "\n";
        for (auto iterator = vertexUniformDict.MatrixUniforms.begin(); iterator != vertexUniformDict.MatrixUniforms.end(); ++iterator)
            vertShader += iterator->second.GetDeclaration() + "\n";
        for (auto iterator = vertexUniformDict.TextureUniforms.begin(); iterator != vertexUniformDict.TextureUniforms.end(); ++iterator)
            vertShader += iterator->second.GetDeclaration() + "\n";
    }
    if (fragmentUniformDict.FloatUniforms.size() + fragmentUniformDict.FloatArrayUniforms.size() + fragmentUniformDict.MatrixUniforms.size() + fragmentUniformDict.TextureUniforms.size() > 0)
    {
        fragShader += "//Uniforms.\n";
        for (auto iterator = fragmentUniformDict.FloatUniforms.begin(); iterator != fragmentUniformDict.FloatUniforms.end(); ++iterator)
            fragShader += iterator->second.GetDeclaration() + "\n";
        for (auto iterator = fragmentUniformDict.FloatArrayUniforms.begin(); iterator != fragmentUniformDict.FloatArrayUniforms.end(); ++iterator)
            fragShader += iterator->second.GetDeclaration() + "\n";
        for (auto iterator = fragmentUniformDict.MatrixUniforms.begin(); iterator != fragmentUniformDict.MatrixUniforms.end(); ++iterator)
            fragShader += iterator->second.GetDeclaration() + "\n";
        for (auto iterator = fragmentUniformDict.TextureUniforms.begin(); iterator != fragmentUniformDict.TextureUniforms.end(); ++iterator)
            fragShader += iterator->second.GetDeclaration() + "\n";
    }


    //Add in the helper functions to the shader code.

    if (vertexFunctionDecls.size() > 0)
    {
        vertShader += "\n\n//Helper functions.\n";
        for (unsigned int i = 0; i < vertexFunctionDecls.size(); ++i)
            vertShader += vertexFunctionDecls[i] + "\n";
        vertShader += "\n\n\n";
    }
    if (fragmentFunctionDecls.size() > 0)
    {
        fragShader += "\n\n//Helper functions.\n";
        for (unsigned int i = 0; i < fragmentFunctionDecls.size(); ++i)
            fragShader += fragmentFunctionDecls[i] + "\n";
    }
    fragShader += "\n\n\n";


    //Set up the main() functions.
    DataNode::SetShaderType(DataNode::Shaders::SH_Vertex_Shader);
    vertShader += "\n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute outputs.                                                                                  \n\
    " + vertexCode + "                                                                                  \n\
                                                                                                        \n\
";
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
        if (IsChannelVertexOutput(iterator->first, false))
            vertShader += "    " + (MaterialConstants::VertexOutNameBase + std::to_string(GetVertexOutputNumber(iterator->first))) +
                          " = " + iterator->second.GetValue() + ";\n";
    vertShader += 
"                                                                                                        \n\
    gl_Position = " + channels[RenderingChannels::RC_ScreenVertexPosition].GetValue() + ";              \n\
}";

    DataNode::SetShaderType(DataNode::Shaders::SH_Fragment_Shader);
    fragShader += "                                                                                     \n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute outputs.                                                                                  \n\
    " + fragmentCode + "                                                                                \n\
                                                                                                        \n";

    //Now output the final color(s).
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        if (iterator->first == RC::RC_Color)
        {
            fragShader += "\t" + outputNames[iterator->first] + " = vec4(" + channels[RC::RC_Color].GetValue() + ", " + channels[RC::RC_Opacity].GetValue() + ");\n";
        }
        else if (IsChannelColorOutput(iterator->first, false))
        {
            fragShader += "\t" + outputNames[iterator->first] + " = " + channels[iterator->first].GetValue() + ";\n";
        }
    }
    fragShader += "}";


    outVShader += vertShader;
    outFShader += fragShader;



    //Finalize the uniforms.

    for (auto iterator = fragmentUniformDict.FloatUniforms.begin(); iterator != fragmentUniformDict.FloatUniforms.end(); ++iterator)
        outUniforms.FloatUniforms[iterator->first] = iterator->second;
    for (auto iterator = fragmentUniformDict.FloatArrayUniforms.begin(); iterator != fragmentUniformDict.FloatArrayUniforms.end(); ++iterator)
        outUniforms.FloatArrayUniforms[iterator->first] = iterator->second;
    for (auto iterator = fragmentUniformDict.MatrixUniforms.begin(); iterator != fragmentUniformDict.MatrixUniforms.end(); ++iterator)
        outUniforms.MatrixUniforms[iterator->first] = iterator->second;
    for (auto iterator = fragmentUniformDict.TextureUniforms.begin(); iterator != fragmentUniformDict.TextureUniforms.end(); ++iterator)
        outUniforms.TextureUniforms[iterator->first] = iterator->second;

    for (auto iterator = vertexUniformDict.FloatUniforms.begin(); iterator != vertexUniformDict.FloatUniforms.end(); ++iterator)
        if (outUniforms.FloatUniforms.find(iterator->first) == outUniforms.FloatUniforms.end())
            outUniforms.FloatUniforms[iterator->first] = iterator->second;
    for (auto iterator = vertexUniformDict.FloatArrayUniforms.begin(); iterator != vertexUniformDict.FloatArrayUniforms.end(); ++iterator)
        if (outUniforms.FloatArrayUniforms.find(iterator->first) == outUniforms.FloatArrayUniforms.end())
            outUniforms.FloatArrayUniforms[iterator->first] = iterator->second;
    for (auto iterator = vertexUniformDict.MatrixUniforms.begin(); iterator != vertexUniformDict.MatrixUniforms.end(); ++iterator)
        if (outUniforms.MatrixUniforms.find(iterator->first) == outUniforms.MatrixUniforms.end())
            outUniforms.MatrixUniforms[iterator->first] = iterator->second;
    for (auto iterator = vertexUniformDict.TextureUniforms.begin(); iterator != vertexUniformDict.TextureUniforms.end(); ++iterator)
        if (outUniforms.TextureUniforms.find(iterator->first) == outUniforms.TextureUniforms.end())
            outUniforms.TextureUniforms[iterator->first] = iterator->second;

    return "";
}