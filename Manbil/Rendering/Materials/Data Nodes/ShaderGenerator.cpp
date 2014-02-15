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
        "#version 330                                                               \n\
                                                                                    \n\
        layout (location = 0) in vec3 " + MaterialConstants::InPos + ";             \n\
        layout (location = 1) in vec4 " + MaterialConstants::InColor + ";           \n\
        layout (location = 2) in vec2 " + MaterialConstants::InUV + ";              \n\
        layout (location = 3) in vec3 " + MaterialConstants::InNormal + ";          \n\
                                                                                    \n\
        out vec3 " + MaterialConstants::OutPos + ";                                 \n\
        out vec4 " + MaterialConstants::OutColor + ";                               \n\
        out vec2 " + MaterialConstants::OutUV + ";                                  \n\
        out vec3 " + MaterialConstants::OutNormal + ";                              \n\
                                                                                    \n\
        " + uniformDeclarations;
    std::string fragmentShaderHeader = std::string() +
        "#version 330                                                               \n\
                                                                                    \n\
        in vec3 " + MaterialConstants::OutPos + ";                                  \n\
        in vec4 " + MaterialConstants::OutColor + ";                                \n\
        in vec2 " + MaterialConstants::OutUV + ";                                   \n\
        in vec3 " + MaterialConstants::OutNormal + ";                               \n\
                                                                                    \n\
        out vec4 finalOutColor;                                                     \n\
                                                                                    \n\
        " + uniformDeclarations;


    std::string vertShader = vertexShaderHeader,
                fragShader = fragmentShaderHeader;


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

            vertexCode += "\n";
            fragmentCode += "\n";
        }
    }


    //Add in the uniforms to the shader code.

    vertShader += "//Uniforms.\n";
    for (auto iterator = vertexUniformDict.FloatUniforms.begin(); iterator != vertexUniformDict.FloatUniforms.end(); ++iterator)
        vertShader += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = vertexUniformDict.FloatArrayUniforms.begin(); iterator != vertexUniformDict.FloatArrayUniforms.end(); ++iterator)
        vertShader += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = vertexUniformDict.MatrixUniforms.begin(); iterator != vertexUniformDict.MatrixUniforms.end(); ++iterator)
        vertShader += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = vertexUniformDict.TextureUniforms.begin(); iterator != vertexUniformDict.TextureUniforms.end(); ++iterator)
        vertShader += iterator->second.GetDeclaration() + "\n";

    fragShader += "//Uniforms.\n";
    for (auto iterator = fragmentUniformDict.FloatUniforms.begin(); iterator != fragmentUniformDict.FloatUniforms.end(); ++iterator)
        fragShader += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = fragmentUniformDict.FloatArrayUniforms.begin(); iterator != fragmentUniformDict.FloatArrayUniforms.end(); ++iterator)
        fragShader += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = fragmentUniformDict.MatrixUniforms.begin(); iterator != fragmentUniformDict.MatrixUniforms.end(); ++iterator)
        fragShader += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = fragmentUniformDict.TextureUniforms.begin(); iterator != fragmentUniformDict.TextureUniforms.end(); ++iterator)
        fragShader += iterator->second.GetDeclaration() + "\n";

    if (useLighting)
    {
        fragShader += "\n//Lighting uniforms.\n\
                      struct DirectionalLight\n\
                      {\n\
                          vec3 Col, Dir;\n\
                          float Ambient, Diffuse;\n\
                      };\n\
                      uniform DirectionalLight dirLight;\n\
                      uniform float specular;\n\
                      uniform float specularIntensity;\n";
        fragmentUniformDict.FloatUniforms["dirLight.Col"] = UniformValue(Vector3f(1.0f, 1.0f, 1.0f), 0, "dirLight.Col");
        fragmentUniformDict.FloatUniforms["dirLight.Dir"] = UniformValue(Vector3f(1.0f, 1.0f, -1.0f).Normalized(), 0, "dirLight.Dir");
        fragmentUniformDict.FloatUniforms["dirLight.Ambient"] = UniformValue(0.2f, 0, "dirLight.Ambient");
        fragmentUniformDict.FloatUniforms["dirLight.Diffuse"] = UniformValue(0.8f, 0, "dirLight.Diffuse");
        fragmentUniformDict.FloatUniforms["specular"].Value[0] = 0.0f;
        fragmentUniformDict.FloatUniforms["specularIntensity"].Value[0] = 0.0f;
    }


    //Add in the helper functions to the shader code.

    vertShader += "\n\n//Helper functions.";
    for (unsigned int i = 0; i < vertexFunctionDecls.size(); ++i)
        vertShader += vertexFunctionDecls[i] + "\n";
    vertShader += "\n\n\n";

    fragShader += "\n\n//Helper functions.";
    for (unsigned int i = 0; i < fragmentFunctionDecls.size(); ++i)
        fragShader += fragmentFunctionDecls[i] + "\n";
    if (useLighting)
    {
        fragShader += "\nvec3 getBrightness(vec3 surfaceNormal, vec3 camToFragNormal, float specular, float specularIntensity, DirectionalLight lightDir)\n\
                      {\n\
                          float dotted = max(dot(-surfaceNormal, lightDir.Dir), 0.0);\n\
                          \n\
                          vec3 fragToCam = -camToFragNormal;\n\
                          vec3 lightReflect = normalize(reflect(lightDir.Dir, surfaceNormal));\n\
                          \n\
                          float specFactor = max(0.0, dot(fragToCam, lightReflect));\n\
                          specFactor = pow(specFactor, specularIntensity);\n\
                          \n\
                          return lightDir.Ambient + (lightDir.Diffuse * dotted) + (specular * specFactor);\n\
                      }\n";
    }
    fragShader += "\n\n\n";


    //Set up the main() functions.
    //TODO: For vertex shader: calculate the screen depth of the vertex from 0 - 1 after transforming the position into clip space, pass it to the vertex shader, and expose it as a DataNode.
    vertShader += "                                                                                             \n\
        void main()                                                                                             \n\
        {                                                                                                       \n\
            //Compute outputs.                                                                                  \n\
            " + vertexCode + "                                                                                  \n\
                                                                                                                \n\
                                                                                                                \n\
            //Compute shader outputs.                                                                           \n\
            " + MaterialConstants::OutColor + " = " + MaterialConstants::InColor + ";                           \n\
            " + MaterialConstants::OutNormal + " = (" + MaterialConstants::WorldMatName +
                                                  " * vec4(" + MaterialConstants::InNormal + ", 0.0)).xyz;      \n\
            " + MaterialConstants::OutUV + " = " + MaterialConstants::InUV + ";                                 \n\
                                                                                                                \n\
            //Compute world position.                                                                           \n\
            vec4 pos = " + MaterialConstants::InPos + ";                                                        \n\
            pos += " + channels[RC::RC_ObjectVertexOffset].GetValue() + ";                                      \n\
            pos = (" + MaterialConstants::WorldMatName + " * vec4(pos, 1.0)" + ");                              \n\
            pos += " + channels[RC::RC_WorldVertexOffset].GetValue() + ";                                       \n\
                                                                                                                \n\
            " + MaterialConstants::OutPos + " = pos;                                                            \n\
                                                                                                                \n\
            gl_Position = " + MaterialConstants::WVPMatName + " * " + MaterialConstants::InPos + ";             \n\
        }";
    //TODO: Finish.
    fragShader += "                                                                                             \n\
        ";


    outVShader += vertShader;
    outFShader += fragShader;


    //Finalize the uniforms.
    //TODO: Implement.

    //Create the shaders.
    //TODO: Implement.

    //Get the uniform locations.
    //TODO: Implement.
}