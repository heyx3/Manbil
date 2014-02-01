#include "RenderTarget.h"

#include <assert.h>

#include "Vertices.h"
#include "ShaderHandler.h"
#include "RenderDataHandler.h"
#include "RenderingState.h"


RenderTarget::RenderTarget(int w, int h)
	: width(w), height(h), ColorTextureSlot(0), DepthTextureSlot(1)
{
	ClearAllRenderingErrors();


	#pragma region Create shaders

	std::string errIntro = "Error creating render target shaders: ";

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
						uniform sampler2D fbColorTex;	            \n\
                        uniform sampler2D fbDepthTex;               \n\
                        vec3 smpl(vec2 uv)                          \n\
                        {                                           \n\
                            return texture(fbColorTex, uv).xyz;     \n\
                        }                                           \n\
                        float smplD(vec2 uv)                        \n\
                        {                                           \n\
                            return texture(fbDepthTex, uv).x;       \n\
                        }                                           \n\
                        float scaleDepth(float d)                   \n\
                        {                                           \n\
                            return pow(d, 300.0);                   \n\
                        }                                           \n\
															        \n\
                        vec3 blur(vec2 uv, float step, int sharpness)\n\
                        {                                           \n\
                            vec3 col = smpl(uv);                    \n\
                            col *= float(sharpness);                \n\
                                                                    \n\
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
                            float depthScl = 1.0 - scaleDepth(smplD(texCoordOut));\n\
                            vec3 worldColor = blur(texCoordOut, 0.008 * (1.0 - depthScl), int(4.0 * depthScl));\n\
                            vec3 fogColor = worldColor;//vec3(1.0, 1.0, 1.0);             \n\
                            outColor = vec4(mix(fogColor, worldColor, depthScl), 1.0);\n\
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

    usesCol = RenderDataHandler::GetUniformLocation(shaderProg, "fbColorTex", colorTexLoc);
    usesDepth = RenderDataHandler::GetUniformLocation(shaderProg, "fbDepthTex", depthTexLoc);

	#pragma endregion


	ClearAllRenderingErrors();


    #pragma region Create frame buffer

    //Create frame buffer object.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    //Set up the color texture.
    if (usesCol)
    {
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
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    //Set up the depth texture.
    if (usesDepth)
    {
        glGenTextures(1, &depthTex);
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    }
    //Need SOME kind of depth testing so that the world looks OK.
    //Renderbuffers should be used in place of depth textures if the shader doesn't do anything with depth.
    else
    {
        glGenRenderbuffers(1, &depthTex);
        glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTex);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Check framebuffer status.
    RenderDataHandler::FrameBufferStatus stat = RenderDataHandler::GetFramebufferStatus(frameBuffer);
    if (stat != RenderDataHandler::FrameBufferStatus::EVERYTHING_IS_FINE)
    {
        errorMsg = "Framebuffer is not ready!";
        return;
    }


    //Check for any errors with setting up the frame buffer.
    errIntro = std::string("Error setting up frame buffer object: ");
    errorMsg = GetCurrentRenderingError();
    ClearAllRenderingErrors();
    if (errorMsg.compare("") != 0)
    {
        errorMsg = errIntro + errorMsg;
        return;
    }

    #pragma endregion


	#pragma region Create VBO/IBO
	
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

    //Check for any errors with setting up the vbo/ibo.
	errIntro = std::string("Error setting up vertex buffer object: ");
	errorMsg = GetCurrentRenderingError();
	ClearAllRenderingErrors();
	if (errorMsg.compare("") != 0)
	{
		errorMsg = errIntro + errorMsg;
		return;
	}

	#pragma endregion


	glViewport(0, 0, width, height);
}

void RenderTarget::ChangeSize(int newW, int newH)
{
	width = newW;
	height = newH;
	

    //Recreate the color texture and set properties.
    if (usesCol)
    {
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newW, newH, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
	
    //Recreate the depth texture and set properties.
    if (usesDepth)
    {
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    }
    else
    {
        glBindRenderbuffer(GL_RENDERBUFFER, depthTex);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    }


	//Attach the objects to the frame buffer.

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);

    if (usesCol)
    {
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    }
    if (usesDepth)
    {
        glBindTexture(GL_TEXTURE_2D, depthTex);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    }
    else
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTex);
    }


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
	if (usesCol) glDeleteTextures(1, &colorTex);

    //Either "depthTex" holds a depth texture, or it actually holds a depth renderbuffer.
    if (usesDepth) glDeleteTextures(1, &depthTex);
    else glDeleteRenderbuffers(1, &depthTex);
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

	RenderingState(true).EnableState();


	glUseProgram(shaderProg);

    //Set up color/depth textures.
    assert(!usesDepth || !usesCol || ColorTextureSlot != DepthTextureSlot);
    if (usesCol)
    {
	    glUniform1i(colorTexLoc, ColorTextureSlot);
	    glActiveTexture(GL_TEXTURE0 + ColorTextureSlot);
	    glBindTexture(GL_TEXTURE_2D, colorTex);
    }
    if (usesDepth)
    {
        glUniform1i(depthTexLoc, DepthTextureSlot);
        glActiveTexture(GL_TEXTURE0 + DepthTextureSlot);
        glBindTexture(GL_TEXTURE_2D, depthTex);
    }

	RenderDataHandler::BindVertexBuffer(vbo);
	VertexPosTex1::EnableAttributes();
	RenderDataHandler::BindIndexBuffer(ibo);
	ShaderHandler::DrawIndexedVertices(PrimitiveTypes::Triangles, 6);
	VertexPosTex1::DisableAttributes();


	errorMsg = GetCurrentRenderingError();
    if (!errorMsg.empty())
        errorMsg = std::string("Error rendering render target: ") + errorMsg;
}