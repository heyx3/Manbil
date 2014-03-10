#include "Material.h"

#include "Mesh.h"


Material::Material(std::string & vs, std::string & fs, UniformDictionary & dict, RenderingModes m, bool il, LightSettings ls)
    : isLit(il), lightSettings(ls), mode(m)
{
    ShaderHandler::CreateShaderProgram(shaderProg);


    RenderObjHandle vsO, fsO;
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



    if (!ShaderHandler::FinalizeShaders(shaderProg))
    {
        errorMsg = std::string() + "Error finalizing material shaders: " + ShaderHandler::GetErrorMessage();
        return;
    }

    glDeleteShader(vsO);
    glDeleteShader(fsO);


    //Get node uniforms.
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
    for (auto iterator = dict.TextureUniforms.begin(); iterator != dict.TextureUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.TextureUniforms.insert(uniforms.TextureUniforms.end(),
                                            UniformList::Uniform(iterator->first, tempLoc));
            dict.TextureUniforms[iterator->first].Location = tempLoc;
        }
    }

    //Get built-in uniforms.
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::ElapsedTimeName.c_str(), timeL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraPosName.c_str(), camPosL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraForwardName.c_str(), camForwardL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraUpName.c_str(), camUpL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::CameraSideName.c_str(), camSideL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::WorldMatName.c_str(), worldMatL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::ViewMatName.c_str(), viewMatL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::ProjMatName.c_str(), projMatL);
    RenderDataHandler::GetUniformLocation(shaderProg, MaterialConstants::WVPMatName.c_str(), wvpMatL);
}

bool Material::Render(RenderPasses pass, const RenderInfo & info, const std::vector<const Mesh*> & meshes)
{
    ClearAllRenderingErrors();

    ShaderHandler::UseShader(shaderProg);

    //Set basic uniforms.
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
    if (RenderDataHandler::UniformLocIsValid(viewMatL))
        RenderDataHandler::SetMatrixValue(viewMatL, *(info.mView));
    if (RenderDataHandler::UniformLocIsValid(projMatL))
        RenderDataHandler::SetMatrixValue(projMatL, *(info.mProj));

    //Each mesh has its own world transform matrix, applied after the world transform specified in the rendering info.
    Matrix4f partialWVP = info.mWVP,
             finalWorld, finalWVP;

    //Render each mesh.
    for (int i = 0; i < meshes.size(); ++i)
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

        //Set the mesh's custom uniform values.
        for (auto iterator = mesh.Uniforms.FloatUniforms.begin(); iterator != mesh.Uniforms.FloatUniforms.end(); ++iterator)
            if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
                RenderDataHandler::SetUniformValue(iterator->second.Location, iterator->second.NData, iterator->second.Value);
        for (auto iterator = mesh.Uniforms.FloatArrayUniforms.begin(); iterator != mesh.Uniforms.FloatArrayUniforms.end(); ++iterator)
            if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
                RenderDataHandler::SetUniformArrayValue(iterator->second.Location, iterator->second.NumbValues, iterator->second.BasicTypesPerValue, iterator->second.Values);
        for (auto iterator = mesh.Uniforms.IntUniforms.begin(); iterator != mesh.Uniforms.IntUniforms.end(); ++iterator)
            if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
                RenderDataHandler::SetUniformValue(iterator->second.Location, iterator->second.NData, iterator->second.Value);
        for (auto iterator = mesh.Uniforms.IntArrayUniforms.begin(); iterator != mesh.Uniforms.IntArrayUniforms.end(); ++iterator)
            if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
                RenderDataHandler::SetUniformArrayValue(iterator->second.Location, iterator->second.NumbValues, iterator->second.BasicTypesPerValue, iterator->second.Values);
        for (auto iterator = mesh.Uniforms.MatrixUniforms.begin(); iterator != mesh.Uniforms.MatrixUniforms.end(); ++iterator)
            if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
                RenderDataHandler::SetMatrixValue(iterator->second.Location, iterator->second.Value);


        //Check for rendering errors.
        errorMsg = GetCurrentRenderingError();
        if (HasError())
        {
            errorMsg = std::string() + "Error setting uniforms, mesh " + std::to_string(i) + ": " + errorMsg;
            return false;
        }

        //Setting mesh texture sampler uniforms is a little more involved.
        unsigned int texUnit = 0;
        for (auto iterator = mesh.Uniforms.TextureUniforms.begin(); iterator != mesh.Uniforms.TextureUniforms.end(); ++iterator)
        {
            if (RenderDataHandler::UniformLocIsValid(iterator->second.Location))
            {
                RenderDataHandler::ActivateTextureUnit(texUnit);
                texUnit += 1;

                //Samplers can use either an SFML Texture object or an OpenGL texture object.
                if (iterator->second.SFMLTexture != 0)
                    sf::Texture::bind(iterator->second.SFMLTexture);
                else if (iterator->second.GLTexture != 0)
                    RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, iterator->second.GLTexture);
            }
        }


        //Check for rendering errors.
        errorMsg = GetCurrentRenderingError();
        if (HasError())
        {
            errorMsg = std::string() + "Error setting textures, mesh " + std::to_string(i) + ": " + errorMsg;
            return false;
        }


        //Now render the mesh.
        for (int v = 0; v < mesh.GetNumbVertexIndexData(); ++v)
        {
            const VertexIndexData & vid = mesh.GetVertexIndexData(v);

            RenderDataHandler::BindVertexBuffer(vid.GetVerticesHandle());

            Vertex::EnableVertexAttributes();

            if (vid.UsesIndices())
            {
                RenderDataHandler::BindIndexBuffer(vid.GetIndicesHandle());
                ShaderHandler::DrawIndexedVertices(mesh.GetPrimType(), vid.GetIndicesCount());
            }
            else
            {
                ShaderHandler::DrawVertices(mesh.GetPrimType(), vid.GetVerticesCount(), sizeof(int) * vid.GetFirstVertex());
            }

            Vertex::DisableVertexAttributes();


            //Check for rendering errors.
            errorMsg = GetCurrentRenderingError();
            if (HasError())
            {
                errorMsg = std::string() + "Error rendering material, mesh " + std::to_string(i) + ", VertexIndexData " + std::to_string(v) + ": " + errorMsg;
                return false;
            }
        }
    }

    return true;
}