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


Material2::Material2(std::vector<MaterialShaders> passes)
    : errorMsg("")
{
    for (int pass = 0; pass < passes.size(); ++pass)
    {
        //Create the program.

        BufferObjHandle prog;
        ShaderHandler::CreateShaderProgram(prog);
        shaderPrograms.insert(shaderPrograms.end(), prog);

        BufferObjHandle dummySamplers[TWODSAMPLERS];
        for (int j = 0; j < TWODSAMPLERS; ++j)
            dummySamplers[j] = 0;
        textureSamplers.insert(textureSamplers.end(), dummySamplers);


        //Compile the shaders.

        MaterialShaders mt = passes[pass];
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
Material2::~Material2(void)
{
    for (int i = 0; i < GetNumbPasses(); ++i)
    {
        glDeleteProgram(shaderPrograms[i]);
    }
}

bool Material2::Render(const RenderInfo & info, const std::vector<const Mesh*> & meshes)
{
    ClearAllRenderingErrors();

    //Set some basic uniforms.

    SetUniformMat("u_view", *info.mView);
    SetUniformMat("u_proj", *info.mProj);

    Vector3f camPos = info.Cam->GetPosition();
    SetUniformF("u_cam_pos", (float*)(&camPos[0]), 3);

    float seconds = info.World->GetTotalElapsedSeconds();
    SetUniformF("u_elapsed_seconds", &seconds, 1);


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

        for (int pass = 0; pass < GetNumbPasses(); ++pass)
        {
            //Set matrix data.
            TrySetUniformMat(pass, "u_world", worldMat);
            TrySetUniformMat(pass, "u_wvp", wvp);

            //Set the mesh's special uniforms.
            for (auto iterator = meshes[mesh]->FloatUniformValues.begin(); iterator != meshes[mesh]->FloatUniformValues.end(); ++iterator)
                TrySetUniformF(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->IntUniformValues.begin(); iterator != meshes[mesh]->IntUniformValues.end(); ++iterator)
                TrySetUniformI(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->MatUniformValues.begin(); iterator != meshes[mesh]->MatUniformValues.end(); ++iterator)
                TrySetUniformMat(pass, iterator->first, iterator->second);

            if (!Render(meshes[mesh], info, pass))
                return false;
        }
    }

    return true;
    



    //TODO: Change to iterating by mesh, THEN by pass, for performance reasons (specifically, setting uniforms). After doing this, this function can just loop through each mesh and call a Render() overload on each individual mesh.
    ClearAllRenderingErrors();



    //Pre-compute matrix math.

    Matrix4f vp = Matrix4f::Multiply(*info.mProj, *info.mView);
    Matrix4f tempMat;
    //std::vector<Matrix4f> worldTotal, wvp;


    //Iterate through each pass.
    for (unsigned int pass = 0; pass < GetNumbPasses(); ++pass)
    {
        //Prepare the shader.
        ShaderHandler::UseShader(shaderPrograms[pass]);

        //Render each mesh.
        VertexIndexData dat;
        for (unsigned int mesh = 0; mesh < meshes.size(); ++mesh)
        {
            //Set up samplers.
            for (int i = 0; i < TWODSAMPLERS; ++i)
            {
                UniformLocMap::const_iterator found = uniforms[pass].find(std::string("u_sampler") + std::to_string(i));
                if (found != uniforms[pass].end())
                {
                    glUniform1i(found->second, i);
                    RenderDataHandler::ActivateTextureUnit(i);
                    //RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, meshes[mesh]->TextureSamplers[pass]);
                }
            }
            if (CheckError("Error setting up rendering/textures: ")) return false;

            //Pre-compute matrix data.
            if (pass == 0)
            {
                //World/object transform.
                meshes[mesh]->Transform.GetWorldTransform(tempMat);
                //worldTotal.insert(worldTotal.end(), Matrix4f::Multiply(*info.mWorld, tempMat));
                //WVP transform.
                //wvp.insert(wvp.end(), Matrix4f::Multiply(vp, worldTotal[mesh]));
            }

            //Set matrix data.
            //TrySetUniformMat(pass, "u_world", worldTotal[mesh]);
            //TrySetUniformMat(pass, "u_wvp", wvp[mesh]);

            //Set the mesh's special uniforms.
            for (auto iterator = meshes[mesh]->FloatUniformValues.begin(); iterator != meshes[mesh]->FloatUniformValues.end(); ++iterator)
                TrySetUniformF(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->IntUniformValues.begin(); iterator != meshes[mesh]->IntUniformValues.end(); ++iterator)
                TrySetUniformI(pass, iterator->first, iterator->second.Data, iterator->second.NData);
            for (auto iterator = meshes[mesh]->MatUniformValues.begin(); iterator != meshes[mesh]->MatUniformValues.end(); ++iterator)
                TrySetUniformMat(pass, iterator->first, iterator->second);

            //Render each mesh's vertex/index buffer.
            for (int j = 0; j < meshes[mesh]->GetNumbVertexIndexData(); ++j)
            {
                dat = meshes[mesh]->GetVertexIndexData(j);

                RenderDataHandler::BindVertexBuffer(dat.GetVerticesHandle());

                Vertex::EnableVertexAttributes();

                if (dat.UsesIndices())
                {
                    RenderDataHandler::BindIndexBuffer(dat.GetIndicesHandle());
                    ShaderHandler::DrawIndexedVertices(meshes[mesh]->GetPrimType(), dat.GetIndicesCount());
                }
                else
                {
                    ShaderHandler::DrawVertices(meshes[mesh]->GetPrimType(), dat.GetVerticesCount(), sizeof(int)* dat.GetFirstVertex());
                }

                Vertex::DisableVertexAttributes();

                if (CheckError("Error rendering mesh: ")) return false;
            }
        }
    }

    return true;
}
bool Material2::Render(unsigned int pass, const RenderInfo & info, const std::vector<const Mesh*> & meshes)
{
    assert(pass < GetNumbPasses());
    ClearAllRenderingErrors();


    //Prepare the shader.
    ShaderHandler::UseShader(shaderPrograms[pass]);


    //Set some basic uniforms.

    TrySetUniformMat(pass, "u_view", *info.mView);
    TrySetUniformMat(pass, "u_proj", *info.mProj);

    Vector3f camPos = info.Cam->GetPosition();
    TrySetUniformF(pass, "u_cam_pos", (float*)(&camPos[0]), 3);

    float seconds = info.World->GetTotalElapsedSeconds();
    TrySetUniformF(pass, "u_elapsed_seconds", &seconds, 1);


    //Do some matrix math before rendering each mesh.

    Matrix4f vp = Matrix4f::Multiply(*info.mProj, *info.mView),
        meshTrans, worldTotal, wvp;
    VertexIndexData dat;
    for (unsigned int mesh = 0; mesh < meshes.size(); ++mesh)
    {
        //Enable any textures.
        for (int i = 0; i < TWODSAMPLERS; ++i)
        {
            UniformLocMap::const_iterator found = uniforms[pass].find(std::string("u_sampler") + std::to_string(i));
            if (found != uniforms[pass].end())
            {
                glUniform1i(found->second, i);
                RenderDataHandler::ActivateTextureUnit(i);
                //RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, meshes[mesh]->TextureSamplers[pass]);
            }
        }

        if (CheckError("Error setting up rendering/textures: ")) return false;


        //Combine the mesh's transform with the world transform.
        meshes[mesh]->Transform.GetWorldTransform(meshTrans);
        worldTotal = Matrix4f::Multiply(*info.mWorld, meshTrans);
        TrySetUniformMat(pass, "u_world", worldTotal);

        //Calculate wvp transform.
        wvp = Matrix4f::Multiply(vp, worldTotal);
        TrySetUniformMat(pass, "u_wvp", wvp);

        //Set the mesh's special uniforms.
        for (auto iterator = meshes[mesh]->FloatUniformValues.begin(); iterator != meshes[mesh]->FloatUniformValues.end(); ++iterator)
            TrySetUniformF(pass, iterator->first, iterator->second.Data, iterator->second.NData);
        for (auto iterator = meshes[mesh]->IntUniformValues.begin(); iterator != meshes[mesh]->IntUniformValues.end(); ++iterator)
            TrySetUniformI(pass, iterator->first, iterator->second.Data, iterator->second.NData);
        for (auto iterator = meshes[mesh]->MatUniformValues.begin(); iterator != meshes[mesh]->MatUniformValues.end(); ++iterator)
            TrySetUniformMat(pass, iterator->first, iterator->second);

        //Render each mesh's vertex/index buffer.
        for (int j = 0; j < meshes[mesh]->GetNumbVertexIndexData(); ++j)
        {
            dat = meshes[mesh]->GetVertexIndexData(j);

            RenderDataHandler::BindVertexBuffer(dat.GetVerticesHandle());

            Vertex::EnableVertexAttributes();

            if (dat.UsesIndices())
            {
                RenderDataHandler::BindIndexBuffer(dat.GetIndicesHandle());
                ShaderHandler::DrawIndexedVertices(meshes[mesh]->GetPrimType(), dat.GetIndicesCount());
            }
            else
            {
                ShaderHandler::DrawVertices(meshes[mesh]->GetPrimType(), dat.GetVerticesCount(), sizeof(int)* dat.GetFirstVertex());
            }

            Vertex::DisableVertexAttributes();

            if (CheckError("Error rendering mesh: ")) return false;
        }
    }

    return true;
}
bool Material2::Render(const Mesh * mesh, const RenderInfo & info, unsigned int pass)
{
    //Enable any textures.
    for (int i = 0; i < TWODSAMPLERS; ++i)
    {
        UniformLocMap::const_iterator found = uniforms[pass].find(std::string("u_sampler") + std::to_string(i));
        if (found != uniforms[pass].end())
        {
            glUniform1i(found->second, i);
            RenderDataHandler::ActivateTextureUnit(i);
            RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, mesh->TextureSamplers[pass][i]);
        }
    }

    if (CheckError("Error setting up rendering/textures: ")) return false;

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

bool Material2::TryAddUniform(unsigned int programIndex, std::string uniform)
{
    if (uniforms[programIndex].find(uniform) != uniforms[programIndex].end()) return false;

    UniformLocation temp;
    bool b = RenderDataHandler::GetUniformLocation(shaderPrograms[programIndex], uniform.c_str(), temp);

    uniforms[programIndex][uniform] = temp;
    return b;
}




Material::Material(std::string vs, std::string ps)
{
	//Initialize data.

	errorMsg = "";
	ShaderHandler::CreateShaderProgram(shaderProgram);
	for (int i = 0; i < TWODSAMPLERS; ++i)
		TextureSamplers[i] = 0;

    vs.insert(vs.begin(), vsHeader.begin(), vsHeader.end());
    ps.insert(ps.begin(), psHeader.begin(), psHeader.end());

	//Create shaders.

	GLuint vsObj;
	if (!ShaderHandler::CreateShader(shaderProgram, vsObj, vs.c_str(), GL_VERTEX_SHADER))
	{
		errorMsg = std::string("Couldn't create vertex shader: ") + ShaderHandler::GetErrorMessage();
		return;
	}

	GLuint psObj;
	if (!ShaderHandler::CreateShader(shaderProgram, psObj, ps.c_str(), GL_FRAGMENT_SHADER))
	{
		errorMsg = std::string("Couldn't create fragment shader: ") + ShaderHandler::GetErrorMessage();
		return;
	}

	glDeleteShader(vsObj);
	glDeleteShader(psObj);


	//Try to finalize/link the shaders.

	if (!ShaderHandler::FinalizeShaders(shaderProgram, true))
	{
		errorMsg = std::string("Error finalizing shaders: ") + ShaderHandler::GetErrorMessage();
		return;
	}



	//Get default uniforms.

	UniformLocation uLoc;
	
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_wvp", uLoc))
		uniforms["u_wvp"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_world", uLoc))
		uniforms["u_world"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_view", uLoc))
		uniforms["u_view"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_proj", uLoc))
		uniforms["u_proj"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_cam_pos", uLoc))
		uniforms["u_cam_pos"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_elapsed_seconds", uLoc))
		uniforms["u_elapsed_seconds"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_sampler0", uLoc))
		uniforms["u_sampler0"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_sampler1", uLoc))
		uniforms["u_sampler1"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_sampler2", uLoc))
		uniforms["u_sampler2"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_sampler3", uLoc))
		uniforms["u_sampler3"] = uLoc;
	if (RenderDataHandler::GetUniformLocation(shaderProgram, "u_sampler4", uLoc))
		uniforms["u_sampler4"] = uLoc;
}
Material::Material(const Material & cpy)
{
	if (cpy.HasError())
	{
		errorMsg = std::string("Tried to create a copy of a material with the error: ") + cpy.GetErrorMessage();
		return;
	}

	for (int i = 0; i < TWODSAMPLERS; ++i)
		TextureSamplers[i] = cpy.TextureSamplers[i];
	shaderProgram = cpy.shaderProgram;

	uniforms = cpy.uniforms;
}

void Material::DeleteMaterial(void)
{
	glDeleteProgram(shaderProgram);
}

bool Material::AddUniform(std::string uniformName)
{
	UniformLocMap::const_iterator found = uniforms.find(uniformName);

	//If the uniform does not already exist, find it.
	if (found == uniforms.end())
	{		
		UniformLocation loc;
		if (!RenderDataHandler::GetUniformLocation(shaderProgram, uniformName.c_str(), loc))
		{
			return false;
		}

		uniforms[uniformName] = loc;
	}
	//Otherwise, error.
	else
	{
		return false;
	}

	return true;
}

bool Material::SetUniformF(std::string uniform, const float * start, int numbFloats)
{
	UniformLocMap::const_iterator found = uniforms.find(uniform);

	if (found == uniforms.end())
	{
		return false;
	}

	UniformLocation loc = (*found).second;
	RenderDataHandler::SetUniformValue(loc, numbFloats, start);

	return true;
}
bool Material::SetUniformI(std::string uniform, const int * start, int numbInts)
{
	UniformLocMap::const_iterator found = uniforms.find(uniform);

	if (found == uniforms.end())
	{
		return false;
	}

	UniformLocation loc = (*found).second;
	RenderDataHandler::SetUniformValue(loc, numbInts, start);

	return true;
}
bool Material::SetUniformMat(std::string uniform, const Matrix4f & matrix)
{
	UniformLocMap::const_iterator found = uniforms.find(uniform);

	if (found == uniforms.end())
	{
		return false;
	}

	UniformLocation loc = (*found).second;
	RenderDataHandler::SetMatrixValue(loc, matrix);

	return true;
}

bool Material::Render(const RenderInfo & rendInfo, const std::vector<const Mesh*> & meshes)
{
	ClearAllRenderingErrors();

	//Prepare the shader.
	ShaderHandler::UseShader(shaderProgram);
	if (CheckError("Error using this material: ")) return false;


	//Set some uniforms.
    SetUniformMat("u_view", *rendInfo.mView);
    SetUniformMat("u_proj", *rendInfo.mProj);

	Vector3f camPos = rendInfo.Cam->GetPosition();
    SetUniformF("u_cam_pos", (float*)(&camPos[0]), 3);
	
	float seconds = rendInfo.World->GetTotalElapsedSeconds();
    SetUniformF("u_elapsed_seconds", &seconds, 1);

	if (CheckError("Unknown error setting basic uniforms: ")) return false;


	//Enable any textures.
	int texUnit;
	for (int i = 0; i < TWODSAMPLERS; ++i)
	{
		UniformLocMap::const_iterator found = uniforms.find(std::string("u_sampler") + std::to_string(i));
		if (found != uniforms.end())
		{
			if (TextureSamplers[i] < 0)
			{
				errorMsg = std::string("Texture sampler ") + std::to_string(i) + " is invalid";
				return false;
			}

			glGetUniformiv(shaderProgram, found->second, &texUnit);
			RenderDataHandler::ActivateTextureUnit(texUnit);
			RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, TextureSamplers[i]);
		}
	}

	if (CheckError("Error activating texture units and binding textures: ")) return false;


	//Do some matrix math before rendering the meshes.

	Matrix4f meshTrans, worldTotal, vp, wvp;

	vp = Matrix4f::Multiply(*rendInfo.mProj, *rendInfo.mView);


	//Render each mesh.

	VertexIndexData dat;
	for (int i = 0; i < meshes.size(); ++i)
	{
		//Combine the mesh's transform with the world transform.
		meshes[i]->Transform.GetWorldTransform(meshTrans);
		worldTotal = Matrix4f::Multiply(*rendInfo.mWorld, meshTrans);
        SetUniformMat("u_world", worldTotal);

		//Calculate wvp transform.
		wvp = Matrix4f::Multiply(vp, worldTotal);
        SetUniformMat("u_wvp", wvp);

		if (CheckError("Error setting world/wvp uniforms for a mesh: ")) return false;

		//Render.
		for (int j = 0; j < meshes[i]->GetNumbVertexIndexData(); ++j)
		{
			dat = meshes[i]->GetVertexIndexData(j);

			RenderDataHandler::BindVertexBuffer(dat.GetVerticesHandle());

            Vertex::EnableVertexAttributes();

			if (dat.UsesIndices())
			{
				RenderDataHandler::BindIndexBuffer(dat.GetIndicesHandle());
				ShaderHandler::DrawIndexedVertices(meshes[i]->GetPrimType(), dat.GetIndicesCount());
			}
			else
			{
				ShaderHandler::DrawVertices(meshes[i]->GetPrimType(), dat.GetVerticesCount(), sizeof(int) * dat.GetFirstVertex());
			}

            Vertex::DisableVertexAttributes();

            //RenderDataHandler::BindVertexBuffer();
            //RenderDataHandler::BindIndexBuffer();

			if (CheckError("Error rendering mesh: ")) return false;
		}
	}

	if (CheckError("Unknown rendering error: ")) return false;

	return true;
}