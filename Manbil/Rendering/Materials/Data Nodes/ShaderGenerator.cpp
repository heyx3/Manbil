#include "ShaderGenerator.h"

#include "../../../Math/Higher Math/Lighting.h"
#include "../../../Material.h"
#include "../../../DebugAssist.h"
#include "SerializedMaterial.h"
#include "DataNode.h"


typedef ShaderGenerator SG;
typedef ShaderHandler::Shaders Shaders;


std::string SG::GenerateUniformDeclarations(const UniformDictionary & dict)
{
    std::string decls;

    //First define any subroutines so that they are separate from actual uniform declarations.
    if (dict.SubroutineUniforms.size() > 0)
    {
        std::vector<SubroutineDefinition*> usedDefinitions;

        decls += "//Subroutine definitions.\n";
        for (auto iterator = dict.SubroutineUniforms.begin(); iterator != dict.SubroutineUniforms.end(); ++iterator)
        {
            if (std::find(usedDefinitions.begin(), usedDefinitions.end(), iterator->second.Definition.get()) == usedDefinitions.end())
            {
                decls += iterator->second.Definition->GetDefinition() + "\n";
                usedDefinitions.insert(usedDefinitions.end(), iterator->second.Definition.get());
            }
        }
        decls += "\n";
    }

    //Now handle the rest of the uniform types.
    decls += "//Uniform declarations.\n";
    for (auto iterator = dict.FloatUniforms.begin(); iterator != dict.FloatUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.FloatArrayUniforms.begin(); iterator != dict.FloatArrayUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.IntUniforms.begin(); iterator != dict.IntUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.IntArrayUniforms.begin(); iterator != dict.IntArrayUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.MatrixUniforms.begin(); iterator != dict.MatrixUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.Texture2DUniforms.begin(); iterator != dict.Texture2DUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.Texture3DUniforms.begin(); iterator != dict.Texture3DUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.TextureCubemapUniforms.begin(); iterator != dict.TextureCubemapUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.SubroutineUniforms.begin(); iterator != dict.SubroutineUniforms.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";

    return decls + "\n";
}


SG::GeneratedMaterial SG::GenerateMaterial(UniformDictionary & outUniforms, RenderingModes mode)
{
    const ShaderInOutAttributes & vertexIns = DataNode::VertexIns;
    GeoShaderData & geometryShader = DataNode::GeometryShader;

    std::string vs, fs;
    std::string error = GenerateVertFragShaders(vs, fs, outUniforms, mode);

    std::string geo = "";
    if (geometryShader.IsValidData())
        geo = GenerateGeometryShader();

    //Make sure the shaders were generated successfully.
    if (!error.empty())
        return GeneratedMaterial(std::string("Error generating vertex/fragment shaders: ") + error);
    if (geo.find("ERROR:") != std::string::npos)
        return GeneratedMaterial(std::string("Error generating geometry shader: '") + geo + "'");

    if (geometryShader.IsValidData())
        outUniforms.AddUniforms(geometryShader.Params, true);


    //Attempt to create the material.

    Material * mat = new Material(vs, fs, outUniforms, vertexIns, mode, geo);
    GeneratedMaterial genMat(mat->GetErrorMsg());

    if (genMat.ErrorMessage.empty())
        genMat.Mat = mat;
    else delete mat;

    return genMat;
}

std::string SG::GenerateGeometryShader(void)
{
    const std::vector<ShaderOutput> & vertexOutputs = DataNode::MaterialOuts.VertexOutputs;
    const GeoShaderData & data = DataNode::GeometryShader;

    if (!data.IsValidData()) return "ERROR: Invalid GeoShaderData instance.";


    //First generate the vertex inputs.
    std::string code = "//Inputs from Vertex Shader.\n";
    for (unsigned int i = 0; i < vertexOutputs.size(); ++i)
    {
        unsigned int size = vertexOutputs[i].Value.GetSize();
        if (size == 0)
        {
            return "ERROR: size of vertex output index " + std::to_string(i) +
                       ", '" + vertexOutputs[i].Value.GetNonConstantValue() + "', has a size of 0.";\
        }

        code += "in " + VectorF(size).GetGLSLType() + " " + vertexOutputs[i].Name + "[" + std::to_string(PrimitiveTypeToNVertices(data.InputPrimitive)) + "];\n";
    }
    code += "\n";


    //Next generate the geometry outputs.
    code += "//Shader outputs.\n";
    unsigned int n = data.OutputTypes.GetNumbAttributes();
    for (unsigned int i = 0; i < n; ++i)
    {
        code += "out " + VectorF(data.OutputTypes.GetAttributeSize(i)).GetGLSLType() + " " + data.OutputTypes.GetAttributeName(i) + ";\n";
    }

    //Now create the header.
    code = MaterialConstants::GetGeometryHeader(code, data.InputPrimitive, data.OutputPrimitive, data.MaxVertices, data.UsageFlags);

    //Now generate the uniforms.
    code += "//Uniforms.\n";
    code += GenerateUniformDeclarations(data.Params) + "\n";

    //Finally, add the actual functions.
    code += "\n\n" + data.ShaderCode;

    return code;
}

std::string SG::GenerateVertFragShaders(std::string & outVShader, std::string & outFShader,
                                        UniformDictionary & outUniforms, RenderingModes mode)
{
    const ShaderInOutAttributes & vertexIns = DataNode::VertexIns;
    const MaterialOutputs & matData = DataNode::MaterialOuts;
    const GeoShaderData & geoShaderData = DataNode::GeometryShader;


    bool useGeoShader = geoShaderData.IsValidData();

    //First make sure all shader outputs are valid sizes.
    if (!matData.VertexPosOutput.GetSize() != 4)
        return "Vertex pos output value must be size 4, but it is size " + std::to_string(matData.VertexPosOutput.GetSize());
    for (unsigned int i = 0; i < matData.FragmentOutputs.size(); ++i)
    {
        if (matData.FragmentOutputs[i].Value.GetSize() != 4)
        {
            return "Fragment output index " + std::to_string(i) +
                        " must be size 4, but it is size " +
                        std::to_string(matData.FragmentOutputs[i].Value.GetSize());
        }
    }

    //Get information about what external data each shader uses.
    MaterialUsageFlags vertFlags;
    DataNode::CurrentShader = Shaders::SH_Vertex_Shader;
    for (unsigned int i = 0; i < matData.VertexOutputs.size(); ++i)
    {
        if (matData.VertexOutputs[i].Value.IsConstant()) continue;

        const ShaderOutput & vertOut = matData.VertexOutputs[i];

        try
        {
            if (vertOut.Value.GetNode() == 0)
                return "Vertex output '" + vertOut.Name + "'s node value '" +
                            vertOut.Value.GetNonConstantValue() + "' doesn't exist!";

            vertOut.Value.GetNode()->SetFlags(vertFlags, vertOut.Value.GetNonConstantOutputIndex());
        }
        catch (int ex)
        {
            if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
                return "Unexpected exception writing usage flags for vertex input '" + vertOut.Name +
                            "' (the only expected one is DataNode::EXCEPTION_ASSERT_FAILED, " +
                            std::to_string(DataNode::EXCEPTION_ASSERT_FAILED) + "): " + std::to_string(ex);
            return "Error setting flags for vertex output '" + vertOut.Name +
                        "', node '" + vertOut.Value.GetNonConstantValue() +
                        ": " + vertOut.Value.GetNode()->GetError();
        }
    }
    MaterialUsageFlags fragFlags;
    DataNode::CurrentShader = Shaders::SH_Fragment_Shader;
    for (unsigned int i = 0; i < matData.FragmentOutputs.size(); ++i)
    {
        if (matData.FragmentOutputs[i].Value.IsConstant()) continue;

        const ShaderOutput & fragOut = matData.FragmentOutputs[i];

        try
        {
            if (fragOut.Value.GetNode() == 0)
                return "Fragment output '" + fragOut.Name + "'s node value '" +
                            fragOut.Value.GetNonConstantValue() + "' doesn't exist!";

            fragOut.Value.GetNode()->SetFlags(fragFlags, fragOut.Value.GetNonConstantOutputIndex());
        }
        catch (int ex)
        {
            if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
                return "Unexpected exception writing usage flags for fragment input '" + fragOut.Name +
                            "' (the only expected one is DataNode::EXCEPTION_ASSERT_FAILED, " +
                            std::to_string(DataNode::EXCEPTION_ASSERT_FAILED) + "): " + std::to_string(ex);

            return "Error setting flags for fragment output '" + fragOut.Name +
                "', node '" + fragOut.Value.GetNonConstantValue() +
                ": " + fragOut.Value.GetNode()->GetError();
        }
    }
    //PRIORITY: Finish.

    //Generate the vertex output/fragment input declarations.
    std::string vertOutput, fragInput;
    for (auto iterator = channels.begin(); iterator != channels.end(); ++iterator)
    {
        if (IsChannelVertexOutput(iterator->first, false))
        {
            unsigned int vOutputIndex = GetVertexOutputNumber(iterator->first);
            std::string name = MaterialConstants::VertexOutNameBase + std::to_string(vOutputIndex);

            vertOutput += "out " + VectorF(iterator->second.GetDataLineSize()).GetGLSLType() + " " + name + ";\n";

            //If there is no geometry shader, the vertex outputs exactly match the fragment inputs.
            //Otherwise, the fragment inputs will exactly match the geometry shader outputs (this is done right after this "for" loop).
            if (!useGeoShader)
                fragInput += "in " + VectorF(iterator->second.GetDataLineSize()).GetGLSLType() + " " + name + ";\n";
        }
    }
    if (useGeoShader)
    {
        unsigned int numb = geoShaderData.OutputTypes.GetNumbOutputs();
        for (unsigned int i = 0; i < numb; ++i)
            fragInput += "in " + VectorF(geoShaderData.OutputTypes.OutputSizes[i]).GetGLSLType() + " " + geoShaderData.OutputTypes.OutputNames[i] + ";\n";
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
            fragOutput += "layout (location = " + std::to_string(numb - 1) + ") out vec4 " +
                          MaterialConstants::FragmentOutName + std::to_string(numb) + ";\n";
            outputNames[iterator->first] = MaterialConstants::FragmentOutName + std::to_string(numb);
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
    if (vertexUniformDict.GetNumbUniforms() > 0)
        vertShader += GenerateUniformDeclarations(vertexUniformDict);
    if (fragmentUniformDict.GetNumbUniforms() > 0)
        fragShader += GenerateUniformDeclarations(fragmentUniformDict);


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
        fragShader += "\n\n\n";
    }


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
    gl_Position = " + channels[RenderingChannels::RC_VertexPosOutput].GetValue() + ";              \n\
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
    outUniforms.AddUniforms(fragmentUniformDict, true);
    outUniforms.AddUniforms(vertexUniformDict, true);


    return "";
}