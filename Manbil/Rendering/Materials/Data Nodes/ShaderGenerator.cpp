#include "ShaderGenerator.h"

#include "DataNodeIncludes.h"
#include "../../../Math/Higher Math/Lighting.h"
#include "../../../Material.h"


typedef ShaderGenerator SG;
typedef RenderingChannels RC;
typedef ShaderHandler::Shaders Shaders;
typedef std::unordered_map<RenderingChannels, DataLine> RenderChannels;


unsigned int SG::GetChannelInputSize(RC channel)
{
    switch (channel)
    {
        case RC::RC_Diffuse:
        case RC::RC_Normal:
        case RC::RC_ObjectVertexOffset:
        //case RC::RC_WorldVertexOffset:
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
        //case RC::RC_WorldVertexOffset:
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
    outChannels.insert(outChannels.end(), RC::RC_Opacity);
    outChannels.insert(outChannels.end(), RC::RC_ObjectVertexOffset);
    //outChannels.insert(outChannels.end(), RC::RC_WorldVertexOffset);

    if (useLighting)
    {
        outChannels.insert(outChannels.end(), RC::RC_Normal);
        outChannels.insert(outChannels.end(), RC::RC_Specular);
        outChannels.insert(outChannels.end(), RC::RC_SpecularIntensity);
    }

    if (IsModeTransparent(mode))
    {
        outChannels.insert(outChannels.end(), RC::RC_Distortion);
    }
}

void SG::RemoveUnusedChannels(RenderChannels & channels, RenderingModes mode, bool useLighting, const LightSettings & settings)
{
    //Because of the nature of this algorithm, we have to ensure the first channel is valid.
    while (channels.size() > 0 && !IsChannelUsed(channels.begin()->first, mode, useLighting, settings))
        channels.erase(channels.begin());

    //Remove all channels that aren't used.
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        if (!IsChannelUsed(iterator->first, mode, useLighting, settings))
        {
            channels.erase(iterator);
            iterator = channels.begin();//Dictionaries aren't in any kind of numerical order, so start the search over.
        }
    }
}
void SG::AddMissingChannels(RenderChannels & channels, RenderingModes mode, bool useLighting, const LightSettings & settings)
{
    std::vector<RC> validChannels;
    GetUsedChannels(mode, useLighting, settings, validChannels);
    for (unsigned int i = 0; i < validChannels.size(); ++i)
    {
        if (channels.find(validChannels[i]) == channels.end())
        {
            switch (validChannels[i])
            {
            case RC::RC_Diffuse:
                channels[RC::RC_Diffuse] = DataLine(DataNodePtr(new ObjectColorNode()), 0);
                break;
            case RC::RC_DiffuseIntensity:
                channels[RC::RC_DiffuseIntensity] = DataLine(VectorF(1.0f));
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
                //case RC::RC_WorldVertexOffset:
                //    channels[RC::RC_WorldVertexOffset] = DataLine(Vector3f(0.0f, 0.0f, 0.0f));
                //    break;

            default: assert(false);
            }
        }
    }
}

void SG::GenerateShaders(std::string & outVShader, std::string & outFShader, UniformDictionary & outUniforms,
                         RenderingModes mode, bool useLighting, const LightSettings & settings,
                         std::unordered_map<RenderingChannels, DataLine> & channels)
{
    RemoveUnusedChannels(channels, mode, useLighting, settings);
    AddMissingChannels(channels, mode, useLighting, settings);

    //First, make sure the channels are all correctly set up.
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        //Make sure each channel has a valid input data line size.
        assert(GetChannelInputSize(iterator->first) == iterator->second.GetDataLineSize());
    }


    //Create the shaders.

    std::string vertShader = MaterialConstants::GetVertexHeader(useLighting),
                fragShader = MaterialConstants::GetFragmentHeader(useLighting);


    //Collect data node uniforms, functions, and output.

    std::vector<RC> validChannels;
    GetUsedChannels(mode, useLighting, settings, validChannels);

    UniformDictionary vertexUniformDict, fragmentUniformDict;
    std::vector<std::string> vertexFunctionDecls, fragmentFunctionDecls;
    std::string vertexCode, fragmentCode;
    std::vector<unsigned int> vertexUniforms, fragmentUniforms,
                              vertexFunctions, fragmentFunctions,
                              vertexCodes, fragmentCodes;

    for (unsigned int i = 0; i < validChannels.size(); ++i)
    {
        const DataLine & data = channels[validChannels[i]];
        if (!data.IsConstant())
        {
            std::string oldVCode = vertexCode,
                        oldFCode = fragmentCode;

            switch (validChannels[i])
            {
                //case RC::RC_WorldVertexOffset:
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
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_ColorName] = UniformValueF(Vector3f(1.0f, 1.0f, 1.0f), "dirLight.Col");
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_DirName] = UniformValueF(Vector3f(1.0f, 1.0f, -1.0f).Normalized(), "dirLight.Dir");
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName] = UniformValueF(0.2f, "dirLight.Ambient");
        fragmentUniformDict.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName] = UniformValueF(0.8f, "dirLight.Diffuse");
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
    //TODO: Normal channel is relative to a surface pointing straight up.
    //TODO: Create a separate rendering mode for distortion -- it is opaque, and blending is manually done in the fragment shader using a sampler for the rendered world.
    //TODO: For vertex shader: calculate the screen depth of the vertex from 0 - 1 after transforming the position into clip space, pass it to the vertex shader, and expose it as a DataNode.
    vertShader += "\n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute shader out data (except position, which will be changed by a channel).                    \n\
    " + MaterialConstants::OutObjNormal + " = " + MaterialConstants::InObjNormal + ";                   \n\
    " + MaterialConstants::OutWorldNormal + " = normalize(" + MaterialConstants::WorldMatName + " * " +
                                                          "vec4(" + MaterialConstants::InObjNormal + ", 0.0)).xyz;\n\
    vec3 " + MaterialConstants::InWorldNormal + " = " + MaterialConstants::OutWorldNormal + ";          \n\
    " + MaterialConstants::OutUV + " = " + MaterialConstants::InUV + ";                                 \n\
    " + MaterialConstants::OutColor + " = " + MaterialConstants::InColor + ";                           \n\
                                                                                                        \n\
    //Compute outputs.                                                                                  \n\
    " + vertexCode + "                                                                                  \n\
                                                                                                        \n\
                                                                                                        \n\
    //Compute position out data.                                                                        \n\
    " + MaterialConstants::OutObjPos + " = " + MaterialConstants::InObjPos + " + " + channels[RC::RC_ObjectVertexOffset].GetValue() + ";\n\
    vec4 worldPos = (" + MaterialConstants::WorldMatName + " * vec4(" + MaterialConstants::OutObjPos + ", 1.0));                        \n\
    " + //pos3 += " + "channels[RC::RC_WorldVertexOffset].GetValue() + ;                                \n
    "                                                                                                   \n\
    " + MaterialConstants::OutWorldPos + " = worldPos.xyz / worldPos.w;                                 \n\
                                                                                                        \n\
    gl_Position = " + MaterialConstants::WVPMatName + " * vec4(" + MaterialConstants::OutObjPos + ", 1.0);  \n\
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
     diffuseCol *= getLight(normalize(normalVal), " +
                           "normalize(" + MaterialConstants::CameraPosName + " - " +
                                           MaterialConstants::OutWorldPos + ")" +
                            ", " + channels[RC::RC_Specular].GetValue() +
                            ", " + channels[RC::RC_SpecularIntensity].GetValue() +
                            ", " + MaterialConstants::DirectionalLightName + ");              \n\n";
        }

    //Now output the final color.
    fragShader +=
    "\t" + MaterialConstants::FinalOutColor + " = vec4(diffuseCol, " + channels[RC::RC_Opacity].GetValue() + ");\n\
}";


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

Material * ShaderGenerator::GenerateMaterial(std::unordered_map<RenderingChannels, DataLine> & channels,
                                             UniformDictionary & uniforms,
                                             RenderingModes mode, bool useLighting, const LightSettings & settings)
{
    std::string vs, fs;
    GenerateShaders(vs, fs, uniforms, mode, useLighting, settings, channels);
    
    return new Material(vs, fs, uniforms, mode, useLighting, settings);
}