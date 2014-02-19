#include "ShaderGenerator.h"

#include "DataNodeIncludes.h"
#include "../../../Math/Higher Math/Lighting.h"


typedef ShaderGenerator SG;
typedef RenderingChannels RC;
typedef ShaderHandler::Shaders Shaders;


unsigned int SG::GetChannelInputSize(RC channel)
{
    switch (channel)
    {
        case RC::RC_Diffuse:
        case RC::RC_Normal:
        case RC::RC_ObjectVertexOffset:
        case RC::RC_WorldVertexOffset:
            return 3;

        case RC::RC_Distortion:
            return 2;

        case RC::RC_DiffuseIntensity:
        case RC::RC_Opacity:
        case RC::RC_Specular:
        case RC::RC_SpecularIntensity:
            return 1;

        default: assert(false);
    }
}

bool SG::IsChannelUsed(RC channel, RenderingModes mode, bool useLighting, const LightSettings & settings)
{
    switch (channel)
    {
        case RC::RC_Diffuse:
        case RC::RC_DiffuseIntensity:
        case RC::RC_ObjectVertexOffset:
        case RC::RC_WorldVertexOffset:
            return true;

        case RC::RC_Normal:
        case RC::RC_Specular:
        case RC::RC_SpecularIntensity:
            return useLighting;

        case RC::RC_Distortion:
        case RC::RC_Opacity:
            return IsModeTransparent(mode);

        default: assert(false);
    }
}

void SG::GetUsedChannels(RenderingModes mode, bool useLighting, const LightSettings & settings, std::vector<RenderingChannels> & outChannels)
{
    outChannels.insert(outChannels.end(), RC::RC_Diffuse);
    outChannels.insert(outChannels.end(), RC::RC_DiffuseIntensity);
    outChannels.insert(outChannels.end(), RC::RC_ObjectVertexOffset);
    outChannels.insert(outChannels.end(), RC::RC_WorldVertexOffset);

    if (useLighting)
    {
        outChannels.insert(outChannels.end(), RC::RC_Normal);
        outChannels.insert(outChannels.end(), RC::RC_Specular);
        outChannels.insert(outChannels.end(), RC::RC_SpecularIntensity);
    }

    if (IsModeTransparent(mode))
    {
        outChannels.insert(outChannels.end(), RC::RC_Distortion);
        outChannels.insert(outChannels.end(), RC::RC_Opacity);
    }
}

void SG::GenerateShaders(std::string & outVShader, std::string & outFShader, UniformDictionary & outUniforms,
                         RenderingModes mode, bool useLighting, const LightSettings & settings,
                         std::unordered_map<RenderingChannels, DataLine> channels)
{
    //First, make sure the channels are all correctly set up.
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        //Make sure each channel is actually used in the current rendering state.
        assert(IsChannelUsed(iterator->first, mode, useLighting, settings));

        //Make sure each channel has a valid input data line size.
        assert(GetChannelInputSize(iterator->first) == iterator->second.GetDataLineSize());
    }


    //If an expected channel input doesn't exist, create it and use a default value.
    std::vector<RC> validChannels;
    GetUsedChannels(mode, useLighting, settings, validChannels);
    for (int i = 0; i < validChannels.size(); ++i)
    {
        if (channels.find(validChannels[i]) == channels.end())
        {
            switch (validChannels[i])
            {
            case RC::RC_Diffuse:
                channels[RC::RC_Diffuse] = DataLine(DataNodePtr(new ObjectColorNode()), 0);
                break;
            case RC::RC_DiffuseIntensity:
                channels[RC::RC_DiffuseIntensity] = DataLine(Vector(1.0f));
                break;
            case RC::RC_Distortion:
                channels[RC::RC_Distortion] = DataLine(Vector2f(0.0f, 0.0f));
                break;
            case RC::RC_Normal:
                channels[RC::RC_Normal] = DataLine(DataNodePtr(new WorldNormalNode()), 0);
                break;
            case RC::RC_ObjectVertexOffset:
                channels[RC::RC_ObjectVertexOffset] = DataLine(Vector3f(0.0f, 0.0f, 0.0f));
                break;
            case RC::RC_Opacity:
                channels[RC::RC_Opacity] = DataLine(1.0f);
                break;
            case RC::RC_Specular:
                channels[RC::RC_Specular] = DataLine(0.0f);
                break;
            case RC::RC_SpecularIntensity:
                channels[RC::RC_SpecularIntensity] = DataLine(32.0f);
                break;
            case RC::RC_WorldVertexOffset:
                channels[RC::RC_WorldVertexOffset] = DataLine(Vector3f(0.0f, 0.0f, 0.0f));
                break;

            default: assert(false);
            }
        }
    }


    //Create the shaders.

/*

    std::string uniformDeclarations = std::string() +
"uniform float " + MaterialConstants::ElapsedTimeName + ";                   \n\
uniform vec3 " + MaterialConstants::CameraPosName + ", " +
                 MaterialConstants::CameraForwardName + ", " +
                 MaterialConstants::CameraUpName + ", " +
                 MaterialConstants::CameraSideName + ";                     \n\
uniform mat4 " + MaterialConstants::WorldMatName + ", " +
                 MaterialConstants::ViewMatName + ", " +
                 MaterialConstants::ProjMatName + ", " +
                 MaterialConstants::WVPMatName + ";\n\n";
    std::string vertexShaderHeader = std::string() +
"#version 330                                                                       \n\
                                                                                    \n\
layout (location = 0) in vec3 " + MaterialConstants::InPos + ";                     \n\
layout (location = 1) in vec4 " + MaterialConstants::InColor + ";                   \n\
layout (location = 2) in vec2 " + MaterialConstants::InUV + ";                      \n\
layout (location = 3) in vec3 " + MaterialConstants::InNormal + ";                  \n\
                                                                                    \n\
out vec3 " + MaterialConstants::OutPos + ";                                         \n\
out vec4 " + MaterialConstants::OutColor + ";                                       \n\
out vec2 " + MaterialConstants::OutUV + ";                                          \n\
out vec3 " + MaterialConstants::OutNormal + ";                                      \n\
                                                                                    \n\
" + uniformDeclarations;
    std::string fragmentShaderHeader = std::string() +
"#version 330                                                                       \n\
                                                                                    \n\
in vec3 " + MaterialConstants::OutPos + ";                                          \n\
in vec4 " + MaterialConstants::OutColor + ";                                        \n\
in vec2 " + MaterialConstants::OutUV + ";                                           \n\
in vec3 " + MaterialConstants::OutNormal + ";                                       \n\
                                                                                    \n\
out vec4 " + MaterialConstants::FinalOutColor + ";                                  \n\
                                                                                    \n\
" + uniformDeclarations;

*/

    std::string vertShader = MaterialConstants::GetVertexHeader(useLighting),
                fragShader = MaterialConstants::GetFragmentHeader(useLighting);


    //Collect data node uniforms, functions, and output.

    UniformDictionary vertexUniformDict, fragmentUniformDict;
    std::vector<std::string> vertexFunctionDecls, fragmentFunctionDecls;
    std::string vertexCode, fragmentCode;
    std::vector<unsigned int> vertexUniforms, fragmentUniforms,
                              vertexFunctions, fragmentFunctions,
                              vertexCodes, fragmentCodes;

    for (int i = 0; i < validChannels.size(); ++i)
    {
        const DataLine & data = channels[validChannels[i]];
        if (!data.IsConstant())
        {
            std::string oldVCode = vertexCode,
                        oldFCode = fragmentCode;

            switch (validChannels[i])
            {
                case RC::RC_WorldVertexOffset:
                case RC::RC_ObjectVertexOffset:
                    DataNode::SetShaderType(DataNode::Shaders::SH_Vertex_Shader);
                    data.GetDataNodeValue()->GetParameterDeclarations(vertexUniformDict, vertexUniforms);
                    data.GetDataNodeValue()->GetFunctionDeclarations(vertexFunctionDecls, vertexFunctions);
                    data.GetDataNodeValue()->WriteOutputs(vertexCode, vertexCodes);
                    break;

                case RC::RC_Diffuse:
                case RC::RC_DiffuseIntensity:
                case RC::RC_Distortion:
                case RC::RC_Normal:
                case RC::RC_Opacity:
                case RC::RC_Specular:
                case RC::RC_SpecularIntensity:
                    DataNode::SetShaderType(DataNode::Shaders::SH_Fragment_Shader);
                    data.GetDataNodeValue()->GetParameterDeclarations(fragmentUniformDict, fragmentUniforms);
                    data.GetDataNodeValue()->GetFunctionDeclarations(fragmentFunctionDecls, fragmentFunctions);
                    data.GetDataNodeValue()->WriteOutputs(fragmentCode, fragmentCodes);
                    break;

                default: assert(false);
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
    if (useLighting)
    {
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_ColorName] = UniformValue(Vector3f(1.0f, 1.0f, 1.0f), 0, "dirLight.Col");
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_DirName] = UniformValue(Vector3f(1.0f, 1.0f, -1.0f).Normalized(), 0, "dirLight.Dir");
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName] = UniformValue(0.2f, 0, "dirLight.Ambient");
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName] = UniformValue(0.8f, 0, "dirLight.Diffuse");
    }


    //Add in the helper functions to the shader code.

    if (vertexFunctionDecls.size() > 0)
    {
        vertShader += "\n\n//Helper functions.";
        for (unsigned int i = 0; i < vertexFunctionDecls.size(); ++i)
            vertShader += vertexFunctionDecls[i] + "\n";
        vertShader += "\n\n\n";
    }
    if (fragmentFunctionDecls.size() > 0)
    {
        fragShader += "\n\n//Helper functions.";
        for (unsigned int i = 0; i < fragmentFunctionDecls.size(); ++i)
            fragShader += fragmentFunctionDecls[i] + "\n";
    }
    fragShader += "\n\n\n";


    //Set up the main() functions.
    //TODO: Add/modify a DataNode so that players can transform a normal from object space into world space.
    //TODO: Create a separate rendering mode for distortion -- it is opaque, and blending is manually done in the fragment shader using a sampler for the rendered world.
    //TODO: For vertex shader: calculate the screen depth of the vertex from 0 - 1 after transforming the position into clip space, pass it to the vertex shader, and expose it as a DataNode.
    vertShader += "\n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute outputs.                                                                                  \n\
    " + vertexCode + "                                                                                  \n\
                                                                                                        \n\
    //Compute shader outputs.                                                                           \n\
    " + MaterialConstants::OutColor + " = " + MaterialConstants::InColor + ";                           \n\
    " + MaterialConstants::OutNormal + " = (" + MaterialConstants::WorldMatName +
                                             " * vec4(" + MaterialConstants::InNormal + ", 0.0)).xyz;   \n\
    " + MaterialConstants::OutUV + " = " + MaterialConstants::InUV + ";                                 \n\
                                                                                                        \n\
    //Compute world position.                                                                           \n\
    vec3 pos3 = " + MaterialConstants::InPos + ";                                                       \n\
    pos3 += " + channels[RC::RC_ObjectVertexOffset].GetValue() + ";                                     \n\
    vec4 pos4 = (" + MaterialConstants::WorldMatName + " * vec4(pos3, 1.0)" + ");                       \n\
    pos3 = pos4.xyz / pos4.w;                                                                           \n\
    pos3 += " + channels[RC::RC_WorldVertexOffset].GetValue() + ";                                      \n\
                                                                                                        \n\
    " + MaterialConstants::OutPos + " = pos3;                                                           \n\
                                                                                                        \n\
    gl_Position = " + MaterialConstants::WVPMatName + " * vec4(" + MaterialConstants::InPos + ", 1.0);  \n\
}";
    fragShader += "                                                                                     \n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute outputs.                                                                                  \n\
    " + fragmentCode + "                                                                                \n\
                                                                                                        \n";
    //If diffuse intensity isn't used, don't bother computing it.
    if (channels[RC::RC_DiffuseIntensity].IsConstant() && channels[RC::RC_DiffuseIntensity].GetConstantValue().GetValue()[0] == 1.0f)
        fragShader += "\tvec3 diffuseCol = " + channels[RC::RC_Diffuse].GetValue() + ";\n";
    else fragShader +=
        "\tvec3 diffuseCol = vec3(pow(" + channels[RC::RC_Diffuse].GetValue() + ".x, " + channels[RC::RC_DiffuseIntensity].GetValue() + "), " +
                                 "pow(" + channels[RC::RC_Diffuse].GetValue() + ".y, " + channels[RC::RC_DiffuseIntensity].GetValue() + "), " +
                                 "pow(" + channels[RC::RC_Diffuse].GetValue() + ".z, " + channels[RC::RC_DiffuseIntensity].GetValue() + "));\n";
    //If this material uses lighting, calculate lighting stuff.
    if (useLighting)
    {
        fragShader +=
    "\tvec3 normalVal = " + channels[RC::RC_Normal].GetValue() + ";                           \n\
     diffuseCol *= getLight(normalVal, " + MaterialConstants::OutPos + " - " +
                                           MaterialConstants::CameraPosName +
                            ", " + channels[RC::RC_Specular].GetValue() +
                            ", " + channels[RC::RC_SpecularIntensity].GetValue() +
                            ", " + MaterialConstants::DirectionalLightName + ");              \n\n";
        }
    //Now change how the final color is computed based on rendering mode.
    switch (mode)
    {
    case RenderingModes::RM_Opaque:
        fragShader += "\t" + MaterialConstants::FinalOutColor + " = vec4(diffuseCol, 1.0);\n";
        break;
    case RenderingModes::RM_Transluscent:
        fragShader += "\t" + MaterialConstants::FinalOutColor + " = vec4(diffuseCol, " + channels[RC::RC_Opacity].GetValue() + ");\n";
        break;
    case RenderingModes::RM_Additive:
        fragShader += "\t" + MaterialConstants::FinalOutColor + " = vec4(diffuseCol, " + channels[RC::RC_Opacity].GetValue() + ");\n";
        break;

    default: assert(false);
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
}