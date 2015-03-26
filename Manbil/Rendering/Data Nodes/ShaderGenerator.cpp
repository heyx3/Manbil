#include "ShaderGenerator.h"

#include "../Basic Rendering/Material.h"
#include "../../DebugAssist.h"
#include "SerializedMaterial.h"
#include "DataNode.h"


typedef ShaderGenerator SG;


std::string SG::GenerateUniformDeclarations(const UniformDictionary& dict)
{
    std::string decls;

    //First define any subroutines so that they are separate from actual uniform declarations.
    if (dict.Subroutines.size() > 0)
    {
        std::vector<SubroutineDefinition*> usedDefinitions;

        decls += "//Subroutine definitions.\n";
        for (auto iterator = dict.Subroutines.begin(); iterator != dict.Subroutines.end(); ++iterator)
        {
            if (std::find(usedDefinitions.begin(), usedDefinitions.end(),
                          iterator->second.Definition.get()) == usedDefinitions.end())
            {
                decls += iterator->second.Definition->GetDefinition() + "\n";
                usedDefinitions.insert(usedDefinitions.end(), iterator->second.Definition.get());
            }
        }
        decls += "\n";
    }

    //Now handle the rest of the uniform types.
    decls += "//Uniform declarations.\n";
    for (auto iterator = dict.Floats.begin(); iterator != dict.Floats.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.FloatArrays.begin(); iterator != dict.FloatArrays.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.Ints.begin(); iterator != dict.Ints.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.IntArrays.begin(); iterator != dict.IntArrays.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.Matrices.begin(); iterator != dict.Matrices.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.Texture2Ds.begin(); iterator != dict.Texture2Ds.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.Texture3Ds.begin(); iterator != dict.Texture3Ds.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.TextureCubemaps.begin(); iterator != dict.TextureCubemaps.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";
    for (auto iterator = dict.Subroutines.begin(); iterator != dict.Subroutines.end(); ++iterator)
        decls += iterator->second.GetDeclaration() + "\n";

    return decls + "\n";
}


SG::GeneratedMaterial SG::GenerateMaterial(UniformDictionary& outUniforms, BlendMode blendMode)
{
    const RenderIOAttributes& vertexIns = DataNode::VertexIns;
    GeoShaderData& geometryShader = DataNode::GeometryShader;

    std::string vs, fs;
    std::string error = GenerateVertFragShaders(vs, fs, outUniforms);

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

    Material* mat = new Material(vs, fs, outUniforms, vertexIns, blendMode, error, geo);
    GeneratedMaterial genMat(error);

    if (genMat.ErrorMessage.empty())
    {
        genMat.Mat = mat;
    }
    else
    {
        delete mat;
    }

    return genMat;
}

std::string SG::GenerateGeometryShader(void)
{
    const std::vector<ShaderOutput>& vertexOutputs = DataNode::MaterialOuts.VertexOutputs;
    const GeoShaderData& data = DataNode::GeometryShader;

    if (!data.IsValidData()) return "ERROR: Invalid GeoShaderData instance.";


    //First generate the vertex inputs.
    std::string code = "//Inputs from Vertex Shader.\n";
    for (unsigned int i = 0; i < vertexOutputs.size(); ++i)
    {
        unsigned int size = vertexOutputs[i].Value.GetSize();
        if (size == 0)
        {
            return "ERROR: size of vertex output index " + std::to_string(i) +
                       ", '" + vertexOutputs[i].Value.GetNonConstantValue() + "', has a size of 0.";
        }

        code += "in " + VectorF(size, 0).GetGLSLType() + " " + vertexOutputs[i].Name +
                    "[" + std::to_string(PrimitiveTypeToNVertices(data.InputPrimitive)) + "];\n";
    }
    code += "\n";


    //Next generate the geometry outputs.
    code += "//Shader outputs.\n";
    unsigned int n = data.OutputTypes.GetNumbAttributes();
    for (unsigned int i = 0; i < n; ++i)
    {
        code += "out " + VectorF(data.OutputTypes.GetAttribute(i).Size, 0).GetGLSLType() + " " +
                                    data.OutputTypes.GetAttribute(i).Name + ";\n";
    }

    //Now create the header.
    code = MaterialConstants::GetGeometryHeader(code, data.InputPrimitive, data.OutputPrimitive,
                                                data.MaxVertices, data.UsageFlags);

    //Now generate the uniforms.
    code += "//Uniforms.\n";
    code += GenerateUniformDeclarations(data.Params) + "\n";

    //Finally, add the actual functions.
    code += "\n\n" + data.ShaderCode;

    return code;
}

std::string SG::GenerateVertFragShaders(std::string& outVShader, std::string& outFShader,
                                        UniformDictionary& outUniforms)
{
    const RenderIOAttributes& vertexIns = DataNode::VertexIns;
    const MaterialOutputs& matData = DataNode::MaterialOuts;
    const GeoShaderData& geoShaderData = DataNode::GeometryShader;


    bool useGeoShader = geoShaderData.IsValidData();

    //First make sure all shader outputs are valid sizes.
    //Note that vertex outputs can be any size, so they don't need to be tested.
    if (matData.VertexPosOutput.GetSize() != 4)
    {
        return "Vertex pos output value must be size 4, but it is size " +
                    std::to_string(matData.VertexPosOutput.GetSize());
    }
    DataNode::CurrentShader = SH_FRAGMENT;

    //Now make sure all shader outputs have valid inputs.
    std::string outputInfo = "";
    DataNode* currentNode = 0;
    try
    {
        DataNode::CurrentShader = SH_VERTEX;
        if (!matData.VertexPosOutput.IsConstant())
        {
            outputInfo = "vertex pos output node";
            currentNode = matData.VertexPosOutput.GetNode();
            if (currentNode == 0)
            {
                return "Vertex position output node '" + matData.VertexPosOutput.GetNonConstantValue() +
                            "' doesn't exist!";
            }
            currentNode->AssertAllInputsValid();

            for (unsigned int i = 0; i < matData.VertexOutputs.size(); ++i)
            {
                if (!matData.VertexOutputs[i].Value.IsConstant())
                {
                    outputInfo = "vertex shader output #" + std::to_string(i + 1);
                    currentNode = matData.VertexOutputs[i].Value.GetNode();
                    if (currentNode == 0)
                    {
                        return "Vertex shader output node '" +
                                    matData.VertexOutputs[i].Value.GetNonConstantValue() +
                                    "' doesn't exist!";
                    }
                    currentNode->AssertAllInputsValid();
                }
            }

            DataNode::CurrentShader = SH_FRAGMENT;
            for (unsigned int i = 0; i < matData.FragmentOutputs.size(); ++i)
            {
                if (!matData.FragmentOutputs[i].Value.IsConstant())
                {
                    outputInfo = "fragment shader output #" + std::to_string(i + 1);
                    currentNode = matData.FragmentOutputs[i].Value.GetNode();
                    if (currentNode == 0)
                    {
                        return "Fragment shader output node '" +
                                    matData.FragmentOutputs[i].Value.GetNonConstantValue() +
                                    "' doesn't exist!";
                    }
                    currentNode->AssertAllInputsValid();
                }
            }
        }
    }
    catch (int ex)
    {
        assert(ex == DataNode::EXCEPTION_ASSERT_FAILED);
        assert(currentNode != 0);
        return "Error with inputs for " + outputInfo + ": " + currentNode->GetError();
    }

    //Get information about what external data each shader uses.
    MaterialUsageFlags vertFlags;
    DataNode::CurrentShader = SH_VERTEX;
    try
    {
        matData.VertexPosOutput.GetNode()->SetFlags(vertFlags,
                                                    matData.VertexPosOutput.GetNonConstantOutputIndex());
    }
    catch (int ex)
    {
        if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
        {
            return "Unexpected exception writing usage flags for vertex pos output node '" +
                       matData.VertexPosOutput.GetNonConstantValue() + ": " + std::to_string(ex);
        }
    }
    for (unsigned int i = 0; i < matData.VertexOutputs.size(); ++i)
    {
        if (matData.VertexOutputs[i].Value.IsConstant()) continue;

        const ShaderOutput& vertOut = matData.VertexOutputs[i];

        try
        {
            vertOut.Value.GetNode()->SetFlags(vertFlags, vertOut.Value.GetNonConstantOutputIndex());
        }
        catch (int ex)
        {
            if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
                return "Unexpected exception writing usage flags for vertex output '" + vertOut.Name +
                            "' (the only expected one is DataNode::EXCEPTION_ASSERT_FAILED, " +
                            std::to_string(DataNode::EXCEPTION_ASSERT_FAILED) + "): " + std::to_string(ex);
            return "Error setting flags for vertex output '" + vertOut.Name +
                        "', node '" + vertOut.Value.GetNonConstantValue() +
                        ": " + vertOut.Value.GetNode()->GetError();
        }
    }
    MaterialUsageFlags fragFlags;
    DataNode::CurrentShader = SH_FRAGMENT;
    for (unsigned int i = 0; i < matData.FragmentOutputs.size(); ++i)
    {
        if (matData.FragmentOutputs[i].Value.IsConstant()) continue;

        const ShaderOutput& fragOut = matData.FragmentOutputs[i];

        try
        {
            fragOut.Value.GetNode()->SetFlags(fragFlags, fragOut.Value.GetNonConstantOutputIndex());
        }
        catch (int ex)
        {
            if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
            {
                return "Unexpected exception writing usage flags for fragment input '" + fragOut.Name +
                            "' (the only expected one is DataNode::EXCEPTION_ASSERT_FAILED, " +
                            std::to_string(DataNode::EXCEPTION_ASSERT_FAILED) + "): " + std::to_string(ex);
            }

            return "Error setting flags for fragment output '" + fragOut.Name +
                        "', node '" + fragOut.Value.GetNonConstantValue() +
                        ": " + fragOut.Value.GetNode()->GetError();
        }
    }

    //Generate the vertex output/fragment input declarations.
    std::string vertOutput, fragInput;
    for (unsigned int vertOut = 0; vertOut < matData.VertexOutputs.size(); ++vertOut)
    {
        vertOutput += "out " + VectorF(matData.VertexOutputs[vertOut].Value.GetSize(), 0).GetGLSLType() +
                           " " + matData.VertexOutputs[vertOut].Name + ";\n";

        //If not using a geometry shader, the fragment inputs are the same as the vertex outputs.
        if (!useGeoShader)
        {
            fragInput += "in " + VectorF(matData.VertexOutputs[vertOut].Value.GetSize(), 0).GetGLSLType() +
                             " " + matData.VertexOutputs[vertOut].Name + ";\n";
        }
    }

    //If using a geometry shader, the fragment inputs are the same as the geometry outputs.
    if (useGeoShader)
    {
        for (unsigned int geoOut = 0; geoOut < geoShaderData.OutputTypes.GetNumbAttributes(); ++geoOut)
        {
            fragInput += "in " +
                          VectorF(geoShaderData.OutputTypes.GetAttribute(geoOut).Size, 0).GetGLSLType() +
                          " " + geoShaderData.OutputTypes.GetAttribute(geoOut).Name + ";\n";
        }
    }

    //Generate the fragment outputs.
    std::string fragOutput;
    for (unsigned int fragOut = 0; fragOut < matData.FragmentOutputs.size(); ++fragOut)
    {
        fragOutput += "out " + VectorF::GetGLSLType(matData.FragmentOutputs[fragOut].Value.GetSize()) +
                      " " + matData.FragmentOutputs[fragOut].Name + ";\n";
    }


    //Generate the headers for each shader.
    std::string vertShader = MaterialConstants::GetVertexHeader(vertOutput, vertexIns, vertFlags),
                fragShader = MaterialConstants::GetFragmentHeader(fragInput, fragOutput, fragFlags);


    //Generate uniforms, functions, and output calculations.

    UniformDictionary vertexUniformDict, fragmentUniformDict;
    std::vector<std::string> vertexFunctionDecls, fragmentFunctionDecls;
    std::string vertexCode, fragmentCode;
    std::vector<const DataNode*> vertexUniforms, fragmentUniforms,
                           vertexFunctions, fragmentFunctions,
                           vertexCodes, fragmentCodes;

    DataNode::CurrentShader = SH_VERTEX;
    for (int vertOut = -1; vertOut < (int)matData.VertexOutputs.size(); ++vertOut)
    {
        const DataLine& inDat = (vertOut < 0 ?
                                     matData.VertexPosOutput :
                                     matData.VertexOutputs[(unsigned int)vertOut].Value);
        if (!inDat.IsConstant())
        {
            DataNode* node = inDat.GetNode();

            //Track whether code was added.
            unsigned int oldCodeLength = vertexCode.size();

            try
            {
                node->GetParameterDeclarations(vertexUniformDict, vertexUniforms);
                node->GetFunctionDeclarations(vertexFunctionDecls, vertexFunctions);
                node->WriteOutputs(vertexCode, vertexCodes);
            }
            catch (int ex)
            {
                if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
                {
                    return "Unexpected exception in node '" + node->GetName() +
                                "': " + std::to_string(ex);
                }

                return "Error setting parameters, functions, and outputs for node '" + node->GetName() +
                            "': " + node->GetError();
            }

            //If code was added to the shader, add a line break for clarity.
            if (oldCodeLength != vertexCode.size())
            {
                vertexCode += "\n";
            }
        }
    }
    DataNode::CurrentShader = SH_FRAGMENT;
    for (unsigned int fragOut = 0; fragOut < matData.FragmentOutputs.size(); ++fragOut)
    {
        const DataLine& inDat = matData.FragmentOutputs[fragOut].Value;

        if (!inDat.IsConstant())
        {
            DataNode* node = inDat.GetNode();

            //Track whether code was added.
            unsigned int oldCodeLength = fragmentCode.size();

            try
            {
                node->GetParameterDeclarations(fragmentUniformDict, fragmentUniforms);
                node->GetFunctionDeclarations(fragmentFunctionDecls, fragmentFunctions);
                node->WriteOutputs(fragmentCode, fragmentCodes);
            }
            catch (int ex)
            {
                if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
                {
                    return "Unexpected exception in node '" + node->GetName() +
                               "': " + std::to_string(ex);
                }

                return "Error setting parameters, functions, and outputs for node '" +
                            node->GetName() + "': " + node->GetError();
            }

            //If code was added to the shader, add a line break for clarity.
            if (oldCodeLength != fragmentCode.size())
            {
                fragmentCode += "\n";
            }
        }
    }


    //Add in the uniforms to the shader code.
    if (vertexUniformDict.GetNumbUniforms() > 0)
    {
        vertShader += GenerateUniformDeclarations(vertexUniformDict);
    }
    if (fragmentUniformDict.GetNumbUniforms() > 0)
    {
        fragShader += GenerateUniformDeclarations(fragmentUniformDict);
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
        fragShader += "\n\n\n";
    }


    //Set up the main() functions.
    DataNode::CurrentShader = SH_VERTEX;
    vertShader += "\n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute outputs.                                                                                  \n\
    " + vertexCode + "                                                                                  \n\
                                                                                                        \n\
";
    for (unsigned int vertOut = 0; vertOut < matData.VertexOutputs.size(); ++vertOut)
    {
        vertShader += "\t" + matData.VertexOutputs[vertOut].Name + " = " +
                           matData.VertexOutputs[vertOut].Value.GetValue() + ";\n";
    }
    vertShader +=
"                                                                                                        \n\
    gl_Position = " + matData.VertexPosOutput.GetValue() + ";              \n\
}";

    DataNode::CurrentShader = SH_FRAGMENT;
    fragShader += "                                                                                     \n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute outputs.                                                                                  \n\
    " + fragmentCode + "                                                                                \n\
                                                                                                        \n";

    //Now output the final color(s).
    for (unsigned int fragOut = 0; fragOut < matData.FragmentOutputs.size(); ++fragOut)
    {
        fragShader += "\t" + matData.FragmentOutputs[fragOut].Name + " = " +
                        matData.FragmentOutputs[fragOut].Value.GetValue() + ";\n";
    }
    fragShader += "}";


    //Output the generated shaders.
    outVShader += vertShader;
    outFShader += fragShader;


    //Finalize the uniforms.
    outUniforms.AddUniforms(fragmentUniformDict, true);
    outUniforms.AddUniforms(vertexUniformDict, true);


    return "";
}