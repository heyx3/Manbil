#include "Material.h"

#include "../../Rendering/Basic Rendering/MaterialData.h"
#include "../../Rendering/Basic Rendering/Mesh.h"



const UniformLocation Material::INVALID_UNIFORM_LOCATION = 0xFFFFFFFF;
unsigned int Material::maxTexUnits = 0;


RenderObjHandle Material::CreateShader(RenderObjHandle shaderProg, const GLchar* shaderText,
                                       Shaders shaderType, std::string& outError)
{
    GLenum glShaderType = ShaderTypeToGLEnum(shaderType);

    RenderObjHandle shader = glCreateShader(glShaderType);
    if (shader == 0)
    {
        outError = "Couldn't create shader object.";
        return 0;
    }

    const GLchar* shaderTexts[] = { shaderText };
    UniformLocation shaderTextLengths[] = { strlen(shaderText) };
    glShaderSource(shader, 1, shaderTexts, shaderTextLengths);

    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        outError = infoLog;
        return 0;
    }

    glAttachShader(shaderProg, shader);

    return shader;
}

unsigned int Material::GetMaxTextureUnits(void)
{
    if (maxTexUnits == 0)
    {
        int max;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max);
        assert(max > 0);
        maxTexUnits = (unsigned int)max;
    }

    return maxTexUnits;
}

UniformLocation Material::GetUniformLoc(RenderObjHandle shaderProgram, const GLchar* name)
{
    return glGetUniformLocation(shaderProgram, name);
}
UniformLocation Material::GetSubroutineUniformLoc(RenderObjHandle shaderProgram, Shaders shaderType,
                                                  const GLchar* name)
{
    return glGetSubroutineUniformLocation(shaderProgram, ShaderTypeToGLEnum(shaderType), name);
}
RenderObjHandle Material::GetSubroutineID(RenderObjHandle shaderProgram, Shaders shaderType,
                                          const GLchar* name)
{
    return glGetSubroutineIndex(shaderProgram, ShaderTypeToGLEnum(shaderType), name);
}

void Material::SetUniformValueF(UniformLocation loc, unsigned int nComponents, const float* components)
{
    assert(nComponents >= 1 && nComponents <= 4);
    switch (nComponents)
    {
        case 1:
            glUniform1f(loc, components[0]);
            break;
        case 2:
            glUniform2f(loc, components[0], components[1]);
            break;
        case 3:
            glUniform3f(loc, components[0], components[1], components[2]);
            break;
        case 4:
            glUniform4f(loc, components[0], components[1], components[2], components[3]);
            break;

        default: assert(false);
    }
}
void Material::SetUniformValueArrayF(UniformLocation loc, unsigned int nArrayElements,
                                     unsigned int nComponents, const float* elements)
{
    assert(nComponents >= 1 && nComponents <= 4);

    switch (nComponents)
    {
        case 1:
            glUniform1fv(loc, nArrayElements, elements);
            break;
        case 2:
            glUniform2fv(loc, nArrayElements, elements);
            break;
        case 3:
            glUniform3fv(loc, nArrayElements, elements);
            break;
        case 4:
            glUniform4fv(loc, nArrayElements, elements);
            break;

        default: assert(false);
    }
}
void Material::SetUniformValueI(UniformLocation loc, unsigned int nComponents, const int* components)
{
    assert(nComponents >= 1 && nComponents <= 4);
    switch (nComponents)
    {
        case 1:
            glUniform1i(loc, components[0]);
            break;
        case 2:
            glUniform2i(loc, components[0], components[1]);
            break;
        case 3:
            glUniform3i(loc, components[0], components[1], components[2]);
            break;
        case 4:
            glUniform4i(loc, components[0], components[1], components[2], components[3]);
            break;

        default: assert(false);
    }
}
void Material::SetUniformValueArrayI(UniformLocation loc, unsigned int nArrayElements,
                                     unsigned int nComponents, const int* elements)
{
    assert(nComponents >= 1 && nComponents <= 4);

    switch (nComponents)
    {
        case 1:
            glUniform1iv(loc, nArrayElements, elements);
            break;
        case 2:
            glUniform2iv(loc, nArrayElements, elements);
            break;
        case 3:
            glUniform3iv(loc, nArrayElements, elements);
            break;
        case 4:
            glUniform4iv(loc, nArrayElements, elements);
            break;

        default: assert(false);
    }
}
void Material::SetUniformValueMatrix4f(UniformLocation loc, const Matrix4f& mat)
{
    glUniformMatrix4fv(loc, 1, GL_TRUE, (const GLfloat*)(&mat));
}

void Material::SetUniformValueSubroutine(Shaders shaderType, RenderObjHandle* valuesForAllSubroutines)
{
    glUniformSubroutinesuiv(ShaderTypeToGLEnum(shaderType),
                            uniforms.Subroutines.size(), valuesForAllSubroutines);
}

void Material::ActivateTextureUnit(unsigned int unitIndex)
{
    assert(GetMaxTextureUnits() > unitIndex);
    glActiveTexture(GL_TEXTURE0 + unitIndex);
}


Material::Material(const std::string& vs, const std::string& fs, UniformDictionary& dict,
                   const RenderIOAttributes& attrs, BlendMode m, std::string& outErrorMsg,
                   std::string geoShader)
    : mode(m), attributes(attrs)
{
    //Create the material object.
    shaderProg = glCreateProgram();
    if (shaderProg == 0)
    {
        outErrorMsg = "Couldn't create shader program.";
        return;
    }


    //Compile the shaders.

    bool useGeoShader = !geoShader.empty();
    RenderObjHandle vsO, fsO, gsO = 0;

    vsO = CreateShader(shaderProg, vs.c_str(), SH_VERTEX, outErrorMsg);
    if (!outErrorMsg.empty())
    {
        outErrorMsg = std::string("Error creating vertex shader: ") + outErrorMsg;
        return;
    }

    fsO = CreateShader(shaderProg, fs.c_str(), SH_FRAGMENT, outErrorMsg);
    if (!outErrorMsg.empty())
    {
        outErrorMsg = std::string("Error creating fragment shader: ") + outErrorMsg;
        return;
    }

    if (useGeoShader)
    {
        gsO = CreateShader(shaderProg, geoShader.c_str(), SH_GEOMETRY, outErrorMsg);
        if (!outErrorMsg.empty())
        {
            outErrorMsg = std::string("Error creating geometry shader: ") + outErrorMsg;
            return;
        }
    }


    //Link and verify the shaders.

    UniformLocation success;
    char error[1024];

    glLinkProgram(shaderProg);
    glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProg, sizeof(error), NULL, error);
        outErrorMsg = error;
        return;
    }

    glValidateProgram(shaderProg);
    glGetProgramiv(shaderProg, GL_VALIDATE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProg, sizeof(error), NULL, error);
        outErrorMsg = error;
        return;
    }


    //Clean up the individual shader programs.
    glDeleteShader(vsO);
    glDeleteShader(fsO);
    if (useGeoShader)
    {
        glDeleteShader(gsO);
    }


    //Get node uniforms.
    UniformLocation tempLoc;
    for (auto it = dict.Floats.begin(); it != dict.Floats.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.Floats.insert(uniforms.Floats.end(),
                                   UniformList::Uniform(it->first, tempLoc));
            dict.Floats[it->first].Location = tempLoc;
        }
    }
    for (auto it = dict.FloatArrays.begin(); it != dict.FloatArrays.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.FloatArrays.insert(uniforms.FloatArrays.end(),
                                        UniformList::Uniform(it->first, tempLoc));
            dict.FloatArrays[it->first].Location = tempLoc;
        }
    }
    for (auto it = dict.Ints.begin(); it != dict.Ints.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.Ints.insert(uniforms.Ints.end(),
                                 UniformList::Uniform(it->first, tempLoc));
            dict.Ints[it->first].Location = tempLoc;
        }
    }
    for (auto it = dict.IntArrays.begin(); it != dict.IntArrays.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.IntArrays.insert(uniforms.IntArrays.end(),
                                      UniformList::Uniform(it->first, tempLoc));
            dict.IntArrays[it->first].Location = tempLoc;
        }
    }
    for (auto it = dict.Matrices.begin(); it != dict.Matrices.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.Matrices.insert(uniforms.Matrices.end(),
                                     UniformList::Uniform(it->first, tempLoc));
            dict.Matrices[it->first].Location = tempLoc;
        }
    }
    for (auto it = dict.Texture2Ds.begin(); it != dict.Texture2Ds.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.Texture2Ds.insert(uniforms.Texture2Ds.end(),
                                       UniformList::Uniform(it->first, tempLoc));
            dict.Texture2Ds[it->first].Location = tempLoc;
        }
    }
    for (auto it = dict.Texture3Ds.begin(); it != dict.Texture3Ds.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.Texture3Ds.insert(uniforms.Texture3Ds.end(),
                                       UniformList::Uniform(it->first, tempLoc));
            dict.Texture3Ds[it->first].Location = tempLoc;
        }
    }
    for (auto it = dict.TextureCubemaps.begin(); it != dict.TextureCubemaps.end(); ++it)
    {
        tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.TextureCubemaps.insert(uniforms.TextureCubemaps.end(),
                                            UniformList::Uniform(it->first, tempLoc));
            dict.TextureCubemaps[it->first].Location = tempLoc;
        }
    }
    //Subroutines are a bit more complex to set up.
    for (auto it = dict.Subroutines.begin(); it != dict.Subroutines.end(); ++it)
    {
        tempLoc = GetSubroutineUniformLoc(shaderProg, it->second.Definition->Shader, it->first.c_str());
        if (tempLoc != INVALID_UNIFORM_LOCATION)
        {
            uniforms.Subroutines.insert(uniforms.Subroutines.end(),
                                        UniformList::Uniform(it->first, tempLoc));

            UniformValueSubroutine& sb = dict.Subroutines[it->first];
            sb.Location = tempLoc;

            //Get the ID of each subroutine function.
            for (unsigned int i = 0; i < sb.PossibleValueIDs.size(); ++i)
            {
                sb.PossibleValueIDs[i] = GetSubroutineID(shaderProg, sb.Definition->Shader,
                                                         sb.PossibleValues[i].c_str());
            }

            //Insert the subroutine into the correct subroutine list.
            Shaders shader = sb.Definition->Shader;
            std::vector<UniformLocation>& putInto = (shader == SH_VERTEX ?
                                                        vertexShaderSubroutines :
                                                        (shader == SH_GEOMETRY ?
                                                            geometryShaderSubroutines :
                                                            fragmentShaderSubroutines));
            unsigned int i;
            for (i = 0; i < putInto.size(); ++i)
            {
                if (putInto[i] > sb.Location)
                {
                    break;
                }
            }
            putInto.insert(putInto.begin() + i, sb.Location);
        }
    }
    vertexShaderSubroutineValues.resize(vertexShaderSubroutines.size());
    geometryShaderSubroutineValues.resize(geometryShaderSubroutines.size());
    fragmentShaderSubroutineValues.resize(fragmentShaderSubroutines.size());


    //Get built-in uniforms.
    timeL = GetUniformLoc(shaderProg, MaterialConstants::ElapsedTimeName.c_str());
    camPosL = GetUniformLoc(shaderProg, MaterialConstants::CameraPosName.c_str());
    camForwardL = GetUniformLoc(shaderProg, MaterialConstants::CameraForwardName.c_str());
    camUpL = GetUniformLoc(shaderProg, MaterialConstants::CameraUpName.c_str());
    camSideL = GetUniformLoc(shaderProg, MaterialConstants::CameraSideName.c_str());
    camWidthL = GetUniformLoc(shaderProg, MaterialConstants::CameraWidthName.c_str());
    camHeightL = GetUniformLoc(shaderProg, MaterialConstants::CameraHeightName.c_str());
    camZNearL = GetUniformLoc(shaderProg, MaterialConstants::CameraZNearName.c_str());
    camZFarL = GetUniformLoc(shaderProg, MaterialConstants::CameraZFarName.c_str());
    camFovL = GetUniformLoc(shaderProg, MaterialConstants::CameraFovName.c_str());
    camOrthoMinL = GetUniformLoc(shaderProg, MaterialConstants::CameraOrthoMinName.c_str());
    camOrthoMaxL = GetUniformLoc(shaderProg, MaterialConstants::CameraOrthoMaxName.c_str());
    worldMatL = GetUniformLoc(shaderProg, MaterialConstants::WorldMatName.c_str());
    viewMatL = GetUniformLoc(shaderProg, MaterialConstants::ViewMatName.c_str());
    projMatL = GetUniformLoc(shaderProg, MaterialConstants::ProjMatName.c_str());
    wvpMatL = GetUniformLoc(shaderProg, MaterialConstants::WVPMatName.c_str());
    viewProjMatL = GetUniformLoc(shaderProg, MaterialConstants::ViewProjMatName.c_str());
}
Material::~Material(void)
{
    glDeleteProgram(shaderProg);
}

void Material::Render(const RenderInfo& info, const Mesh* toRender, const UniformDictionary& params)
{
    Render(info, params, &toRender, 1);
}
void Material::Render(const RenderInfo& info, const std::vector<const Mesh*>& meshes,
                      const UniformDictionary& params)
{
    Render(info, params, meshes.data(), meshes.size());
}
void Material::Render(const RenderInfo& info, const UniformDictionary& params,
                      const Mesh*const* meshPtrArray, unsigned int nMeshes)
{
    glUseProgram(shaderProg);


    #pragma region Set basic uniforms

    //TODO: Turn these into global uniforms.
    if (timeL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(timeL, 1, &info.TotalElapsedSeconds);
    }
    Vector3f camPos = info.Cam->GetPosition();
    if (camPosL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camPosL, 3, &camPos[0]);
    }
    Vector3f camF = info.Cam->GetForward();
    if (camForwardL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camForwardL, 3, &camF[0]);
    }
    Vector3f camU = info.Cam->GetUpward();
    if (camUpL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camUpL, 3, &camU[0]);
    }
    Vector3f camS = info.Cam->GetSideways();
    if (camSideL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camSideL, 3, &camS[0]);
    }
    if (camWidthL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camWidthL, 1, &info.Cam->PerspectiveInfo.Width);
    }
    if (camHeightL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camHeightL, 1, &info.Cam->PerspectiveInfo.Height);
    }
    if (camZNearL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camZNearL, 1, &info.Cam->PerspectiveInfo.zNear);
    }
    if (camZFarL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camZFarL, 1, &info.Cam->PerspectiveInfo.zFar);
    }
    if (camFovL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camFovL, 1, &info.Cam->PerspectiveInfo.FOV);
    }
    if (camOrthoMinL != INVALID_UNIFORM_LOCATION)
    {
        Vector3f min = info.Cam->MinOrthoBounds + info.Cam->GetPosition();
        SetUniformValueF(camOrthoMinL, 3, &min.x);
    }
    if (camOrthoMaxL != INVALID_UNIFORM_LOCATION)
    {
        Vector3f max = info.Cam->MaxOrthoBounds + info.Cam->GetPosition();
        SetUniformValueF(camOrthoMaxL, 3, &max.x);
    }
    if (viewMatL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueMatrix4f(viewMatL, *(info.mView));
    }
    if (projMatL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueMatrix4f(projMatL, *(info.mProj));
    }
    if (viewProjMatL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueMatrix4f(viewProjMatL, info.mVP);
    }

    #pragma endregion

    SetUniforms(params);


    Matrix4f mWorld, mWVP;

    //Render each mesh.
    for (unsigned int i = 0; i < nMeshes; ++i)
    {
        const Mesh& mesh = *meshPtrArray[i];

        //Calculate world and wvp matrices.
        mesh.Transform.GetWorldTransform(mWorld);
        mWVP = Matrix4f::Multiply(info.mVP, mWorld);

        //Pass those matrices to the shader.
        if (worldMatL != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueMatrix4f(worldMatL, mWorld);
        }
        if (wvpMatL != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueMatrix4f(wvpMatL, mWVP);
        }


        //Now render the mesh.

        const MeshData& data = mesh.SubMeshes[mesh.CurrentSubMesh];
        
        data.Bind();
        attributes.EnableAttributes();
        
        if (data.GetUsesIndices())
        {
            glDrawElements(PrimitiveTypeToGLEnum(data.PrimType),
                           data.GetNIndices(), GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(PrimitiveTypeToGLEnum(data.PrimType),
                         0, data.GetNVertices());
        }

        attributes.DisableAttributes();
    }
}

void Material::Render(const RenderInfo& info, const MeshData& toRender,
                      const Matrix4f& worldMat, const UniformDictionary& params)

{
    Render(info, &toRender, &worldMat, 1, params);
}
void Material::Render(const RenderInfo& info, const MeshData* toRender, const Matrix4f* worldMats,
                      unsigned int nToRender, const UniformDictionary& params)
{
    glUseProgram(shaderProg);

    #pragma region Set basic uniforms

    //TODO: Turn these into global uniforms.
    if (timeL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(timeL, 1, &info.TotalElapsedSeconds);
    }
    Vector3f camPos = info.Cam->GetPosition();
    if (camPosL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camPosL, 3, &camPos[0]);
    }
    Vector3f camF = info.Cam->GetForward();
    if (camForwardL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camForwardL, 3, &camF[0]);
    }
    Vector3f camU = info.Cam->GetUpward();
    if (camUpL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camUpL, 3, &camU[0]);
    }
    Vector3f camS = info.Cam->GetSideways();
    if (camSideL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camSideL, 3, &camS[0]);
    }
    if (camWidthL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camWidthL, 1, &info.Cam->PerspectiveInfo.Width);
    }
    if (camHeightL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camHeightL, 1, &info.Cam->PerspectiveInfo.Height);
    }
    if (camZNearL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camZNearL, 1, &info.Cam->PerspectiveInfo.zNear);
    }
    if (camZFarL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camZFarL, 1, &info.Cam->PerspectiveInfo.zFar);
    }
    if (camFovL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueF(camFovL, 1, &info.Cam->PerspectiveInfo.FOV);
    }
    if (camOrthoMinL != INVALID_UNIFORM_LOCATION)
    {
        Vector3f min = info.Cam->MinOrthoBounds + info.Cam->GetPosition();
        SetUniformValueF(camOrthoMinL, 3, &min.x);
    }
    if (camOrthoMaxL != INVALID_UNIFORM_LOCATION)
    {
        Vector3f max = info.Cam->MaxOrthoBounds + info.Cam->GetPosition();
        SetUniformValueF(camOrthoMaxL, 3, &max.x);
    }
    if (viewMatL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueMatrix4f(viewMatL, *(info.mView));
    }
    if (projMatL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueMatrix4f(projMatL, *(info.mProj));
    }
    if (viewProjMatL != INVALID_UNIFORM_LOCATION)
    {
        SetUniformValueMatrix4f(viewProjMatL, info.mVP);
    }

    #pragma endregion

    SetUniforms(params);


    Matrix4f mWVP;

    //Render each mesh.
    for (unsigned int i = 0; i < nToRender; ++i)
    {
        const MeshData& meshDat = toRender[i];

        //Calculate world and wvp matrices.
        mWVP = Matrix4f::Multiply(info.mVP, worldMats[i]);

        //Pass those matrices to the shader.
        if (worldMatL != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueMatrix4f(worldMatL, worldMats[i]);
        }
        if (wvpMatL != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueMatrix4f(wvpMatL, mWVP);
        }


        //Now render the mesh.

        meshDat.Bind();
        attributes.EnableAttributes();
        
        if (meshDat.GetUsesIndices())
        {
            glDrawElements(PrimitiveTypeToGLEnum(meshDat.PrimType),
                           meshDat.GetNIndices(), GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(PrimitiveTypeToGLEnum(meshDat.PrimType),
                         0, meshDat.GetNVertices());
        }

        attributes.DisableAttributes();
    }
}

void Material::SetUniforms(const UniformDictionary& params)
{
    //Floats.
    for (auto it = params.Floats.begin(); it != params.Floats.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueF(it->second.Location, it->second.NData, it->second.Value);
        }
    }
    //Float arrays.
    for (auto it = params.FloatArrays.begin(); it != params.FloatArrays.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueArrayF(it->second.Location, it->second.NValues,
                                  it->second.NComponentsPerValue, it->second.Values);
        }
    }
    //Ints.
    for (auto it = params.Ints.begin(); it != params.Ints.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueI(it->second.Location, it->second.NData, it->second.Value);
        }
    }
    //Int arrays.
    for (auto it = params.IntArrays.begin(); it != params.IntArrays.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueArrayI(it->second.Location, it->second.NValues,
                                  it->second.NComponentsPerValue, it->second.Values);
        }
    }
    //Matrices.
    for (auto it = params.Matrices.begin(); it != params.Matrices.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueMatrix4f(it->second.Location, it->second.Value);
        }
    }

    //Textures are a bit more complicated to set.
    int texUnit = 0;
    for (auto it = params.Texture2Ds.begin(); it != params.Texture2Ds.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueI(it->second.Location, 1, &texUnit);
            ActivateTextureUnit(texUnit);
            texUnit += 1;

            glBindTexture(GL_TEXTURE_2D, it->second.Texture);
        }
    }
    for (auto it = params.Texture3Ds.begin(); it != params.Texture3Ds.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueI(it->second.Location, 1, &texUnit);
            ActivateTextureUnit(texUnit);
            texUnit += 1;

            glBindTexture(GL_TEXTURE_3D, it->second.Texture);
        }
    }
    for (auto it = params.TextureCubemaps.begin(); it != params.TextureCubemaps.end(); ++it)
    {
        if (it->second.Location != INVALID_UNIFORM_LOCATION)
        {
            SetUniformValueI(it->second.Location, 1, &texUnit);
            ActivateTextureUnit(texUnit);
            texUnit += 1;

            glBindTexture(GL_TEXTURE_CUBE_MAP, it->second.Texture);
        }
    }

    //Subroutines are a LOT more complicated to set, because they all have to be set at once.
    for (auto iterator = params.Subroutines.begin(); iterator != params.Subroutines.end(); ++iterator)
    {
        std::vector<UniformLocation>* subroutineOrder = 0;
        std::vector<RenderObjHandle>* subroutineValue = 0;

        switch (iterator->second.Definition->Shader)
        {
            case SH_VERTEX:
                subroutineOrder = &vertexShaderSubroutines;
                subroutineValue = &vertexShaderSubroutineValues;
                break;
            case SH_FRAGMENT:
                subroutineOrder = &fragmentShaderSubroutines;
                subroutineValue = &fragmentShaderSubroutineValues;
                break;
            case SH_GEOMETRY:
                subroutineOrder = &geometryShaderSubroutines;
                subroutineValue = &geometryShaderSubroutineValues;
                break;

            default: assert(false);
        }

        //Find where to insert the subroutine value.
        bool inserted = false;
        for (unsigned int i = 0; i < subroutineOrder->size(); ++i)
        {
            if (subroutineOrder->operator[](i) == iterator->second.Location)
            {
                subroutineValue->operator[](i) = iterator->second.PossibleValueIDs[iterator->second.ValueIndex];
                inserted = true;
                break;
            }
        }
        assert(inserted);
    }
    if (vertexShaderSubroutineValues.size() > 0)
    {
        SetUniformValueSubroutine(SH_VERTEX, vertexShaderSubroutineValues.data());
    }
    if (fragmentShaderSubroutineValues.size() > 0)
    {
        SetUniformValueSubroutine(SH_FRAGMENT, fragmentShaderSubroutineValues.data());
    }
    if (geometryShaderSubroutineValues.size() > 0)
    {
        SetUniformValueSubroutine(SH_GEOMETRY, geometryShaderSubroutineValues.data());
    }
}