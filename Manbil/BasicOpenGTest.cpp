#include "BasicOpenGTest.h"

#include "Matrix4f.h"

#include "OpenGLIncludes.h"
#include "RenderSettings.h"
#include "ShaderHandler.h"

#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>
#include <SFML\Window.hpp>

#include "BareColor.h"
#include "Vertices.h"
#include "MovingCamera.h"
#include "Mesh.h"
#include "PPETest.h"

#include "RenderTarget.h"

#include <iostream>



#pragma region Error-handling

//Prints (errorIntro + ": " + error) to the console stream.
void PrintError(const char * errorIntro, const char * error)
{
	fprintf(stderr, (std::string(errorIntro) + ": '%s'\n").c_str(), error);
}

//Waits for any console input.
void Pause(void) { char dummy; std::cin >> dummy; }

//Prints out the most recent OpenGL error and flushes out any other OpenGL errors sitting in the stack.
//Returns whether or not the GL context is error-free.
bool CheckGLErrors(const char * error)
{
	const char * err = GetCurrentRenderingError();
	if (strcmp(err, "") != 0)
	{
		PrintError(error, err);
		ClearAllRenderingErrors();
		Pause();
		return false;
	}

	return true;
}

#pragma endregion




Material * mat;
MovingCamera cam;
VertexIndexData vid1, vid2;
Mesh mesh(PrimitiveTypes::Triangles);



GLuint vbo1, vbo2;
void CreateVBO(void)
{
	VertexPosColor vertices[3];
	vertices[0] = VertexPosColor(Vector3f(-5.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f));
	vertices[1] = VertexPosColor(Vector3f(-2.5f, 2.5f, 0.0f), Vector3f(1.0f, 1.0f, 0.0f));
	vertices[2] = VertexPosColor(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
	RenderDataHandler::CreateVertexBuffer(vbo1, vertices, 3, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	CheckGLErrors("Error creating vbo for first triangle");

	vid1 = VertexIndexData(3, vbo1);

	
	VertexPosColor vertices2[3];
	vertices2[0] = VertexPosColor(Vector3f(-1.0f, 0.0f, 2.0f), Vector3f(1.0f, 0.0f, 0.0f));
	vertices2[1] = VertexPosColor(Vector3f(-0.5f, 0.5f, 2.0f), Vector3f(1.0f, 1.0f, 0.0f));
	vertices2[2] = VertexPosColor(Vector3f(0.0f, 0.0f, 2.0f), Vector3f(0.0f, 0.0f, 1.0f));
	RenderDataHandler::CreateVertexBuffer(vbo2, vertices2, 3, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	CheckGLErrors("Error creating vbo for second triangle");

	vid2 = VertexIndexData(3, vbo2);
}


RenderTarget * renderTarget = 0;

void RenderWorld(const RenderInfo & ri)
{
	ClearAllRenderingErrors();

	mat->Enable();
	CheckGLErrors("Error enabling world");
	if (!mesh.Render(ri))
	{
		PrintError("Error rendering world", mat->GetErrorMessage());
	}
}


BasicOpenGTest::BasicOpenGTest(void)
{
	sf::RenderWindow wind(sf::VideoMode(500, 500), sf::String("title"));
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		PrintError("Glew Init error", (char*)glewGetErrorString(res));
		Pause();
	}

	mat = 0;
	cam.SetMoveSpeed(0.5f);
	cam.SetRotSpeed(0.05f);
	cam.SetPosition(Vector3f(10.0f, 10.0f, 10.0f));
	cam.SetRotation(-cam.GetPosition(), Vector3f(0.0f, 0.0f, 1.0f), false);
	cam.Info.FOV = ToRadian(60.0f);
	cam.Info.Width = 500;
	cam.Info.Height = 500;
	cam.Info.zNear = 1.0f;
	cam.Info.zFar = 1000.0f;
	cam.Window = &wind;


	CreateVBO();


	mat = new BareColor();
	if (strcmp(mat->GetErrorMessage(), "") != 0)
	{
		PrintError("Bare color material error", mat->GetErrorMessage());
	}
	mat->Enable();
	mesh.SetMaterial(mat);
	VertexIndexData vids[] = { vid1, vid2 };
	mesh.SetVertexIndexData(vids, 2);
	
	renderTarget = new RenderTarget(500, 500);
	if (!renderTarget->IsValid())
	{
		PrintError("Error setting up render target", renderTarget->GetErrorMessage().c_str());
		Pause();
		return;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderSettings::SetDepthTest(true);
	if (!CheckGLErrors("Error setting use deth test")) return;
	RenderSettings::SetUseDepthMask(true);
	if (!CheckGLErrors("Error setting use depth mask")) return;
	RenderSettings::SetFacesToCull(RenderSettings::Cullables::C_NONE);
	if (!CheckGLErrors("Error setting culling")) return;

	//glViewport(0, 0, 500, 500);
	



	bool shiftPressed = false;
	bool useRenderTarget = false;

	sf::Event windEv;
	Matrix4f worldM, viewM, projM, wvpM;
	TransformObject transObj(Vector3f(0.0f, 0.0f, 0.0f));
	transObj.SetScale(20.0f);
	RenderInfo ri((SFMLOpenGLWorld*)0, (Camera*)&cam, &transObj, &wvpM, &worldM, &viewM, &projM);

	while (wind.isOpen())
	{
		while (wind.pollEvent(windEv))
		{
			if (windEv.type == sf::Event::Closed)
			{
				return;
			}
			else if (windEv.type == sf::Event::Resized)
			{
				glViewport(0, 0, windEv.size.width, windEv.size.height);
				renderTarget->ChangeSize(windEv.size.width, windEv.size.height);
				CheckGLErrors("Error resizing viewport");
				cam.Info.Width = windEv.size.width;
				cam.Info.Height = windEv.size.height;
			}
		}


		//Update camera.
		if (cam.Update(1.0f / 60.0f)) return;

		//Update input.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
		{
			if (!shiftPressed)
			{
				shiftPressed = true;
				useRenderTarget = !useRenderTarget;
			}
		}
		else shiftPressed = false;


		//Get transform data.

		Vector3f rots = transObj.GetRotationAngles();
		transObj.SetRotation(rots + Vector3f(0.0f, 0.0002f, 0.0f));

		transObj.GetWorldTransform(worldM);
		cam.GetViewTransform(viewM);
		projM.SetAsPerspProj(cam.Info);
		wvpM.SetAsWVP(projM, viewM, worldM);


		RenderSettings::Clearable toClear[] = { RenderSettings::Clearable::COLOR, RenderSettings::Clearable::DEPTH };


		ClearAllRenderingErrors();

		//Render.
		if (useRenderTarget && renderTarget->IsValid())
		{
			//Render to frame buffer.
		
			renderTarget->EnableDrawingInto();
			if (!renderTarget->IsValid())
			{
				PrintError("Error enabling render target", renderTarget->GetErrorMessage().c_str());
				Pause();
				return;
			}

			RenderSettings::ClearScreen(toClear, sizeof(toClear) / sizeof(RenderSettings::Clearable));
			
			//glDisable(GL_TEXTURE_2D);
			//glEnable(GL_BLEND);
			//glEnable(GL_DEPTH_TEST);
			RenderWorld(ri);



			//Render to screen.

			renderTarget->DisableDrawingInto();
			if (!renderTarget->IsValid())
			{
				PrintError("Error disabling render target", renderTarget->GetErrorMessage().c_str());
				Pause();
				return;
			}

			RenderSettings::ClearScreen(toClear, sizeof(toClear) / sizeof(RenderSettings::Clearable));
			CheckGLErrors("Error clearing screen");
		
			renderTarget->Draw();
			if (!renderTarget->IsValid())
			{
				PrintError("Error drawing render target", renderTarget->GetErrorMessage().c_str());
				Pause();
				return;
			}
		}
		else
		{
			RenderSettings::ClearScreen(toClear, sizeof(toClear) / sizeof(RenderSettings::Clearable));
			RenderWorld(ri);
		}

		wind.display();
	}
}


BasicOpenGTest::~BasicOpenGTest(void)
{
	if (mat != 0) delete mat;
	mat = 0;

	if (renderTarget != 0)
	{
		delete renderTarget;
	}
	renderTarget = 0;
}

*/