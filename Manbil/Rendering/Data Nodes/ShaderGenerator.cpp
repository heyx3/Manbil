#include "ShaderGenerator.h"

#include "../Basic Rendering/Material.h"
#include "../../DebugAssist.h"
#include "SerializedMaterial.h"
#include "DataNode.h"


typedef ShaderGenerator SG;


std::string SG::GenerateUniformDeclarations(const UniformList& dict)
{
    std::string decls = "";
    for (auto it = dict.begin(); it != dict.end(); ++it)
    {
        it->GetDeclaration(decls);
        decls += "\n";
    }
    return decls;
}


SG::GeneratedMaterial SG::GenerateMaterial(const SerializedMaterial& matData,
                                           UniformDictionary& outUniforms, BlendMode blendMode)
{
    std::string vs, fs;
    UniformList uList;
    std::string error = GenerateVertFragShaders(vs, fs, matData, uList);

    //Make sure the shaders were generated successfully.
    if (!error.empty())
    {
        return GeneratedMaterial(std::string("Error generating vertex/fragment shaders: ") + error);
    }

    for (unsigned int i = 0; i < uList.size(); ++i)
        outUniforms[uList[i].Name] = uList[i];


    std::string geo = "";
    if (matData.GeoShader.IsValidData())
    {
        geo = GenerateGeometryShader(matData);
    }
    if (geo.find("ERROR:") != std::string::npos)
    {
        return GeneratedMaterial(std::string("Error generating geometry shader: '") + geo + "'");
    }

    //Add the geometry shader uniforms to the collection of all uniforms.
    if (matData.GeoShader.IsValidData())
    {
        Uniform::AddUniforms(matData.GeoShader.Params, outUniforms, true);
    }


    //Attempt to create the material.

    Material* mat = new Material(vs, fs, outUniforms, matData.VertexInputs, blendMode, error, geo);
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

std::string SG::GenerateGeometryShader(const SerializedMaterial& matData)
{
    DataNode::SetCurrentMaterial(&matData);
    const std::vector<ShaderOutput>& vertexOutputs = matData.MaterialOuts.VertexOutputs;
    const GeoShaderData& data = matData.GeoShader;

    if (!data.IsValidData())
    {
        return "ERROR: Invalid GeoShaderData instance.";
    }


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
                                        const SerializedMaterial& matData,
                                        UniformList& outUniforms)
{
    DataNode::SetCurrentMaterial(&matData);

    const RenderIOAttributes& vertexIns = matData.VertexInputs;
    const MaterialOutputs& matOuts = matData.MaterialOuts;
    const GeoShaderData& geoShaderData = matData.GeoShader;


    bool useGeoShader = geoShaderData.IsValidData();

    //First make sure all shader outputs are valid sizes.
    //Note that vertex outputs can be any size, so they don't need to be tested.
    if (matOuts.VertexPosOutput.GetSize() != 4)
    {
        return "Vertex pos output value must be size 4, but it is size " +
                    std::to_string(matOuts.VertexPosOutput.GetSize());
    }
    DataNode::CurrentShader = SH_FRAGMENT;

    //Now make sure all shader outputs have valid inputs.
    std::string outputInfo = "";
    DataNode* currentNode = 0;
    try
    {
        DataNode::CurrentShader = SH_VERTEX;
        if (!matOuts.VertexPosOutput.IsConstant())
        {
            outputInfo = "vertex pos output node";
            currentNode = matOuts.VertexPosOutput.GetNode();
            if (currentNode == 0)
            {
                return "Vertex position output node '" + matOuts.VertexPosOutput.GetNonConstantValue() +
                            "' doesn't exist!";
            }
            currentNode->AssertAllInputsValid();

            for (unsigned int i = 0; i < matOuts.VertexOutputs.size(); ++i)
            {
                if (!matOuts.VertexOutputs[i].Value.IsConstant())
                {
                    outputInfo = "vertex shader output #" + std::to_string(i + 1);
                    currentNode = matOuts.VertexOutputs[i].Value.GetNode();
                    if (currentNode == 0)
                    {
                        return "Vertex shader output node '" +
                                    matOuts.VertexOutputs[i].Value.GetNonConstantValue() +
                                    "' doesn't exist!";
                    }
                    currentNode->AssertAllInputsValid();
                }
            }

            DataNode::CurrentShader = SH_FRAGMENT;
            for (unsigned int i = 0; i < matOuts.FragmentOutputs.size(); ++i)
            {
                if (!matOuts.FragmentOutputs[i].Value.IsConstant())
                {
                    outputInfo = "fragment shader output #" + std::to_string(i + 1);
                    currentNode = matOuts.FragmentOutputs[i].Value.GetNode();
                    if (currentNode == 0)
                    {
                        return "Fragment shader output node '" +
                                    matOuts.FragmentOutputs[i].Value.GetNonConstantValue() +
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
        matOuts.VertexPosOutput.GetNode()->SetFlags(vertFlags,
                                                    matOuts.VertexPosOutput.GetNonConstantOutputIndex());
    }
    catch (int ex)
    {
        if (ex != DataNode::EXCEPTION_ASSERT_FAILED)
        {
            return "Unexpected exception writing usage flags for vertex pos output node '" +
                       matOuts.VertexPosOutput.GetNonConstantValue() + ": " + std::to_string(ex);
        }
    }
    for (unsigned int i = 0; i < matOuts.VertexOutputs.size(); ++i)
    {
        if (matOuts.VertexOutputs[i].Value.IsConstant())
        {
            continue;
        }

        const ShaderOutput& vertOut = matOuts.VertexOutputs[i];

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
    for (unsigned int i = 0; i < matOuts.FragmentOutputs.size(); ++i)
    {
        if (matOuts.FragmentOutputs[i].Value.IsConstant()) continue;

        const ShaderOutput& fragOut = matOuts.FragmentOutputs[i];

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
    for (unsigned int vertOut = 0; vertOut < matOuts.VertexOutputs.size(); ++vertOut)
    {
        vertOutput += "out " + VectorF(matOuts.VertexOutputs[vertOut].Value.GetSize(), 0).GetGLSLType() +
                           " " + matOuts.VertexOutputs[vertOut].Name + ";\n";

        //If not using a geometry shader, the fragment inputs are the same as the vertex outputs.
        if (!useGeoShader)
        {
            fragInput += "in " + VectorF(matOuts.VertexOutputs[vertOut].Value.GetSize(), 0).GetGLSLType() +
                             " " + matOuts.VertexOutputs[vertOut].Name + ";\n";
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
    for (unsigned int fragOut = 0; fragOut < matOuts.FragmentOutputs.size(); ++fragOut)
    {
        fragOutput += "out " + VectorF::GetGLSLType(matOuts.FragmentOutputs[fragOut].Value.GetSize()) +
                      " " + matOuts.FragmentOutputs[fragOut].Name + ";\n";
    }


    //Generate the headers for each shader.
    std::string vertShader = MaterialConstants::GetVertexHeader(vertOutput, vertexIns, vertFlags),
                fragShader = MaterialConstants::GetFragmentHeader(fragInput, fragOutput, fragFlags);


    //Generate uniforms, functions, and output calculations.

    UniformList vertexUniformDict, fragmentUniformDict;
    std::vector<std::string> vertexFunctionDecls, fragmentFunctionDecls;
    std::string vertexCode, fragmentCode;
    std::vector<const DataNode*> vertexUniforms, fragmentUniforms,
                           vertexFunctions, fragmentFunctions,
                           vertexCodes, fragmentCodes;

    DataNode::CurrentShader = SH_VERTEX;
    for (int vertOut = -1; vertOut < (int)matOuts.VertexOutputs.size(); ++vertOut)
    {
        const DataLine& inDat = (vertOut < 0 ?
                                     matOuts.VertexPosOutput :
                                     matOuts.VertexOutputs[(unsigned int)vertOut].Value);
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
    for (unsigned int fragOut = 0; fragOut < matOuts.FragmentOutputs.size(); ++fragOut)
    {
        const DataLine& inDat = matOuts.FragmentOutputs[fragOut].Value;

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
    if (vertexUniformDict.size() > 0)
    {
        vertShader += GenerateUniformDeclarations(vertexUniformDict);
    }
    if (fragmentUniformDict.size() > 0)
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
    for (unsigned int vertOut = 0; vertOut < matOuts.VertexOutputs.size(); ++vertOut)
    {
        vertShader += "\t" + matOuts.VertexOutputs[vertOut].Name + " = " +
                           matOuts.VertexOutputs[vertOut].Value.GetValue() + ";\n";
    }
    vertShader +=
"                                                                                                        \n\
    gl_Position = " + matOuts.VertexPosOutput.GetValue() + ";              \n\
}";

    DataNode::CurrentShader = SH_FRAGMENT;
    fragShader += "                                                                                     \n\
void main()                                                                                             \n\
{                                                                                                       \n\
    //Compute outputs.                                                                                  \n\
    " + fragmentCode + "                                                                                \n\
                                                                                                        \n";

    //Now output the final color(s).
    for (unsigned int fragOut = 0; fragOut < matOuts.FragmentOutputs.size(); ++fragOut)
    {
        fragShader += "\t" + matOuts.FragmentOutputs[fragOut].Name + " = " +
                        matOuts.FragmentOutputs[fragOut].Value.GetValue() + ";\n";
    }
    fragShader += "}";


    //Output the generated shaders.
    outVShader += vertShader;
    outFShader += fragShader;


    //Add all params to the output list, making sure there are no duplicates.
    for (unsigned int i = 0; i < vertexUniformDict.size(); ++i)
    {
        bool existsAlready = false;
        for (unsigned int j = 0; j < outUniforms.size(); ++j)
        {
            if (outUniforms[j].Name == vertexUniformDict[i].Name)
            {
                existsAlready = true;
                break;
            }
        }

        if (!existsAlready)
        {
            outUniforms.push_back(vertexUniformDict[i]);
        }
    }
    for (unsigned int i = 0; i < fragmentUniformDict.size(); ++i)
    {
        bool existsAlready = false;
        for (unsigned int j = 0; j < outUniforms.size(); ++j)
        {
            if (outUniforms[j].Name == fragmentUniformDict[i].Name)
            {
                existsAlready = true;
                break;
            }
        }

        if (!existsAlready)
        {
            outUniforms.push_back(fragmentUniformDict[i]);
        }
    }


    return "";
}