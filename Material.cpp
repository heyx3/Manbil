#include "Material.h"

#include "Vertex.h"
#include "RenderDataHandler.h"

typedef std::unordered_map<std::string, UniformLocation> UniformLocMap;


void Material::InitializeMaterialDrawing(void)
{
	Vertex::EnableVertexAttributes();
}
void Material::EndMaterialDrawing(void)
{
	Vertex::DisableVertexAttributes();
}

Material::Material(std::string vs, std::string ps)
{
	//Initialize data.

	errorMsg = "";
	ShaderHandler::CreateShaderProgram(shaderProgram);
	for (int i = 0; i < TWODSAMPLERS; ++i)
		TextureSamplers[i] = -1;



	//Add some declarations before the shaders.

	std::string vsHeader = "#version 330		\n\
							\n\
							layout (location = 0) in vec3 in_pos;\n\
							layout (location = 1) in vec4 in_col;\n\
							layout (location = 2) in vec2 in_tex;\n\
							layout (location = 3) in vec3 in_normal;\n\
							\n\
							uniform mat4 u_wvp;\n\
							uniform mat4 u_world;\n\
							uniform mat4 u_view;\n\
							uniform mat4 u_proj;\n\
							uniform vec3 u_cam_pos;\n\
							uniform float u_elapsed_seconds;\n\
							\n\
							uniform sampler2D u_sampler0;\n\
							uniform sampler2D u_sampler1;\n\
							uniform sampler2D u_sampler2;\n\
							uniform sampler2D u_sampler3;\n\
							uniform sampler2D u_sampler4;\n\
							\n\
							out vec3 out_pos;\n\
							out vec4 out_col;\n\
							out vec2 out_tex;\n\
							out vec3 out_normal;\n\
							\n\
							vec4 worldTo4DScreen(vec3 world) { return u_wvp * vec4(world, 1.0); }\n\
							vec3 fourDScreenTo3DScreen(vec4 screen4D) { return screen4D.xyz / screen4D.w; }\n\
							vec3 worldTo3DScreen(vec3 world) { return fourDScreenTo3DScreen(worldTo4DScreen(world)); }\n\
							\n";
	std::string psHeader = "#version 330		\n\
							\n\
							in vec3 out_pos;\n\
							in vec4 out_col;\n\
							in vec2 out_tex;\n\
							in vec3 out_normal;\n\
							\n\
							uniform mat4 u_wvp;\n\
							uniform mat4 u_world;\n\
							uniform mat4 u_view;\n\
							uniform mat4 u_proj;\n\
							uniform vec3 u_cam_pos;\n\
							uniform float u_elapsed_seconds;\n\
							\n\
							uniform sampler2D u_sampler0;\n\
							uniform sampler2D u_sampler1;\n\
							uniform sampler2D u_sampler2;\n\
							uniform sampler2D u_sampler3;\n\
							uniform sampler2D u_sampler4;\n\
							\n\
							out vec4 out_finalCol;\n\
							\n\
							vec4 worldTo4DScreen(vec3 world) { return u_wvp * vec4(world, 1.0); }\n\
							vec3 fourDScreenTo3DScreen(vec4 screen4D) { return screen4D.xyz / screen4D.w; }\n\
							vec3 worldTo3DScreen(vec3 world) { return fourDScreenTo3DScreen(worldTo4DScreen(world)); }\n\
							float getBrightness(vec3 surfaceNormal, vec3 camToFragNormal, vec3 lightDirNormal,\n\
												float ambient, float diffuse, float specular, float specularIntensity)	\n\
							{\n\
								float dotted = max(dot(-surfaceNormal, lightDirNormal), 0.0);\n\
								\n\
								vec3 fragToCam = -camToFragNormal;\n\
								vec3 lightReflect = normalize(reflect(lightDirNormal, surfaceNormal));\n\
								\n\
								float specFactor = max(0.0, dot(fragToCam, lightReflect));\n\
								specFactor = pow(specFactor, specularIntensity);\n\
								\n\
								return ambient + (diffuse * dotted) + (specular * specFactor);\n\
							}\n\
							\n";

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
	//TODO: Did not mean to do the error-checking for setting uniforms. Just call the "SetUniform[X]" function without error checking.

	if ((!SetUniformMat("u_view", *rendInfo.mView) && false) ||
		(!SetUniformMat("u_proj", *rendInfo.mProj) && false))
	{
		errorMsg = "Error setting 'u_view' and 'u_proj'.";
		return false;
	}

	Vector3f camPos = rendInfo.Cam->GetPosition();
	if (!SetUniformF("u_cam_pos", (float*)(&camPos[0]), 3) && false)
	{
		errorMsg = "Error setting 'u_cam_pos'.";
		return false;
	}
	
	float seconds = rendInfo.World->GetTotalElapsedSeconds();
	if (!SetUniformF("u_elapsed_seconds", &seconds, 1) && false)
	{
		errorMsg = "Error setting 'u_elapsed_seconds'.";
		return false;
	}

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
		if (!SetUniformMat("u_world", worldTotal) && false)
		{
			errorMsg = "Error setting 'u_world'.";
			return false;
		}

		//Calculate wvp transform.
		wvp = Matrix4f::Multiply(vp, worldTotal);
		if (!SetUniformMat("u_wvp", wvp) && false)
		{
			errorMsg = "Error setting 'u_wvp'.";
			return false;
		}

		if (CheckError("Error setting world/wvp uniforms for a mesh: ")) return false;

		//Render.
		for (int j = 0; j < meshes[i]->GetNumbVertexIndexData(); ++j)
		{
			dat = meshes[i]->GetVertexIndexData(j);

			RenderDataHandler::BindVertexBuffer(dat.GetVerticesHandle());

			if (dat.UsesIndices())
			{
				RenderDataHandler::BindIndexBuffer(dat.GetIndicesHandle());
				ShaderHandler::DrawIndexedVertices(meshes[i]->GetPrimType(), dat.GetIndicesCount());
			}
			else
			{
				ShaderHandler::DrawVertices(meshes[i]->GetPrimType(), dat.GetVerticesCount(), dat.GetFirstVertex());
			}

			if (CheckError("Error rendering mesh: ")) return false;
		}
	}

	if (CheckError("Unknown rendering error: ")) return false;

	return true;
}