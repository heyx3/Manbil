#include "Material.h"

#include "Mesh.h"


Material::Material(const std::string & vs, const std::string & fs, UniformDictionary & dict, const ShaderInOutAttributes & attrs, RenderingModes m, std::string geoShader)
    : mode(m), attributes(attrs)
{
    ShaderHandler::CreateShaderProgram(shaderProg);

    bool useGeoShader = !geoShader.empty();


    RenderObjHandle vsO, fsO, gsO = 0;
    if (!ShaderHandler::CreateShader(shaderProg, vsO, vs.c_str(), ShaderHandler::Shaders::SH_Vertex_Shader))
    {
        errorMsg = std::string() + "Error creating vertex shader: " + ShaderHandler::GetErrorMessage();
        return;
    }
    if (!ShaderHandler::CreateShader(shaderProg, fsO, fs.c_str(), ShaderHandler::Shaders::SH_Fragment_Shader))
    {
        errorMsg = std::string() + "Error creating fragment shader: " + ShaderHandler::GetErrorMessage();
        return;
    }
    if (useGeoShader && !ShaderHandler::CreateShader(shaderProg, gsO, geoShader.c_str(), ShaderHandler::Shaders::SH_GeometryShader))
    {
        errorMsg = std::string() + "Error creating geometry shader: " + ShaderHandler::GetErrorMessage();
        return;
    }


    if (!ShaderHandler::FinalizeShaders(shaderProg))
    {
        errorMsg = std::string() + "Error finalizing material shaders: " + ShaderHandler::GetErrorMessage();
        return;
    }

    glDeleteShader(vsO);
    glDeleteShader(fsO);
    if (useGeoShader) glDeleteShader(gsO);


    //Get node uniforms.
    //TODO: Refactor into UniformDictionary.
    UniformLocation tempLoc;
    for (auto iterator = dict.FloatUniforms.begin(); iterator != dict.FloatUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.FloatUniforms.insert(uniforms.FloatUniforms.end(),
                                          UniformList::Uniform(iterator->first, tempLoc));
            dict.FloatUniforms[iterator->first].Location = tempLoc;
        }
    }
    for (auto iterator = dict.FloatArrayUniforms.begin(); iterator != dict.FloatArrayUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.FloatArrayUniforms.insert(uniforms.FloatArrayUniforms.end(),
                                               UniformList::Uniform(iterator->first, tempLoc));
            dict.FloatArrayUniforms[iterator->first].Location = tempLoc;
        }
    }
    for (auto iterator = dict.IntUniforms.begin(); iterator != dict.IntUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.IntUniforms.insert(uniforms.IntUniforms.end(),
                                        UniformList::Uniform(iterator->first, tempLoc));
            dict.IntUniforms[iterator->first].Location = tempLoc;
        }
    }
    for (auto iterator = dict.IntArrayUniforms.begin(); iterator != dict.IntArrayUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.IntArrayUniforms.insert(uniforms.IntArrayUniforms.end(),
                                             UniformList::Uniform(iterator->first, tempLoc));
            dict.IntArrayUniforms[iterator->first].Location = tempLoc;
        }
    }
    for (auto iterator = dict.MatrixUniforms.begin(); iterator != dict.MatrixUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.MatrixUniforms.insert(uniforms.MatrixUniforms.end(),
                                           UniformList::Uniform(iterator->first, tempLoc));
            dict.MatrixUniforms[iterator->first].Location = tempLoc;
        }
    }
    for (auto iterator = dict.Texture2DUniforms.begin(); iterator != dict.Texture2DUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.Texture2DUniforms.insert(uniforms.Texture2DUniforms.end(),
                                              UniformList::Uniform(iterator->first, tempLoc));
            dict.Texture2DUniforms[iterator->first].Location = tempLoc;
        }
    }
    for (auto iterator = dict.Texture3DUniforms.begin(); iterator != dict.Texture3DUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.Texture3DUniforms.insert(uniforms.Texture3DUniforms.end(),
                                              UniformList::Uniform(iterator->first, tempLoc));
            dict.Texture3DUniforms[iterator->first].Location = tempLoc;
        }
    }
    for (auto iterator = dict.TextureCubemapUniforms.begin(); iterator != dict.TextureCubemapUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.TextureCubemapUniforms.insert(uniforms.TextureCubemapUniforms.end(),
                                                   UniformList::Uniform(iterator->first, tempLoc));
            dict.TextureCubemapUniforms[iterator->first].Location = tempLoc;
        }
    }
    //Subroutines are a bit more complex to set up.
    for (auto iterator = dict.SubroutineUniforms.begin(); iterator != dict.SubroutineUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetSubroutineUniformLocation(shaderProg, iterator->second.Definition->Shader, iterator->first.c_str(), tempLoc))
        {
            uniforms.SubroutineUniforms.insert(uniforms.SubroutineUniforms.end(),
                                               UniformList::Uniform(iterator->first, tempLoc));

            UniformSubroutineValue & sb = dict.SubroutineUniforms[iterator->first];
            sb.Location = tempLoc;

            //Get the ID of each subroutine function.
            for (unsigned int i = 0; i < sb.PossibleValueIDs.size(); ++i)
                RenderDataHandler::GetSubroutineID(shaderProg, sb.Definition->Shader, sb.PossibleValues[i].c_str(), sb.PossibleValueIDs[i]);
        }
    }

    //Get built-in uniforms.
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::ElapsedTimeName.c_str(), timeL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraPosName.c_str(), camPosL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraForwardName.c_str(), camForwardL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraUpName.c_str(), camUpL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraSideName.c_str(), camSideL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraWidthName.c_str(), camWidthL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraHeightName.c_str(), camHeightL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraZNearName.c_str(), camZNearL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraZFarName.c_str(), camZFarL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraFovName.c_str(), camFovL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraOrthoMinName.c_str(), camOrthoMinL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraOrthoMaxName.c_str(), camOrthoMaxL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::WorldMatName.c_str(), worldMatL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::ViewMatName.c_str(), viewMatL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::ProjMatName.c_str(), projMatL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::WVPMatName.c_str(), wvpMatL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::ViewProjMatName.c_str(), viewProjMatL);
}

bool Material::Render(const RenderInfo & info, const std::vector<const Mesh*> & meshes, const UniformDictionary & params)
{
    ClearAllRenderingErrors();

    ShaderHandler::UseShader(shaderProg);

    //Set basic uniforms.
    //TODO: [NOTE: Not sure if this would actually help at all] Remove all these conditionals for simple uniforms (i.e. not the world/wvp transformation matrix) by creating an std::vector in the constructor that holds all the valid locations for built-in uniforms and how to compute them.
    float time = info.World->GetTotalElapsedSeconds();
    if (RenderDataHandler::UniformLocIsValid(timeL))
        RenderDataHandler::SetUniformValue(timeL, 1, &time);
    Vector3f camPos = info.Cam->GetPosition();
    if (RenderDataHandler::UniformLocIsValid(camPosL))
        RenderDataHandler::SetUniformValue(camPosL, 3, &camPos[0]);
    Vector3f camF = info.Cam->GetForward();
    if (RenderDataHandler::UniformLocIsValid(camForwardL))
        RenderDataHandler::SetUniformValue(camForwardL, 3, &camF[0]);
    Vector3f camU = info.Cam->GetUpward();
    if (RenderDataHandler::UniformLocIsValid(camUpL))
        RenderDataHandler::SetUniformValue(camUpL, 3, &camU[0]);
    Vector3f camS = info.Cam->GetSideways();
    if (RenderDataHandler::UniformLocIsValid(camSideL))
        RenderDataHandler::SetUniformValue(camSideL, 3, &camS[0]);
    if (RenderDataHandler::UniformLocIsValid(camWidthL))
        RenderDataHandler::SetUniformValue(camWidthL, 1, &info.Cam->PerspectiveInfo.Width);
    if (RenderDataHandler::UniformLocIsValid(camHeightL))
        RenderDataHandler::SetUniformValue(camHeightL, 1, &info.Cam->PerspectiveInfo.Height);
    if (RenderDataHandler::UniformLocIsValid(camZNearL))
        RenderDataHandler::SetUniformValue(camZNearL, 1, &info.Cam->PerspectiveInfo.zNear);
    if (RenderDataHandler::UniformLocIsValid(camZFarL))
        RenderDataHandler::SetUniformValue(camZFarL, 1, &info.Cam->PerspectiveInfo.zFar);
    if (RenderDataHandler::UniformLocIsValid(camFovL))
        RenderDataHandler::SetUniformValue(camFovL, 1, &info.Cam->PerspectiveInfo.FOV);
    if (RenderDataHandler::UniformLocIsValid(camOrthoMinL))
    {
        Vector3f min = info.Cam->MinOrthoBounds + info.Cam->GetPosition();
        RenderDataHandler::SetUniformValue(camOrthoMinL, 3, &min.x);
    }
    if (RenderDataHandler::UniformLocIsValid(camOrthoMaxL))
    {
        Vector3f max = info.Cam->MaxOrthoBounds + info.Cam->GetPosition();
        RenderDataHandler::SetUniformValue(camOrthoMaxL, 3, &max.x);
    }
    if (RenderDataHandler::UniformLocIsValid(viewMatL))
        RenderDataHandler::SetMatrixValue(viewMatL, *(info.mView));
    if (RenderDataHandler::UniformLocIsValid(projMatL))
        RenderDataHandler::SetMatrixValue(projMatL, *(info.mProj));
    if (RenderDataHandler::UniformLocIsValid(viewProjMatL))
    {
        Matrix4f vp = Matrix4f::Multiply(*info.mProj, *info.mView);
        RenderDataHandler::SetMatrixValue(viewProjMatL, vp);
    }

    //Set the custom parameters.
    for (auto iterator = params.FloatUniforms.begin(); iterator != params.FloatUniforms.end(); ++iterator)
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
            RenderDataHandler::SetUniformValue(iterator->second.Location, iterator->second.NData, iterator->second.Value);
    for (auto iterator = params.FloatArrayUniforms.begin(); iterator != params.FloatArrayUniforms.end(); ++iterator)
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
            RenderDataHandler::SetUniformArrayValue(iterator->second.Location, iterator->second.NumbValues, iterator->second.BasicTypesPerValue, iterator->second.Values);
    for (auto iterator = params.IntUniforms.begin(); iterator != params.IntUniforms.end(); ++iterator)
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
            RenderDataHandler::SetUniformValue(iterator->second.Location, iterator->second.NData, iterator->second.Value);
    for (auto iterator = params.IntArrayUniforms.begin(); iterator != params.IntArrayUniforms.end(); ++iterator)
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
            RenderDataHandler::SetUniformArrayValue(iterator->second.Location, iterator->second.NumbValues, iterator->second.BasicTypesPerValue, iterator->second.Values);
    for (auto iterator = params.MatrixUniforms.begin(); iterator != params.MatrixUniforms.end(); ++iterator)
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
            RenderDataHandler::SetMatrixValue(iterator->second.Location, iterator->second.Value);
    for (auto iterator = params.SubroutineUniforms.begin(); iterator != params.SubroutineUniforms.end(); ++iterator)
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
            RenderDataHandler::SetSubroutineValue(iterator->second.Location, iterator->second.Definition->Shader, iterator->second.PossibleValueIDs[iterator->second.ValueIndex]);
    //Setting mesh texture sampler uniforms is a little more involved.
    int texUnit = 0;
    for (auto iterator = params.Texture2DUniforms.begin(); iterator != params.Texture2DUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
        {
            RenderDataHandler::SetUniformValue(iterator->second.Location, 1, &texUnit);
            RenderDataHandler::ActivateTextureUnit(texUnit);
            texUnit += 1;

            glBindTexture(GL_TEXTURE_2D, iterator->second.Texture);
        }
    }
    for (auto iterator = params.Texture3DUniforms.begin(); iterator != params.Texture3DUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
        {
            RenderDataHandler::SetUniformValue(iterator->second.Location, 1, &texUnit);
            RenderDataHandler::ActivateTextureUnit(texUnit);
            texUnit += 1;

            glBindTexture(GL_TEXTURE_3D, iterator->second.Texture);
        }
    }
    for (auto iterator = params.TextureCubemapUniforms.begin(); iterator != params.TextureCubemapUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
        {
            RenderDataHandler::SetUniformValue(iterator->second.Location, 1, &texUnit);
            RenderDataHandler::ActivateTextureUnit(texUnit);
            texUnit += 1;

            glBindTexture(GL_TEXTURE_CUBE_MAP, iterator->second.Texture);
        }
    }


    //Each mesh has its own world transform matrix, applied after the world transform specified in the rendering info.
    Matrix4f partialWVP = info.mWVP,
             finalWorld, finalWVP;

    //Render each mesh.
    for (unsigned int i = 0; i < meshes.size(); ++i)
    {
        const Mesh & mesh = *meshes[i];

        //Calculate final world and wvp matrices.
        mesh.Transform.GetWorldTransform(finalWorld);
        finalWorld = Matrix4f::Multiply(*(info.mWorld), finalWorld);
        finalWVP = Matrix4f::Multiply(partialWVP, finalWorld);

        //Pass those matrices to the shader.
        if (RenderDataHandler::UniformLocIsValid(worldMatL))
            RenderDataHandler::SetMatrixValue(worldMatL, finalWorld);
        if (RenderDataHandler::UniformLocIsValid(wvpMatL))
            RenderDataHandler::SetMatrixValue(wvpMatL, finalWVP);


        //Now render the mesh.

        const VertexIndexData& vid = mesh.SubMeshes[mesh.CurrentSubMesh];
        
        RenderDataHandler::BindVertexBuffer(vid.GetVerticesHandle());
        
        attributes.EnableAttributes();

        if (vid.UsesIndices())
        {
            RenderDataHandler::BindIndexBuffer(vid.GetIndicesHandle());
            ShaderHandler::DrawIndexedVertices(mesh.PrimType, vid.GetIndicesCount());
        }
        else
        {
            ShaderHandler::DrawVertices(mesh.PrimType, vid.GetVerticesCount(),
                                        sizeof(int) * vid.GetFirstVertex());
        }

        attributes.DisableAttributes();
    }


    //Check for rendering errors.
    errorMsg = GetCurrentRenderingError();
    if (HasError())
    {
        errorMsg = std::string() + "Error rendering material: " + errorMsg;
        return false;
    }

    return true;
}