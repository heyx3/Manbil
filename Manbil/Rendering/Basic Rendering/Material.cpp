#include "Material.h"

#include "../../Rendering/Basic Rendering/MaterialConstants.h"
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

void Material::SetUniformValueSubroutine(Shaders shaderType, unsigned int nValues,
                                         RenderObjHandle* valuesForAllSubroutines)
{
    glUniformSubroutinesuiv(ShaderTypeToGLEnum(shaderType), nValues, valuesForAllSubroutines);
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


    //Get uniforms.
    UniformLocation tempLoc;
    for (auto it = dict.begin(); it != dict.end(); ++it)
    {
        //Subroutines are a bit more complex to setup than other uniform types.
        if (it->second.Type == UT_VALUE_SUBROUTINE)
        {
            UniformValueSubroutine& uvs = it->second.Subroutine();
            tempLoc = GetSubroutineUniformLoc(shaderProg, uvs.Definition.Shader, it->first.c_str());
            if (tempLoc != INVALID_UNIFORM_LOCATION)
            {
                uniforms.push_back(it->second);
                Uniform& u = uniforms[uniforms.size() - 1];
                UniformValueSubroutine& sb = u.Subroutine();

                u.Loc = tempLoc;

                //Get the ID of each subroutine function.
                for (unsigned int i = 0; i < sb.PossibleValueIDs.size(); ++i)
                {
                    sb.PossibleValueIDs[i] = GetSubroutineID(shaderProg, sb.Definition.Shader,
                                                             sb.PossibleValues[i].c_str());
                }

                //Insert the subroutine into the correct subroutine list.
                Shaders shader = sb.Definition.Shader;
                std::vector<UniformLocation>& putInto = (shader == SH_VERTEX ?
                                                            vertexShaderSubroutines :
                                                            (shader == SH_GEOMETRY ?
                                                                geometryShaderSubroutines :
                                                                fragmentShaderSubroutines));
                unsigned int i;
                for (i = 0; i < putInto.size(); ++i)
                {
                    if (putInto[i] > u.Loc)
                    {
                        break;
                    }
                }
                putInto.insert(putInto.begin() + i, u.Loc);
            }
        }
        else
        {
            tempLoc = GetUniformLoc(shaderProg, it->first.c_str());
            if (tempLoc != INVALID_UNIFORM_LOCATION)
            {
                uniforms.push_back(it->second);
                uniforms[uniforms.size() - 1].Loc = tempLoc;

            }
        }
    }
    //Finalize subroutine values.
    vertexShaderSubroutineValues.resize(vertexShaderSubroutines.size());
    geometryShaderSubroutineValues.resize(geometryShaderSubroutines.size());
    fragmentShaderSubroutineValues.resize(fragmentShaderSubroutines.size());

    //Now that custom uniforms are figured out, put the uniform locations into the original dictionary.
    for (unsigned int i = 0; i < uniforms.size(); ++i)
        dict[uniforms[i].Name].Loc = uniforms[i].Loc;

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
    GetBlendMode().EnableMode();

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
                           data.GetRangeSize(), GL_UNSIGNED_INT,
                           (GLvoid*)data.GetRangeStart());
        }
        else
        {
            glDrawArrays(PrimitiveTypeToGLEnum(data.PrimType),
                         data.GetRangeStart(), data.GetRangeSize());
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
    GetBlendMode().EnableMode();

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
                           meshDat.GetRangeSize(), GL_UNSIGNED_INT, (GLvoid*)meshDat.GetRangeStart());
        }
        else
        {
            glDrawArrays(PrimitiveTypeToGLEnum(meshDat.PrimType),
                         meshDat.GetRangeStart(), meshDat.GetRangeSize());
        }

        attributes.DisableAttributes();
    }
}

void Material::SetUniforms(const UniformDictionary& params)
{
    int texUnit = 0;

    for (auto it = params.begin(); it != params.end(); ++it)
    {
        if (it->second.Loc == INVALID_UNIFORM_LOCATION)
            continue;

        switch (it->second.Type)
        {
            case UT_VALUE_F:
                SetUniformValueF(it->second.Loc, it->second.Float().GetSize(),
                                 it->second.Float().GetValue());
                break;
            case UT_VALUE_I:
                SetUniformValueI(it->second.Loc, it->second.Int().GetSize(),
                                 it->second.Int().GetValue());
                break;
            case UT_VALUE_F_ARRAY:
                SetUniformValueArrayF(it->second.Loc, it->second.FloatArray().NValues,
                                      it->second.FloatArray().NComponentsPerValue,
                                      it->second.FloatArray().Values);
                break;
            case UT_VALUE_I_ARRAY:
                SetUniformValueArrayI(it->second.Loc, it->second.IntArray().NValues,
                                      it->second.IntArray().NComponentsPerValue,
                                      it->second.IntArray().Values);
                break;
            case UT_VALUE_MAT4:
                SetUniformValueMatrix4f(it->second.Loc, it->second.Matrix());
                break;

            //Textures are a bit more involved to enable.
            case UT_VALUE_SAMPLER2D:
            case UT_VALUE_SAMPLER3D:
            case UT_VALUE_SAMPLERCUBE:
                if (it->second.Tex() != INVALID_RENDER_OBJ_HANDLE)
                {
                    SetUniformValueI(it->second.Loc, 1, &texUnit);
                    ActivateTextureUnit(texUnit);
                    texUnit += 1;

                    GLenum enm = (it->second.Type == UT_VALUE_SAMPLER2D) ?
                                    GL_TEXTURE_2D :
                                    ((it->second.Type == UT_VALUE_SAMPLER3D) ?
                                        GL_TEXTURE_3D :
                                        GL_TEXTURE_CUBE_MAP);
                    glBindTexture(enm, it->second.Tex());
                }
                break;

            //Subroutines are even MORE complicated to set up, because they all have to be set at once.
            case UT_VALUE_SUBROUTINE:

                const UniformValueSubroutine& uvs = it->second.Subroutine();

                std::vector<UniformLocation>* subroutineOrder = 0;
                std::vector<RenderObjHandle>* subroutineValue = 0;

                switch (uvs.Definition.Shader)
                {
                    case SH_VERTEX:
                        subroutineOrder = &vertexShaderSubroutines;
                        subroutineValue = &vertexShaderSubroutineValues;
                        break;
                    case SH_GEOMETRY:
                        subroutineOrder = &geometryShaderSubroutines;
                        subroutineValue = &geometryShaderSubroutineValues;
                        break;
                    case SH_FRAGMENT:
                        subroutineOrder = &fragmentShaderSubroutines;
                        subroutineValue = &fragmentShaderSubroutineValues;
                        break;

                    default: assert(false);
                }

                //Find where to insert the subroutine value.
                bool inserted = false;
                for (unsigned int i = 0; i < subroutineOrder->size(); ++i)
                {
                    if (subroutineOrder->operator[](i) == it->second.Loc)
                    {
                        subroutineValue->operator[](i) = uvs.PossibleValueIDs[uvs.ValueIndex];
                        inserted = true;
                        break;
                    }
                }
                assert(inserted);

                break;
        }
    }
    //Finish setting subroutine values.
    if (vertexShaderSubroutineValues.size() > 0)
    {
        SetUniformValueSubroutine(SH_VERTEX, vertexShaderSubroutineValues.size(),
                                  vertexShaderSubroutineValues.data());
    }
    if (fragmentShaderSubroutineValues.size() > 0)
    {
        SetUniformValueSubroutine(SH_FRAGMENT, fragmentShaderSubroutineValues.size(),
                                  fragmentShaderSubroutineValues.data());
    }
    if (geometryShaderSubroutineValues.size() > 0)
    {
        SetUniformValueSubroutine(SH_GEOMETRY, geometryShaderSubroutineValues.size(),
                                  geometryShaderSubroutineValues.data());
    }
}