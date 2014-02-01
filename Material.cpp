#include "Material.h"

#include "Vertex.h"
#include "Mesh.h"
#include "RenderDataHandler.h"

typedef std::unordered_map<std::string, UniformLocation> UniformLocMap;

#pragma region Shader headers

std::string shaderHeaderPostfix = std::string() +
                       "uniform mat4 u_wvp;                                                                       \n\
						uniform mat4 u_world;                                                                     \n\
						uniform mat4 u_view;                                                                      \n\
						uniform mat4 u_proj;                                                                      \n\
						uniform vec3 u_cam_pos;                                                                   \n\
						uniform float u_elapsed_seconds;                                                          \n\
						                                                                                          \n\
						uniform sampler2D u_sampler0;                                                             \n\
						uniform sampler2D u_sampler1;                                                             \n\
						uniform sampler2D u_sampler2;                                                             \n\
						uniform sampler2D u_sampler3;                                                             \n\
						uniform sampler2D u_sampler4;                                                             \n\
						                                                                                          \n\
						                                                                                          \n\
						vec4 worldTo4DScreen(vec3 world) { return u_wvp * vec4(world, 1.0); }                     \n\
						vec3 fourDScreenTo3DScreen(vec4 screen4D) { return screen4D.xyz / screen4D.w; }           \n\
						vec3 worldTo3DScreen(vec3 world) { return fourDScreenTo3DScreen(worldTo4DScreen(world)); }\n\
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
std::vector<RenderingPass> MakeList(const RenderingPass & onlyElement)
{
    tempHelperMaterialPassList.clear();
    tempHelperMaterialPassList.insert(tempHelperMaterialPassList.begin(), onlyElement);
    return tempHelperMaterialPassList;
}



Material::Material(std::vector<RenderingPass> passes)
    : errorMsg("")
{
    for (int pass = 0; pass < passes.size(); ++pass)
    {
        //Create the program.

        BufferObjHandle prog;
        ShaderHandler::CreateShaderProgram(prog);
        shaderPrograms.insert(shaderPrograms.end(), prog);

        PassSamplers samplers;
        textureSamplers.insert(textureSamplers.end(), samplers);


        //Compile the shaders.

        RenderingPass mt = passes[pass];
        mt.VertexShader.insert(mt.VertexShader.begin(), vsHeader.begin(), vsHeader.end());
        mt.FragmentShader.insert(mt.FragmentShader.begin(), psHeader.begin(), psHeader.end());

        BufferObjHandle vS, fS;
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
        if (RenderDataHandler::GetUniformLocation(prog, "u_elapsed_seconds", uLoc))
            uniforms[pass]["u_elapsed_seconds"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_sampler0", uLoc))
            uniforms[pass]["u_sampler0"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_sampler1", uLoc))
            uniforms[pass]["u_sampler1"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_sampler2", uLoc))
            uniforms[pass]["u_sampler2"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_sampler3", uLoc))
            uniforms[pass]["u_sampler3"] = uLoc;
        if (RenderDataHandler::GetUniformLocation(prog, "u_sampler4", uLoc))
            uniforms[pass]["u_sampler4"] = uLoc;
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

    //Each mesh will has a different world matrix on top of the global world matrix in the RenderInfo.
    Matrix4f transMat, worldMat, vpMat, wvp;
    vpMat = Matrix4f::Multiply(*info.mProj, *info.mView);

    //Go through each mesh.
    for (int mesh = 0; mesh < meshes.size(); ++mesh)
    {
        //Calculate the transformation matrices for this mesh.
        meshes[mesh]->Transform.GetWorldTransform(transMat);
        worldMat = Matrix4f::Multiply(worldMat, transMat);
        wvp = Matrix4f::Multiply(vpMat, worldMat);

        //Render each pass for this mesh.
        for (int pass = 0; pass < GetNumbPasses(); ++pass)
        {
            ShaderHandler::UseShader(shaderPrograms[pass]);


            //Set basic uniforms.

            TrySetUniformMat(pass, "u_view", *info.mView);
            TrySetUniformMat(pass, "u_proj", *info.mProj);

            Vector3f camPos = info.Cam->GetPosition();
            TrySetUniformF(pass, "u_cam_pos", (float*)(&camPos[0]), 3);

            float seconds = info.World->GetTotalElapsedSeconds();
            TrySetUniformF(pass, "u_elapsed_seconds", &seconds, 1);

            TrySetUniformMat(pass, "u_world", worldMat);
            TrySetUniformMat(pass, "u_wvp", wvp);


            //Set the mesh's special uniforms.
            for (auto iterator = meshes[mesh]->FloatUniformValues.begin(); iterator != meshes[mesh]->FloatUniformValues.end(); ++iterator)
                TrySetUniformF(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->IntUniformValues.begin(); iterator != meshes[mesh]->IntUniformValues.end(); ++iterator)
                TrySetUniformI(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->MatUniformValues.begin(); iterator != meshes[mesh]->MatUniformValues.end(); ++iterator)
                TrySetUniformMat(pass, iterator->first, iterator->second);


            //Enable any textures.
            for (int i = 0; i < MaterialConstants::TWODSAMPLERS; ++i)
            {
                UniformLocMap::const_iterator found = uniforms[pass].find(std::string("u_sampler") + std::to_string(i));
                if (found != uniforms[pass].end())
                {
                    glUniform1i(found->second, i);
                    RenderDataHandler::ActivateTextureUnit(i);

                    if (meshes[mesh]->TextureSamplers[pass].Samplers[i] == 0)
                    {
                        RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, textureSamplers[pass].Samplers[i]);
                    }
                    else
                    {
                        RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, meshes[mesh]->TextureSamplers[pass].Samplers[i]);
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