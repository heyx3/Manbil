#include "Material.h"


Material::Material(std::string & vs, std::string & fs, const UniformDictionary & dict, RenderingModes m, bool il, LightSettings ls)
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


    //Get uniforms.
    UniformLocation tempLoc;
    for (auto iterator = dict.FloatUniforms.begin(); iterator != dict.FloatUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.FloatUniforms.insert(uniforms.FloatUniforms.end(),
                                          UniformList::Uniform(iterator->first, tempLoc));
        }
    }
    for (auto iterator = dict.FloatArrayUniforms.begin(); iterator != dict.FloatArrayUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.FloatArrayUniforms.insert(uniforms.FloatArrayUniforms.end(),
                                               UniformList::Uniform(iterator->first, tempLoc));
        }
    }
    for (auto iterator = dict.MatrixUniforms.begin(); iterator != dict.MatrixUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.MatrixUniforms.insert(uniforms.MatrixUniforms.end(),
                                           UniformList::Uniform(iterator->first, tempLoc));
        }
    }
    for (auto iterator = dict.TextureUniforms.begin(); iterator != dict.TextureUniforms.end(); ++iterator)
    {
        if (RenderDataHandler::GetUniformLocation(shaderProg, iterator->first.c_str(), tempLoc))
        {
            uniforms.TextureUniforms.insert(uniforms.TextureUniforms.end(),
                                            UniformList::Uniform(iterator->first, tempLoc));
        }
    }
}








//typedef std::unordered_map<std::string, UniformLocation> UniformLocMap;



/*
PassSamplers::PassSamplers(RenderObjHandle samplers[MaterialConstants::TWODSAMPLERS])
{
    for (int i = 0; i < MaterialConstants::TWODSAMPLERS; ++i)
    {
        if (samplers == 0) Samplers[i] = 0;
        else Samplers[i] = samplers[i];

        Panners[i] = Vector2f();
        Scales[i] = Vector2f(1.0f, 1.0f);
    }
}


#pragma region Shader headers

std::string nSamplers = std::to_string(MaterialConstants::TWODSAMPLERS);

//TODO: Rename "worldTo4DScreen" to "objectTo4DScreen". Same with "worldTo3DScreen". Also update the readme file for this system and add uniform names in Material.h.
std::string shaderHeaderPostfix = std::string() +
                       "uniform mat4 u_wvp;                                                                       \n\
						uniform mat4 u_world;                                                                     \n\
						uniform mat4 u_view;                                                                      \n\
						uniform mat4 u_proj;                                                                      \n\
						uniform vec3 u_cam_pos;                                                                   \n\
                        uniform vec3 u_cam_forward;                                                               \n\
                        uniform vec3 u_cam_upward;                                                                \n\
                        uniform vec3 u_cam_sideways;                                                              \n\
						uniform float u_elapsed_seconds;                                                          \n\
						                                                                                          \n\
						uniform sampler2D u_samplers2D[" + nSamplers + "];                                        \n\
                        uniform vec2 u_samplerScales[" + nSamplers + "];                                          \n\
                        uniform vec2 u_samplerPans[" + nSamplers + "];                                            \n\
                        uniform vec2 u_samplerOffsets[" + nSamplers + "];                                         \n\
						                                                                                          \n\
						vec4 sampleTex(int samp, vec2 uvs, vec2 uvOffset)                                         \n\
						{                                                                                         \n\
						    return texture2D(u_samplers2D[samp],                                                  \n\
                                             uvOffset + (u_samplerScales[samp] *                                  \n\
                                                         (uvs + u_samplerOffsets +                                \n\
                                                          (u_elapsed_seconds * u_samplerPans[samp]))));           \n\
                        }                                                                                         \n\
                        vec4 sampleTex(int samp, vec2 uvs) { return sampleTex(samp, uvs, vec2(0.0)); }            \n\
						                                                                                          \n\
						                                                                                          \n\
						vec4 worldTo4DScreen(vec3 world) { return u_wvp * vec4(world, 1.0); }                     \n\
						vec3 fourDScreenTo3DScreen(vec4 screen4D) { return screen4D.xyz / screen4D.w; }           \n\
						vec3 worldTo3DScreen(vec3 world) { return fourDScreenTo3DScreen(worldTo4DScreen(world)); }\n\
                                                                                                                  \n\
                        vec2 smoothstepVec2(vec2 start, vec2 end, float interp)                                   \n\
                        {                                                                                         \n\
                            return vec2(smoothstep(start.x, end.x, interp), smoothstep(start.y, end.y, interp));  \n\
                        }                                                                                         \n\
                        vec3 smoothstepVec3(vec3 start, vec3 end, float interp)                                   \n\
                        {                                                                                         \n\
                            return vec3(smoothstep(start.x, end.x, interp),                                       \n\
                                        smoothstep(start.y, end.y, interp),                                       \n\
                                        smoothstep(start.z, end.z, interp));                                      \n\
                        }                                                                                         \n\
                        vec4 smoothstepVec4(vec4 start, vec4 end, float interp)                                   \n\
                        {                                                                                         \n\
                            return vec4(smoothstep(start.x, end.x, interp),                                       \n\
                                        smoothstep(start.y, end.y, interp),                                       \n\
                                        smoothstep(start.z, end.z, interp),                                       \n\
                                        smoothstep(start.w, end.w, interp));                                      \n\
                        }                                                                                         \n\
                                                                                                                  \n\
                        vec4 getQuaternionRotation(vec3 rotAxis, float rotation)                                  \n\
                        {                                                                                         \n\
                            float sinHalfAngle = sin(rotation * 0.5),                                             \n\
                                  cosHalfAngle = cos(rotation * 0.5);                                             \n\
                            return vec4(rotAxis.xyz * sinHalfAngle, cosHalfAngle);                                \n\
                        }                                                                                         \n\
                        vec4 multiplyQuaternions(vec4 left, vec4 right)                                           \n\
                        {                                                                                         \n\
                            const vec3 constants = vec3(-1.0, 0.0, 1.0);                                          \n\
                            return vec4(dot(left.xwyz * constants.zzzx, right.wxzy),                              \n\
                                        dot(left.ywzx * constants.zzzx, right.wyxz),                              \n\
                                        dot(left.zwxy * constants.zzzx, right.wzyx),                              \n\
                                        dot(left.wxyz * constants.zxxx, right.wxyz));                             \n\
                        }                                                                                         \n\
                        vec4 multiplyQuaternionAndVector(vec4 quat, vec3 vector)                                  \n\
                        {                                                                                         \n\
                            return multiplyQuaternions(quat, vec4(vector.xyz, 0.0));                              \n\
                        }                                                                                         \n\
                        vec3 applyQuaternionRotation(vec3 vector, vec4 quaternion)                                \n\
                        {                                                                                         \n\
                            return vector + (2.0 * cross(cross(vector, quaternion.xyz) +                          \n\
                                                            (quaternion.w * vector),                              \n\
                                                         quaternion.xyz));                                        \n\
                        }                                                                                         \n\
                        vec4 slerp(vec4 first, vec4 second, float t)                                              \n\
                        {                                                                                         \n\
                            first = normalize(first);                                                             \n\
                            second = normalize(second);                                                           \n\
                            float dotted = clamp(dot(first, second), -1.0, 1.0);                                  \n\
                                                                                                                  \n\
                            float theta = acos(dotted) * t;                                                       \n\
                            vec4 finalQ = normalize(first - (second * dotted));                                   \n\
                            return (finalQ * sin(theta)) + (first * cos(theta));                                  \n\
                        }                                                                                         \n\
                                                                                                                  \n\
                                                                                                                  \n\
                        float getBrightness(vec3 surfaceNormal, vec3 camToFragNormal, vec3 lightDirNormal,        \n\
						   				    float ambient, float diffuse, float specular, float specularIntensity)\n\
						{                                                                                         \n\
							float dotted = max(dot(-surfaceNormal, lightDirNormal), 0.0);                         \n\
							                                                                                      \n\
							vec3 fragToCam = -camToFragNormal;                                                    \n\
							vec3 lightReflect = normalize(reflect(lightDirNormal, surfaceNormal));                \n\
							                                                                                      \n\
							float specFactor = max(0.0, dot(fragToCam, lightReflect));                            \n\
							specFactor = pow(specFactor, specularIntensity);                                      \n\
							                                                                                      \n\
							return ambient + (diffuse * dotted) + (specular * specFactor);                        \n\
						}                                                                                         \n\
						\n";
std::string vsHeader = "#version 330		                     \n\
						                                         \n\
						layout (location = 0) in vec3 in_pos;    \n\
						layout (location = 1) in vec4 in_col;    \n\
						layout (location = 2) in vec2 in_tex;    \n\
						layout (location = 3) in vec3 in_normal; \n\
						                                         \n\
						out vec3 out_pos;                        \n\
						out vec4 out_col;                        \n\
						out vec2 out_tex;                        \n\
						out vec3 out_normal;                     \n\
						\n\n" + shaderHeaderPostfix;
std::string psHeader = std::string() +
                       "#version 330		   \n\
						                       \n\
						in vec3 out_pos;       \n\
						in vec4 out_col;       \n\
						in vec2 out_tex;       \n\
						in vec3 out_normal;    \n\
                                               \n\
						out vec4 out_finalCol; \n\
						\n\n" + shaderHeaderPostfix;

#pragma endregion


//Used for the single-pass Material constructor.
std::vector<RenderingPass> tempHelperMaterialPassList;
const std::vector<RenderingPass> & MakeList(const RenderingPass & onlyElement)
{
    tempHelperMaterialPassList.clear();
    tempHelperMaterialPassList.insert(tempHelperMaterialPassList.begin(), onlyElement);
    return tempHelperMaterialPassList;
}


Material::Material(const std::vector<RenderingPass> & passes)
    : errorMsg("")
{
    for (int pass = 0; pass < passes.size(); ++pass)
    {
        //Record this pass's data.

        RenderObjHandle prog;
        ShaderHandler::CreateShaderProgram(prog);
        shaderPrograms.insert(shaderPrograms.end(), prog);

        PassSamplers samplers;
        textureSamplers.insert(textureSamplers.end(), samplers);

        renderStates.insert(renderStates.end(), passes[pass].RenderState);


        //Compile the shaders.

        RenderingPass mt = passes[pass];
        mt.VertexShader.insert(mt.VertexShader.begin(), vsHeader.begin(), vsHeader.end());
        mt.FragmentShader.insert(mt.FragmentShader.begin(), psHeader.begin(), psHeader.end());

        RenderObjHandle vS, fS;
        if (!ShaderHandler::CreateShader(prog, vS, mt.VertexShader.c_str(), GL_VERTEX_SHADER))
        {
            errorMsg = std::string("Couldn't create vertex shader: ") + ShaderHandler::GetErrorMessage();
            return;
        }
        if (!ShaderHandler::CreateShader(prog, fS, mt.FragmentShader.c_str(), GL_FRAGMENT_SHADER))
        {
            errorMsg = std::string("Couldn't create fragment shader: ") + ShaderHandler::GetErrorMessage();
            return;
        }

        glDeleteShader(vS);
        glDeleteShader(fS);

        if (!ShaderHandler::FinalizeShaders(prog))
        {
            errorMsg = std::string("Error finalizing shaders: ") + ShaderHandler::GetErrorMessage();
            ClearAllRenderingErrors();
            return;
        }


        //Get default uniforms.

        UniformLocation uLoc;
        std::unordered_map<std::string, UniformLocation> dummyMap;
        uniforms.insert(uniforms.end(), dummyMap);

        if (RenderDataHandler::GetUniformLocation(prog, "u_wvp", uLoc))
            uniforms[pass]["u_wvp"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_world", uLoc))
            uniforms[pass]["u_world"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_view", uLoc))
            uniforms[pass]["u_view"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_proj", uLoc))
            uniforms[pass]["u_proj"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_cam_pos", uLoc))
            uniforms[pass]["u_cam_pos"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_cam_forward", uLoc))
            uniforms[pass]["u_cam_forward"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_cam_upward", uLoc))
            uniforms[pass]["u_cam_upward"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_cam_sideways", uLoc))
            uniforms[pass]["u_cam_sideways"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_elapsed_seconds", uLoc))
            uniforms[pass]["u_elapsed_seconds"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_samplers2D", uLoc))
            uniforms[pass]["u_samplers2D"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_samplerScales", uLoc))
            uniforms[pass]["u_samplerScales"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_samplerPans", uLoc))
            uniforms[pass]["u_samplerPans"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_samplerOffsets", uLoc))
            uniforms[pass]["u_samplerOffsets"] = uLoc;
    }
}
Material::Material(const RenderingPass & shaders)
    : Material(MakeList(shaders))
{

}
Material::~Material(void)
{
    for (int i = 0; i < GetNumbPasses(); ++i)
    {
        glDeleteProgram(shaderPrograms[i]);
    }
}

bool Material::Render(const RenderInfo & info, const std::vector<const Mesh*> & meshes)
{
    ClearAllRenderingErrors();


    //Set up texture units.
    int samplerValues[MaterialConstants::TWODSAMPLERS];
    for (int i = 0; i < MaterialConstants::TWODSAMPLERS; ++i)
        samplerValues[i] = i;


    //Each mesh will has a different world matrix on top of the global world matrix in the RenderInfo.
    Matrix4f transMat, worldMat, vpMat, wvp;
    vpMat = Matrix4f::Multiply(*info.mProj, *info.mView);

    //Go through each mesh.
    for (int mesh = 0; mesh < meshes.size(); ++mesh)
    {
        //Calculate the transformation matrices for this mesh.
        meshes[mesh]->Transform.GetWorldTransform(transMat);
        worldMat = Matrix4f::Multiply(*info.mWorld, transMat);
        wvp = Matrix4f::Multiply(vpMat, worldMat);

        //Render each pass for this mesh.
        for (int pass = 0; pass < GetNumbPasses(); ++pass)
        {
            ShaderHandler::UseShader(shaderPrograms[pass]);


            //Set basic uniforms.

            TrySetUniformMat(pass, "u_world", worldMat);
            TrySetUniformMat(pass, "u_view", *info.mView);
            TrySetUniformMat(pass, "u_proj", *info.mProj);
            TrySetUniformMat(pass, "u_wvp", wvp);

            Vector3f camPos = info.Cam->GetPosition();
            TrySetUniformF(pass, "u_cam_pos", (float*)(&camPos[0]), 3);
            Vector3f dir = info.Cam->GetForward();
            TrySetUniformF(pass, "u_cam_forward", (float*)(&dir[0]), 3);
            dir = info.Cam->GetUpward();
            TrySetUniformF(pass, "u_cam_upward", (float*)(&dir[0]), 3);
            dir = info.Cam->GetSideways();
            TrySetUniformF(pass, "u_cam_sideways", (float*)(&dir[0]), 3);

            float seconds = info.World->GetTotalElapsedSeconds();
            TrySetUniformF(pass, "u_elapsed_seconds", &seconds, 1);


            //Set the mesh's special uniforms.
            for (auto iterator = meshes[mesh]->FloatUniformValues.begin(); iterator != meshes[mesh]->FloatUniformValues.end(); ++iterator)
                TrySetUniformF(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->IntUniformValues.begin(); iterator != meshes[mesh]->IntUniformValues.end(); ++iterator)
                TrySetUniformI(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->MatUniformValues.begin(); iterator != meshes[mesh]->MatUniformValues.end(); ++iterator)
                TrySetUniformMat(pass, iterator->first, iterator->second);
            for (auto iterator = meshes[mesh]->FloatArrayUniformValues.begin(); iterator != meshes[mesh]->FloatArrayUniformValues.end(); ++iterator)
                TrySetUniformArrayF(pass, iterator->first, iterator->second.Data, iterator->second.NArrayElements, iterator->second.NDataPerElement);
            for (auto iterator = meshes[mesh]->IntArrayUniformValues.begin(); iterator != meshes[mesh]->IntArrayUniformValues.end(); ++iterator)
                TrySetUniformArrayI(pass, iterator->first, iterator->second.Data, iterator->second.NArrayElements, iterator->second.NDataPerElement);


            //Enable textures that the shader uses.
            if (TrySetUniformArrayI(pass, "u_samplers2D", samplerValues, MaterialConstants::TWODSAMPLERS, 1))
            {
                for (unsigned int i = 0; i < MaterialConstants::TWODSAMPLERS; ++i)
                {
                    RenderDataHandler::ActivateTextureUnit(i);

                    //If the mesh has a sampler it wants to use, use that. Otherwise, use the default.
                    if (meshes[mesh]->TextureSamplers[pass].Samplers[i] != 0)
                    {
                        RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, meshes[mesh]->TextureSamplers[pass].Samplers[i]);
                        TrySetUniformArrayF(pass, "u_samplerScales", i, &meshes[mesh]->TextureSamplers[pass].Scales[i][0], 2);
                        TrySetUniformArrayF(pass, "u_samplerPans", i, &meshes[mesh]->TextureSamplers[pass].Panners[i][0], 2);
                        TrySetUniformArrayF(pass, "u_samplerOffsets", i, &meshes[mesh]->TextureSamplers[pass].Offsets[i][0], 2);
                    }
                    else
                    {
                        RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, textureSamplers[pass].Samplers[i]);
                        TrySetUniformArrayF(pass, "u_samplerScales", i, &textureSamplers[pass].Scales[i][0], 2);
                        TrySetUniformArrayF(pass, "u_samplerPans", i, &textureSamplers[pass].Panners[i][0], 2);
                        TrySetUniformArrayF(pass, "u_samplerOffsets", i, &textureSamplers[pass].Offsets[i][i], 2);
                    }
                }
            }

            if (CheckError("Error setting up rendering/textures: ")) return false;


            //Finally render the mesh.
            if (!Render(meshes[mesh], info, pass))
                return false;
        }
    }

    return true;
}
bool Material::Render(const Mesh * mesh, const RenderInfo & info, unsigned int pass)
{
    renderStates[pass].EnableState();

    //Render each vertex/index buffer.
    VertexIndexData dat;
    for (int j = 0; j < mesh->GetNumbVertexIndexData(); ++j)
    {
        dat = mesh->GetVertexIndexData(j);

        RenderDataHandler::BindVertexBuffer(dat.GetVerticesHandle());

        Vertex::EnableVertexAttributes();

        if (dat.UsesIndices())
        {
            RenderDataHandler::BindIndexBuffer(dat.GetIndicesHandle());
            ShaderHandler::DrawIndexedVertices(mesh->GetPrimType(), dat.GetIndicesCount());
        }
        else
        {
            ShaderHandler::DrawVertices(mesh->GetPrimType(), dat.GetVerticesCount(), sizeof(int)* dat.GetFirstVertex());
        }

        Vertex::DisableVertexAttributes();

        if (CheckError("Error rendering mesh: ")) return false;
    }

    return true;
}

bool Material::TryAddUniform(unsigned int programIndex, std::string uniform)
{
    if (uniforms[programIndex].find(uniform) != uniforms[programIndex].end()) return false;

    UniformLocation temp;
    bool b = RenderDataHandler::GetUniformLocation(shaderPrograms[programIndex], uniform.c_str(), temp);

    uniforms[programIndex][uniform] = temp;
    return b;
}

*/