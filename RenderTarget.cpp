#include "RenderTarget.h"

#include "Vertices.h"
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "RenderingState.h"


RenderTarget::RenderTarget(int w, int h)
	: width(w), height(h), TextureSlot(0)
{
	ClearAllRenderingErrors();


	#pragma region Create frame buffer

	//Create frame buffer object.
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	//Create the color texture.
	glGenTextures(1, &colorTex);
	glBindTexture(GL_TEXTURE_2D, colorTex);

	//Set some parameters for the texture.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Attach the texture to the frame buffer.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);


	//Set up the depth renderbuffer.
	glGenRenderbuffers(1, &depthBuff);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderDataHandler::FrameBufferStatus stat = RenderDataHandler::GetFramebufferStatus(frameBuffer);
	if (stat != RenderDataHandler::FrameBufferStatus::EVERYTHING_IS_FINE)
	{
		errorMsg = "Framebuffer is not ready!";
		return;
	}

	#pragma endregion


	std::string errIntro = std::string("Error setting up frame buffer object: ");
	errorMsg = GetCurrentRenderingError();
	ClearAllRenderingErrors();
	if (errorMsg.compare("") != 0)
	{
		errorMsg = errIntro + errorMsg;
		return;
	}


	#pragma region Create shaders

	errIntro = "Error creating render target shaders: ";

	const char * pVS = "#version 330						        \n\
															        \n\
						layout (location = 0) in vec3 position;     \n\
						layout (location = 1) in vec2 texCoord;     \n\
															        \n\
						out vec2 texCoordOut;				        \n\
															        \n\
						void main()							        \n\
						{									        \n\
							texCoordOut = texCoord;			        \n\
							gl_Position = vec4(position, 1.0);      \n\
						}";
	const char * pFS = "#version 330						        \n\
															        \n\
						in vec2 texCoordOut;				        \n\
						out vec4 outColor;					        \n\
															        \n\
						uniform sampler2D frameBufferTex;	        \n\
                        vec3 smpl(vec2 uv)                          \n\
                        {                                           \n\
                            return texture(frameBufferTex, uv).xyz; \n\
                        }                                           \n\
															        \n\
                        vec3 blur(vec2 uv)                          \n\
                        {                                           \n\
                            vec3 col = smpl(uv);                    \n\
                            const int sharpness = 4;                \n\
                            col *= float(sharpness);                \n\
                                                                    \n\
                            const float step = 0.002;               \n\
                            for (int i = -1; i < 2; ++i)            \n\
                                for (int j = -1; j < 2; ++j)        \n\
                                    col += smpl(uv + vec2(step * i, \n\
                                                          step * j));\n\
                                                                    \n\
                            return col / float(9 + sharpness);      \n\
                        }                                           \n\
                                                                    \n\
						void main()							        \n\
						{									        \n\
							outColor = vec4(blur(texCoordOut), 1.0);\n\
						}";

	if (!ShaderHandler::CreateShaderProgram(shaderProg))
	{
		errorMsg = errIntro + ShaderHandler::GetErrorMessage();
		return;
	}

	if (!ShaderHandler::CreateShader(shaderProg, vShader, pVS, GL_VERTEX_SHADER))
	{
		errorMsg = errIntro + ShaderHandler::GetErrorMessage();
		return;
	}
	if (!ShaderHandler::CreateShader(shaderProg, fShader, pFS, GL_FRAGMENT_SHADER))
	{
		errorMsg = errIntro + ShaderHandler::GetErrorMessage();
		return;
	}

	if (!ShaderHandler::FinalizeShaders(shaderProg))
	{
		errorMsg = errIntro + ShaderHandler::GetErrorMessage();
		return;
	}

	if (!RenderDataHandler::GetUniformLocation(shaderProg, "frameBufferTex", colorTexLoc))
	{
		errorMsg = std::string("Error getting frame buffer texture uniform: ", RenderDataHandler::GetErrorMessage());
		return;
	}

	#pragma endregion


	ClearAllRenderingErrors();


	#pragma region Create VBO
	
	VertexPosTex1 vs[4];
	vs[0] = VertexPosTex1(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(0.0f, 0.0f));
	vs[1] = VertexPosTex1(Vector3f(-1.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
	vs[2] = VertexPosTex1(Vector3f(1.0f, -1.0f, 0.0f), Vector2f(1.0f, 0.0f));
	vs[3] = VertexPosTex1(Vector3f(1.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f));

	unsigned int indices[6];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 3;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	RenderDataHandler::CreateVertexBuffer(vbo, vs, 4, RenderDataHandler::UPDATE_ONCE_AND_DRAW);
	RenderDataHandler::CreateIndexBuffer(ibo, indices, 6, RenderDataHandler::UPDATE_ONCE_AND_DRAW);

	#pragma endregion


	errIntro = std::string("Error setting up vertex buffer object: ");
	errorMsg = GetCurrentRenderingError();
	ClearAllRenderingErrors();
	if (errorMsg.compare("") != 0)
	{
		errorMsg = errIntro + errorMsg;
		return;
	}

	glViewport(0, 0, width, height);
}

void RenderTarget::ChangeSize(int newW, int newH)
{
	width = newW;
	height = newH;
	

    //Recreate the color texture.
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newW, newH, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	
	//Set properties.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
    //Recreate the depth renderbuffer.
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);

	//Attach the objects to the frame buffer.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    std::string str = GetCurrentRenderingError();
    if (!str.empty())
    {
        errorMsg = "Error resizing render target: ";
        errorMsg += str;
    }
}

bool RenderTarget::IsValid(void) const
{
	GLint currentBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBuffer);


	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	bool goodFBO = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, currentBuffer);


	if (errorMsg.empty())
	{
		errorMsg = std::string(GetCurrentRenderingError());
	}

	return goodFBO && errorMsg.compare("") == 0;
}


RenderTarget::~RenderTarget(void)
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteTextures(1, &colorTex);
	glDeleteRenderbuffers(1, &depthBuff);
}



void RenderTarget::EnableDrawingInto(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
}
void RenderTarget::DisableDrawingInto(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}


void RenderTarget::Draw(void)
{
	ClearAllRenderingErrors();

	RenderingState(false).EnableState();


	glUseProgram(shaderProg);
	glUniform1i(colorTexLoc, TextureSlot);
	glActiveTexture(GL_TEXTURE0 + TextureSlot);
	glBindTexture(GL_TEXTURE_2D, colorTex);

	errorMsg = GetCurrentRenderingError();
	if (!errorMsg.empty()) return;

	RenderDataHandler::BindVertexBuffer(vbo);
	VertexPosTex1::EnableAttributes();
	RenderDataHandler::BindIndexBuffer(ibo);
	ShaderHandler::DrawIndexedVertices(PrimitiveTypes::Triangles, 6);
	VertexPosTex1::DisableAttributes();


	errorMsg = GetCurrentRenderingError();
}