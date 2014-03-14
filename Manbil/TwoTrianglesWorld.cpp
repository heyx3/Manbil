#include "TwoTrianglesWorld.h"

#include "TextureSettings.h"
#include "RenderingState.h"
#include "ScreenClearer.h"
#include "Rendering/Texture Management/RenderTargetManager.h"
#include <iostream>
#include <fstream>

typedef TwoTrianglesWorld TTW;


//Debug printing stuff.
namespace TTWPrints
{
	void Pause(void) { char dummy; std::cin >> dummy; }
	void PrintData(std::string datIntro, std::string dat, bool useLineBreak = true)
	{
		std::cout << datIntro << ": " << dat;
		if (useLineBreak) std::cout << "\n";
	}
	bool PrintRenderError(const char * errorIntro)
	{
		const char * error = GetCurrentRenderingError();
		if (strcmp(error, "") != 0)
		{
			PrintData(errorIntro, error);
			ClearAllRenderingErrors();
			Pause();
			return false;
		}

		return true;
	}
}
using namespace TTWPrints;



const Vector2i windowSize(500, 500);
float shaderLoadedTime = 0.0f;
bool isInFocus = true;


bool shouldCaptureMouse = false,
	 mousePressedLastFrame = false;


Material * mat;
DrawingQuad * quad;
RenderTargetManager targManager;



TTW::TwoTrianglesWorld(void) : SFMLOpenGLWorld(windowSize.x, windowSize.y)
{
    mat = 0;
    quad = 0;
}

void TTW::InitializeWorld(void)
{
	//GL/SFML/window initialization.

	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;

	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

	GetWindow()->setMouseCursorVisible(!shouldCaptureMouse);

	//Create the material to render the triangles.

	LoadTTFShader(mat);
	mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));

	//Noise sampler texture.
	mat->SetSamplerTextureUnit(0, TwoTrianglesMaterial::NoiseSampler);
	sf::Image noiseImg;
	if (!noiseImg.loadFromFile("NoiseTex.png"))
	{
		PrintData("Error loading 'NoiseTex.png'", "SFML could not load the file.");
	}
	else
	{
		RenderDataHandler::CreateTexture2D(noiseObjLoaded, noiseImg);
		PrintRenderError("Error occurred creating specified texture 2D");
			
		TextureSettings(TextureSettings::TextureFiltering::TF_NEAREST, TextureSettings::TextureWrapping::TW_WRAP).SetData(noiseObjLoaded);
		PrintRenderError("Error occurred setting texture info for noise image.");

		mat->SetTexture(TwoTrianglesMaterial::NoiseSampler, noiseObjLoaded);
	}

	//Loaded image sampler texture.
	if (mat->UsesSampler(TwoTrianglesMaterial::FileSampler))
	{
		SetUpLoadedTexture(mat);
	}

	std::cout << "\nShader accepted.\n\n\n";

	
	shaderCam = MovingCamera();
	shaderCam.SetPosition(Vector3f());
	shaderCam.SetRotation(Vector3f(1.0, 0.0, 0.0), Vector3f(0.0, 1.0, 0.0), true);
}

void TTW::OnWorldEnd(void)
{
	DeleteAndSetToNull(mat);
}

void TTW::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	Pause();
}


void TTW::OnOtherWindowEvent(sf::Event & event)
{
	if (event.type == sf::Event::LostFocus)
		isInFocus = false;
	if (event.type == sf::Event::GainedFocus)
		isInFocus = true;
}


bool pressedLastFrame_1 = false,
	 pressedLastFrame_2 = false,
	 pressedLastFrame_3 = false,
	 pressedLastFrame_4 = false;
void TTW::UpdateWorld(float elapsedSeconds)
{
	shaderCam.Window = (shouldCaptureMouse ? GetWindow() : 0);
	if (isInFocus) shaderCam.Update(elapsedSeconds);

	//React to player input.
	#define KeyPressed(key) (sf::Keyboard::isKeyPressed(sf::Keyboard::Key:: ## key))

	if (isInFocus)
	{
		if (KeyPressed(Num1))
		{
			if (!pressedLastFrame_1)
			{
				LoadTTFShader(mat);
				shaderLoadedTime = GetTotalElapsedSeconds();
				mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));
				mat->SetSamplerTextureUnit(0, TwoTrianglesMaterial::NoiseSampler);
				mat->SetTexture(TwoTrianglesMaterial::NoiseSampler, noiseObjLoaded);
				mat->SetTexture(TwoTrianglesMaterial::FileSampler, texObjLoaded);
			}

			pressedLastFrame_1 = true;
		}
		else pressedLastFrame_1 = false;

		if (KeyPressed(Num2))
		{
			if (!pressedLastFrame_2)
			{
				std::cout << "\n\n\n\n\n\n";
				shaderPath = "";
				LoadTTFShader(mat);
				shaderLoadedTime = GetTotalElapsedSeconds();
				mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));
				mat->SetResolution(Vector2f(GetWindow()->getSize().x, GetWindow()->getSize().y));
				mat->SetTexture(TwoTrianglesMaterial::NoiseSampler, noiseObjLoaded);
				mat->SetTexture(TwoTrianglesMaterial::FileSampler, texObjLoaded);
			}

			pressedLastFrame_2 = true;
		}
		else pressedLastFrame_2 = false;

		if (KeyPressed(Num3))
		{
			if (!pressedLastFrame_3)
			{
				SetUpLoadedTexture(mat);
			}

			pressedLastFrame_3 = true;
		}
		else pressedLastFrame_3 = false;

		if (KeyPressed(Num4))
		{
			if (!pressedLastFrame_4)
			{
				texPath = "";
				SetUpLoadedTexture(mat);
			}

			pressedLastFrame_4 = true;
		}
		else pressedLastFrame_4 = false;



		if (KeyPressed(Escape))
		{
			shouldCaptureMouse = false;
			GetWindow()->setMouseCursorVisible(!shouldCaptureMouse);
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			if (!mousePressedLastFrame)
			{
				shouldCaptureMouse = !shouldCaptureMouse;
				GetWindow()->setMouseCursorVisible(!shouldCaptureMouse); 
			}

			mousePressedLastFrame = true;
		}
		else mousePressedLastFrame = false;
	}


	

	//Update shader data.

	if (!mat->SetElapsedTime(GetTotalElapsedSeconds() - shaderLoadedTime))
	{
		std::cout << "Error setting elapsed time: " << mat->GetErrorMessage() << "\n";
		Pause();
	}
	if (!mat->SetCamera(shaderCam))
	{
		std::cout << "Error setting camera: " << mat->GetErrorMessage() << "\n";
		Pause();
	}
}

void TTW::RenderWorld(float elapsedSeconds)
{
	Matrix4f id;
	id.SetAsIdentity();

	ScreenClearer clearer;


	clearer.ClearScreen();
	if (!PrintRenderError("Error clearing back buffer of color and depth")) return;

	if (!mat->Render())
	{
		PrintData("Error drawing two triangles", mat->GetErrorMessage());
	}
	GetWindow()->display();
}

void TTW::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	if (!PrintRenderError("Error updating the OpenGL viewport size"))
	{
		EndWorld();
		return;
	}

	if (!mat->SetResolution(Vector2f(newWidth, newHeight)))
	{
		PrintData("Error setting resolution of TTM", mat->GetErrorMessage());
		EndWorld();
		return;
	}
}