#include "TwoTrianglesMaterial.h"

#include "Vertices.h"
#include "RenderingState.h"
#include "ShaderHandler.h"


typedef TwoTrianglesMaterial TTM;

const string TTM::Uniform_Time = "u_elapsed_seconds",
			 TTM::Uniform_MousePos = "u_mouse_pos",
			 TTM::Uniform_Resolution = "u_resolution",
			 TTM::Uniform_CamPos = "u_cam_pos",
			 TTM::Uniform_CamForward = "u_cam_forward",
			 TTM::Uniform_CamUp = "u_cam_up",
			 TTM::Uniform_CamSideways = "u_cam_side",
			 TTM::Uniform_Sampler[TTM::AvailableSamplers] =
				{
					"u_sampler_0",
					"u_sampler_1",
					"u_sampler_2",
					"u_sampler_3",
				},
			 TTM::In_UV = "in_uv_coords",
			 TTM::Out_Color = "out_color4";



bool TTM::CheckError(const char * errIntro) const
{
	const char * err = GetCurrentRenderingError();

	if (err[0] == '\0') return true;

	errorMsg = errIntro;
	errorMsg = ": ";
	errorMsg += err;

	return false;
}

TTM::TwoTrianglesMaterial(string fShader)
{
	ClearAllRenderingErrors();

	//Create vertex buffer.

	typedef VertexPosTex1 VPT;
	VPT vertices[6];
	vertices[0] = VPT(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(0.0f, 0.0f));
	vertices[1] = VPT(Vector3f(-1.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
	vertices[2] = VPT(Vector3f(1.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f));
	vertices[3] = vertices[0];
	vertices[4] = VPT(Vector3f(1.0f, -1.0f, 0.0f), Vector2f(1.0f, 0.0f));
	vertices[5] = vertices[2];

	RenderDataHandler::CreateVertexBuffer(vbo, vertices, 6, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	if (!CheckError("Error creating vertex buffer")) return;


	//Create shaders.

	string vs = string("#version 330	\n\
				 \n\
				 layout (location = 0) in vec3 Position;		\n\
				 layout (location = 1) in vec2 TexCoord;	\n\
				 \n\
				 out vec2 ") + In_UV + ";		\n\
				 void main()		\n\
				 {	\n\
					" + In_UV + " = TexCoord;	\n\
					gl_Position = vec4(Position.xyz, 1.0);	\n\
				 }";
	
	string samplerUniformDeclarations = "";
	for (unsigned int i = 0; i < AvailableSamplers; ++i)
	{
		samplerUniformDeclarations += "uniform sampler2D ";
		samplerUniformDeclarations += Uniform_Sampler[i];
		samplerUniformDeclarations += ";\n";
	}
	string fs = string("#version 330				\n") + "\
				\n\
				uniform float " + Uniform_Time + ";\n\
				uniform vec2 " + Uniform_MousePos + ";\n\
				uniform vec2 " + Uniform_Resolution + ";\n\
				uniform vec3 " + Uniform_CamPos + ", " + Uniform_CamForward + ", " + Uniform_CamUp + ", " + Uniform_CamSideways + ";\n\
				" + samplerUniformDeclarations + "\n\
				\n\
				in vec2 " + In_UV + ";				\n\
				\n\
				out vec4 " + Out_Color + ";			\n\
				\n\
				\n" +
				fShader;

	GLuint vsObj, fsObj;
	if (!ShaderHandler::CreateShaderProgram(shaderProg))
	{
		errorMsg = "Error creating shader program: ";
		errorMsg += ShaderHandler::GetErrorMessage();
		return;
	}
	if (!ShaderHandler::CreateShader(shaderProg, vsObj, vs.c_str(), GL_VERTEX_SHADER))
	{
		errorMsg = "Error creating vertex shader: ";
		errorMsg += ShaderHandler::GetErrorMessage();
		return;
	}
	if (!ShaderHandler::CreateShader(shaderProg, fsObj, fs.c_str(), GL_FRAGMENT_SHADER))
	{
		errorMsg = "Error creating fragment shader: ";
		errorMsg += ShaderHandler::GetErrorMessage();
		return;
	}
	if (!ShaderHandler::FinalizeShaders(shaderProg))
	{
		errorMsg = "Error finalizing shaders: ";
		errorMsg += ShaderHandler::GetErrorMessage();
		return;
	}

	glDeleteShader(vsObj);
	glDeleteShader(fsObj);

	ShaderHandler::UseShader(shaderProg);


	//Get all uniforms that are used.

	DH::GetUniformLocation(shaderProg, Uniform_Time.c_str(), timeLoc);
	DH::GetUniformLocation(shaderProg, Uniform_CamPos.c_str(), camPosLoc);
	DH::GetUniformLocation(shaderProg, Uniform_CamForward.c_str(), camFLoc);
	DH::GetUniformLocation(shaderProg, Uniform_CamUp.c_str(), camULoc);
	DH::GetUniformLocation(shaderProg, Uniform_CamSideways.c_str(), camSLoc);
	DH::GetUniformLocation(shaderProg, Uniform_MousePos.c_str(), mPosLoc);
	DH::GetUniformLocation(shaderProg, Uniform_Resolution.c_str(), resLoc);
	for (unsigned int i = 0; i < AvailableSamplers; ++i)
	{
		DH::GetUniformLocation(shaderProg, Uniform_Sampler[i].c_str(), samplerLocs[i]);
	}
	ClearAllRenderingErrors();


	//Set the texture unit values for samplers.

	for (int i = 0; i < AvailableSamplers; ++i)
	{
		SetSamplerTextureUnit(i, i);
	}
}


bool TTM::SetCamera(const MovingCamera & cam) const
{
	if (UsesCameraPos()) glUniform3f(camPosLoc, cam.GetPosition().x, cam.GetPosition().y, cam.GetPosition().z);
	if (UsesCameraForward()) glUniform3f(camFLoc, cam.GetForward().x, cam.GetForward().y, cam.GetForward().z);
	if (UsesCameraUp()) glUniform3f(camULoc, cam.GetUpward().x, cam.GetUpward().y, cam.GetUpward().z);
	if (UsesCameraSide()) glUniform3f(camSLoc, cam.GetSideways().x, cam.GetSideways().y, cam.GetSideways().z);

	return CheckError("Error setting 'camera' uniform");
}

bool TTM::Render(void) const
{
	//Set up render state.
	RenderingState(false).EnableState();
	ShaderHandler::UseShader(shaderProg);
	if (!CheckError("Error setting up render state and shaders: ")) return false;

	//Set up textures.
	for (unsigned int i = 0; i < AvailableSamplers; ++i)
	{
		if (!UsesSampler(i)) continue;

		RenderDataHandler::ActivateTextureUnit(samplerTexUnits[i]);
		if (!CheckError((string("Error setting active texture unit ") + std::to_string(i) + " for TTM mat").c_str()))
		{
			return false;
		}

		RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, textures[i]);
		if (!CheckError((string("Error binding texture for unit ") + std::to_string(i) + " for TTM mat").c_str()))
		{
			return false;
		}
	}


	//Draw the screen quad.

	VertexPosTex1::EnableAttributes();

	RenderDataHandler::BindVertexBuffer(vbo);
	ShaderHandler::DrawVertices(PrimitiveTypes::Triangles, 6);

	VertexPosTex1::DisableAttributes();

	return CheckError("Error drawing screen quad");
}