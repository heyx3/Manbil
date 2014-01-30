#include "TerrainWorld.h"
/*
#include "OpenGLIncludes.h"

#include "SFML/Window.hpp"

#include <iostream>

#include "TransformObject.h"
#include "ShaderHandler.h"
#include "Vectors.h"
#include "Terrain.h"
#include "LayeredOctave.h"
#include "DiamondSquare.h"
#include "Worley.h"
#include "NoiseFilterer.h"
#include "RenderSettings.h"
#include "GeometricMath.h"
#include "Interpolator.h"
#include "NoiseCombinations.h"
#include "NoiseToTexture.h"

using namespace std;

typedef ShaderHandler SH;
typedef RenderSettings RS;
typedef Fake2DArray<float> NoiseMap;

//Size data.
const int tSizeX = 1025,
		  tSizeY = tSizeX;
const float tTexSpacing = 0.15f;

namespace RandStuff2 { FastRand fr; }
using namespace RandStuff2;

void GenerateNoise(Noise2D & noise)
{
	if (false)
	{
		fr.Seed = fr.GetRandInt();
		Worley wor(fr.GetRandInt(), 300, Interval(20, 2));
		wor.DistFunc = &Worley::QuadraticDistance;
		wor.ValueGenerator = [](Worley::DistanceValues v) { return -v.Values[0] + v.Values[5]; };
		Noise2D finalNoise(tSizeX, tSizeY);
		wor.Generate(finalNoise);

		NoiseFilterer nf;
		MaxFilterRegion mfr;
		nf.FillRegion = &mfr;
		nf.FilterFunc = &NoiseFilterer::Flatten;
		nf.Flatten_FlatValue = 0.0f;
		mfr.StrengthLerp = 0.25f;
		nf.Generate(finalNoise);

		nf.FilterFunc = &NoiseFilterer::Increase;
		CircularFilterRegion cfr(Vector2f(tSizeX * 0.5f, tSizeY * 0.5f), 25, 1.0f, 1.0f);
		nf.FillRegion = &cfr;
		nf.Increase_Amount = 1.0f;
		nf.Generate(finalNoise);

		nf.FilterFunc = &NoiseFilterer::Noise;
		cfr.DropoffRadiusPercent = 0.0f;
		nf.Noise_Seed = fr.GetRandInt();
		nf.Noise_Amount = 0.25f;
		nf.Generate(finalNoise);

		nf.FillRegion = &mfr;
		nf.FilterFunc = &NoiseFilterer::Flatten;
		mfr.StrengthLerp = 0.25f;
		nf.Generate(finalNoise);

		nf.FillRegion = &mfr;
		mfr.StrengthLerp = 1.0f;
		nf.FilterFunc = &NoiseFilterer::Smooth;
		nf.Generate(finalNoise);

		noise.Fill(finalNoise, Vector2i());
	}
	else
	{
		fr.GetRandInt();
		int seeds[5] = { fr.GetRandInt(), fr.GetRandInt(), fr.GetRandInt(), fr.GetRandInt(), fr.GetRandInt() };

		Noise2D noiseA(tSizeX, tSizeY), noiseA2(tSizeX, tSizeY), noiseA3(tSizeX, tSizeY);

		//Fill with NaN.
		FlatNoise fn(BasicMath::NaN);
		fn.Generate(noiseA);
		noiseA[Vector2i(tSizeX / 4, tSizeY / 4)] = 15.0f;

		//Use Diamond-Square, multiplied with Value noise.

		const int dsSteps = 10;
		DiamondSquareStep fv[dsSteps] = { DiamondSquareStep(Interval(0.0f, 10.0f), 1),
										  DiamondSquareStep(Interval(0.0f, 9.0f), 1),
										  DiamondSquareStep(Interval(0.0f, 8.0f), 1),
										  DiamondSquareStep(Interval(0.0f, 7.0f), 1),
										  DiamondSquareStep(Interval(0.0f, 6.0f), 1),
										  DiamondSquareStep(Interval(0.0f, 4.0f), 1),
										  DiamondSquareStep(Interval(0.0f, 2.5f), 1),
										  DiamondSquareStep(Interval(0.0f, 1.0f), 1),
										  DiamondSquareStep(Interval(0.0f, 0.5f), 1),
										  DiamondSquareStep(Interval(0.0f, 0.1f), 1) };
		Interval valueRange(0.0f, 0.0f);
		for (int i = 0; i < dsSteps; ++i)
		{
			valueRange = Interval(0.0f, valueRange.GetRange() + fv[i].VarianceValueRange.GetRange());
		}
		DiamondSquare ds(seeds[0], Interval(0.0f, 0.000001f), fv, dsSteps, -15.0f);
		ds.Generate(noiseA);

		NoiseFilterer nf;
		RectangularFilterRegion rfr(Vector2i(), Vector2i(noiseA.GetWidth(), noiseA.GetHeight()));
		nf.FillRegion = &rfr;
		nf.RemapValues_OldVals = valueRange;
		nf.FilterFunc = &NoiseFilterer::RemapValues;
		nf.Generate(noiseA);

		Combine2Noises c2n(Combine2Noises::Multiply2, &noiseA, &noiseA);
		c2n.Generate(noiseA2);

		noise.Fill(noiseA2);
	}
}

const float CamMoveSpd = 1.0f,
			CamRotSpd = 0.08f,
			CamHeight = 1.75f;
const float terrSpacing = 10.0f,
			terrHeight = 500.0f;
const float SkySize = (tSizeX * terrSpacing) + 8500.0f;

template<typename Func, class IterateThrough>
//Must have signature "bool outFunc(IterateThrough toPrint)".
//Goes through every element in the given array and calls the given function on it.
void DoToEach(Func outFunc, int stopEvery, IterateThrough * objArray, int objCount)
{
	int count = 0;
	for (int i = 0; i < objCount; ++i)
	{
		if (i != 0 && count > 0 && count % stopEvery == 0)
		{
			char dummy;
			std::cin >> dummy;
		}

		if (outFunc(objArray[i])) count += 1;
	}
}

const Vector2i windowSize(1500, 1000);
TerrainWorld::TerrainWorld(void) : SFMLOpenGLWorld(windowSize.x, windowSize.y)
{
	skyBrightness = 1.0f;

	brickTex = NULL;
	grassTex = NULL;
	skyTex = NULL;
	heightTex = NULL;
	distortTex = NULL;

	verticesTerr = NULL;
	verticesSky = NULL;
	verticesDebug = NULL;
	verticesPPE = NULL;
	indicesTerr = NULL;
	indicesDebug = NULL;

	matTerr = NULL;
	matSky = NULL;
	ppe = NULL;
	//matDebug = NULL;

	skyMesh = NULL;
	terrainMesh = NULL;
	debugMesh = NULL;
	ppeMesh = NULL;

	terr = NULL;

	dirLight = NULL;
}
void TerrainWorld::DeleteData(void)
{
	GeneralOculus::DestroyRiftSystem();
	
	RenderDataHandler::DeleteBuffer(vboTerr);
	RenderDataHandler::DeleteBuffer(vboSky);
	RenderDataHandler::DeleteBuffer(vboDebug);
	RenderDataHandler::DeleteBuffer(vboPPE);
	RenderDataHandler::DeleteBuffer(iboTerr);
	RenderDataHandler::DeleteBuffer(iboDebug);

	DeleteAndSetToNull(brickTex);
	DeleteAndSetToNull(grassTex);
	DeleteAndSetToNull(skyTex);
	DeleteAndSetToNull(heightTex);
	DeleteAndSetToNull(distortTex);

	DeleteAndSetToNull(verticesTerr);
	DeleteAndSetToNull(verticesSky);
	DeleteAndSetToNull(verticesDebug);
	DeleteAndSetToNull(verticesPPE);

	DeleteAndSetToNull(indicesTerr);
	DeleteAndSetToNull(indicesDebug);

	DeleteAndSetToNull(matTerr);
	DeleteAndSetToNull(matSky);
	DeleteAndSetToNull(ppe);
	//DeleteAndSetToNull(matDebug);

	DeleteAndSetToNull(skyMesh);
	DeleteAndSetToNull(terrainMesh);
	DeleteAndSetToNull(debugMesh);
	DeleteAndSetToNull(ppeMesh);

	DeleteAndSetToNull(terr);

	DeleteAndSetToNull(dirLight);
}

void TerrainWorld::OnInitializeError(std::string errorMsg)
{
	EndWorld();
	glError = true;
	
	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	char dummy;
	cin >> dummy;
}

bool GenerateTextures(sf::Image & grass, sf::Image & sky, sf::Image & distortion)
{
	//Generate grass.
	{
		const int layers = 8;
		const int grassSize = 256;
		Value2D basicRands[layers] = { Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()),
									   Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), };
		Generator** basicRandZooms = new Generator*[layers];
		basicRandZooms[0] = new Interpolator(&basicRands[0], grassSize, grassSize, 15);
		basicRandZooms[1] = new Interpolator(&basicRands[1], grassSize, grassSize, 12);
		basicRandZooms[2] = new Interpolator(&basicRands[2], grassSize, grassSize, 8);
		basicRandZooms[3] = new Interpolator(&basicRands[3], grassSize, grassSize, 6);
		basicRandZooms[4] = new Interpolator(&basicRands[3], grassSize, grassSize, 4);
		basicRandZooms[5] = new Interpolator(&basicRands[3], grassSize, grassSize, 2.75f);
		basicRandZooms[6] = new Interpolator(&basicRands[3], grassSize, grassSize, 2);
		basicRandZooms[7] = new Interpolator(&basicRands[3], grassSize, grassSize, 1);
		Noise2D finalNoise = Noise2D(grassSize, grassSize);
	
		float octaveWeights[layers] = { 0.5f, 0.25f, 0.15f, 0.125f, 0.0625f, 0.03125f, 0.01725f, 0.008f };
		LayeredOctave lo(layers, octaveWeights, basicRandZooms);
		lo.Generate(finalNoise);

		ColorGradient colGrad;
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.0f, Vector4f(0, 0, 0, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.5f, Vector4f(0.05f, 0.25f, 0.05f, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(0, 0.5f, 0, 1)));
	
		NoiseToTexture ntt(&colGrad, &finalNoise);
		if (!ntt.GetImage(grass))
			return false;
	}



	//Generate sky.
	{
		const int layers = 8;
		const int skySize = 256;
		Value2D basicRands[layers] = { Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()),
									   Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), Value2D(fr.GetRandInt()), };
		Generator** basicRandZooms = new Generator*[layers];
		basicRandZooms[0] = new Interpolator(&basicRands[0], skySize, skySize, 15);
		basicRandZooms[1] = new Interpolator(&basicRands[1], skySize, skySize, 12);
		basicRandZooms[2] = new Interpolator(&basicRands[2], skySize, skySize, 8);
		basicRandZooms[3] = new Interpolator(&basicRands[3], skySize, skySize, 6);
		basicRandZooms[4] = new Interpolator(&basicRands[3], skySize, skySize, 4);
		basicRandZooms[5] = new Interpolator(&basicRands[3], skySize, skySize, 2.75f);
		basicRandZooms[6] = new Interpolator(&basicRands[3], skySize, skySize, 2);
		basicRandZooms[7] = new Interpolator(&basicRands[3], skySize, skySize, 1);
		Noise2D finalNoise = Noise2D(skySize, skySize);
		float octaveWeights[layers] = { 0.5f, 0.25f, 0.15f, 0.125f, 0.0625f, 0.03125f, 0.01725f, 0.008f };
		LayeredOctave lo(layers, octaveWeights, basicRandZooms);
		lo.Generate(finalNoise);

		ColorGradient colGrad;
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.0f, Vector4f(1, 1, 1, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.5f, Vector4f(0.0f, 0.25f, 0.7f, 1)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(0.25f, 0.65f, 1.0f, 1)));
	
		NoiseToTexture ntt(&colGrad, &finalNoise);
		if (!ntt.GetImage(sky))
			return false;
	}



	//Generate distortion.
	{
		const int distSize = 128;

		//Value2D basicRand(fr.GetRandInt());
		//Interpolator basicRandZooms(&basicRand, distSize, distSize, 60.0f);
		FlatNoise flatRand;
		NoiseFilterer nf;
		CircularFilterRegion cfr = CircularFilterRegion(Vector2f(distSize * 0.5f, distSize * 0.5f), 50, 1.0f, 1.0f);
		nf.FillRegion = &cfr;
		nf.FilterFunc = &NoiseFilterer::Increase;
		nf.Increase_Amount = 1.0f;

		Noise2D finalNoise(distSize, distSize);

		//basicRandZooms.Generate(finalNoise);
		flatRand.Generate(finalNoise);
		nf.Generate(finalNoise);

		ColorGradient colGrad;
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(0.0f, Vector4f(0, 0, 0, 0)));
		colGrad.OrderedNodes.insert(colGrad.OrderedNodes.end(), ColorNode(1.0f, Vector4f(1, 1, 1, 1)));

		NoiseToTexture ntt(&colGrad, &finalNoise);
		if (!ntt.GetImage(distortion))
		{
			return false;
		}
	}

	return true;
}
void TerrainWorld::InitializeWorld(void)
{
	#pragma region Basic GL settings
	
	glError = false;
	SFMLOpenGLWorld::InitializeWorld();
	if (glError)
	{
		return;
	}

	RS::SetDepthTest(true);
	RS::SetUseDepthMask(true);
	RS::SetUseBlending(true);

	GetWindow()->setMouseCursorVisible(false);

	#pragma endregion

	#pragma region Load textures

	brickTex = new sf::Image();
	if (!brickTex->loadFromFile("Brick.png"))
	{
		OnInitializeError(std::string("Couldn't load 'Brick.png'!"));
		return;
	}

	grassTex = new sf::Image();
	skyTex = new sf::Image();
	distortTex = new sf::Image();
	if (!GenerateTextures(*grassTex, *skyTex, *distortTex))
	{
		OnInitializeError(std::string("Couldn't generate grass, distortion, and sky textures."));
		return;
	}
	/*
	if (!grassTex->loadFromFile("Grass.png"))
	{
		OnInitializeError(std::string("Couldn't load 'Grass.png'!"));
		return;
	}

	if (!skyTex->loadFromFile("Sky.png"))
	{
		OnInitializeError(std::string("Couldn't load 'Sky.png'!"));
		return;
	}
	*

	heightTex = new sf::Image();
	if (!heightTex->loadFromFile("heightmap.png"))
	{
		OnInitializeError(std::string("Couldn't load 'heightmap.png'!"));
		return;
	}

	//Create the textures.
	BufferObjHandle texObjTerr, texObjSky, texObjDistort;
	RenderDataHandler::CreateTexture2D(texObjTerr, *grassTex);
	RenderDataHandler::CreateTexture2D(texObjSky, *skyTex);
	RenderDataHandler::CreateTexture2D(texObjDistort, *distortTex);

	#pragma endregion

	#pragma region Create materials

	matTerr = new PhongLitTexture();
	cout << "Phong lighting error: " << matTerr->GetErrorMessage() << "\n";
	matSky = new UnlitTexture();
	cout << "Unlit error: " << matSky->GetErrorMessage() << "\n";
	ppe = new PPETest(windowSize);
	cout << "PPE error: " << ppe->GetErrorMessage() << "\n";
	//matDebug = new BareColor();
	//cout << "Color error: " << matDebug->GetShaderErrorMessage() << "\n";

	matTerr->Enable();
	matTerr->SetTexture(texObjTerr);
	matTerr->SetSpecularPower(32);
	matTerr->SetSpecularIntensity(0.0f);
	dirLight = new DirectionalLight(0.95f, 0.05f, Vector3f(1, 1, 1), Vector3f(1, 0, -1));
	matTerr->SetDirectionalLight(*dirLight);

	matSky->Enable();
	matSky->SetBrightness(1.0f);
	skyBrightness = 1.0f;
	matSky->SetTexture(texObjSky);

	ppe->Enable();

	//Set some texture-sampling settings.

	BufferObjHandle hand = matTerr->GetTextureHandle();
	TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MAG, TextureFilter::LINEAR);
	TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MIN, TextureFilter::LINEAR);
	TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::S, TextureWrapAction::REPEAT);
	TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::T, TextureWrapAction::REPEAT);

	hand = matSky->GetTextureHandle();
	TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MAG, TextureFilter::LINEAR);
	TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MIN, TextureFilter::LINEAR);
	TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::S, TextureWrapAction::REPEAT);
	TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::T, TextureWrapAction::REPEAT);
	
	if (ppe->GetColorSamplerValue() != -1)
	{
		hand = ppe->GetViewportColorTex();
		TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MAG, TextureFilter::NEAREST);
		TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MIN, TextureFilter::NEAREST);
		TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::S, TextureWrapAction::CLAMP);
		TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::T, TextureWrapAction::CLAMP);
	}
	if (ppe->GetDepthSamplerValue() != -1)
	{
		hand = ppe->GetViewportDepthTex();
		TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MAG, TextureFilter::NEAREST);
		TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MIN, TextureFilter::NEAREST);
		TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::S, TextureWrapAction::CLAMP);
		TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::T, TextureWrapAction::CLAMP);
	}

	hand = texObjDistort;
	TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MAG, TextureFilter::NEAREST);
	TextureManipulator::SetTextureFilter(hand, TextureFilterSituation::MIN, TextureFilter::NEAREST);
	TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::S, TextureWrapAction::CLAMP);
	TextureManipulator::SetTextureWrap(hand, TextureWrapDimension::T, TextureWrapAction::CLAMP);

	#pragma endregion

	Fake2DArray<float> noise(1, 1);
	terr = 0;

	noise.Reset(tSizeX, tSizeY);
	GenerateNoise(noise);
	terr = new Terrain(tSizeX, tSizeY);

	terr->SetHeightmap(noise);
	cam.SetHeightmap(terr);
	
	#pragma region Create terrain primitives
	
	//Calculate number of vertices/indices.
	nVerticesTerr = terr->GetVerticesCount();
	nIndicesTerr = terr->GetIndicesCount();

	//Export as vertices.

	Vector3f * vPoses = new Vector3f[nVerticesTerr],
			 * vNormals = new Vector3f[nVerticesTerr];
	Vector2f * vTexCoords = new Vector2f[nVerticesTerr];
	verticesTerr = new VertexPosTex1Normal[nVerticesTerr];
	indicesTerr = new int[nIndicesTerr];

	terr->CreateVertexPositions(vPoses, Vector2i(), Vector2i(terr->GetWidth() - 1, terr->GetHeight() - 1));
	for (int i = 0; i < nVerticesTerr; ++i)
	{
		vPoses[i] = Vector3f(vPoses[i].x * terrSpacing, vPoses[i].y * terrSpacing, vPoses[i].z * terrHeight);
	}
	terr->CreateVertexIndices(indicesTerr, Vector2i(), Vector2i(terr->GetWidth() - 1, terr->GetHeight() - 1));
	terr->CreateVertexTexCoords(vTexCoords, Vector2f(tTexSpacing, tTexSpacing), Vector2i(), Vector2i(terr->GetWidth() - 1, terr->GetHeight() - 1));
	//GeometricMath::CalculateNormals(vPoses, indicesTerr, nVerticesTerr, nIndicesTerr, vNormals);
	terr->CreateVertexNormals(vNormals, vPoses, Vector3f(1, 1, 1), Vector2i(), Vector2i(terr->GetWidth() - 1, terr->GetHeight() - 1));
	for (int i = 0; i < nVerticesTerr; ++i)
	{
		verticesTerr[i] = VertexPosTex1Normal(Vector3f(vPoses[i].x, vPoses[i].y, vPoses[i].z), vTexCoords[i], vNormals[i]);
	}
	matTerr->Vertices = verticesTerr;
	matTerr->LightDir = dirLight->Direction;

	delete vPoses;
	delete vNormals;
	delete vTexCoords;

	RenderDataHandler::CreateVertexBuffer(vboTerr, verticesTerr, nVerticesTerr, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	RenderDataHandler::CreateIndexBuffer(iboTerr, indicesTerr, nIndicesTerr, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

	terrainMesh = new Mesh(PrimitiveTypes::Triangles);
	terrainMesh->SetMaterial(matTerr);
	VertexIndexData vid(nVerticesTerr, vboTerr, nIndicesTerr, iboTerr);
	terrainMesh->SetVertexIndexData(&vid, 1);

	#pragma endregion

	if (false)
	{
		#pragma region Create debug primitives

		const float debugArrowSize = CamHeight + -3.0f;

		nVerticesDebug = 3 * nVerticesTerr;
		nIndicesDebug = nVerticesDebug;

		//verticesDebug = new VertexPosColor[nVerticesDebug];
		verticesDebug = new VertexPosTex1[nVerticesDebug];
		indicesDebug = new int[nIndicesDebug];

		int terrVertex;
		const float space = 0.5f;
		for (int i = 0; i < nVerticesDebug; i += 3)
		{
			terrVertex = i / 3;

			//verticesDebug[i] =	 VertexPosColor(verticesTerr[terrVertex].Pos + Vector3f(0.01f, 0, 0), Vector3b(75, 75, 75));
			//verticesDebug[i + 1] = VertexPosColor(verticesTerr[terrVertex].Pos + Vector3f(-0.01f, 0, 0), Vector3b(75, 75, 75));
			//verticesDebug[i + 2] = VertexPosColor(verticesTerr[terrVertex].Pos + (verticesTerr[terrVertex].Normal * debugArrowSize),
			//									  Vector3b(75, 75, 75));
		
			verticesDebug[i] =       VertexPosTex1(verticesTerr[terrVertex].Pos + Vector3f(space, space, 0), Vector2f(0.0f, 0.0f));
			verticesDebug[i + 1] =   VertexPosTex1(verticesTerr[terrVertex].Pos + Vector3f(-space, -space, 0), Vector2f(1.0f, 0.0f));
			verticesDebug[i + 2] =   VertexPosTex1(verticesTerr[terrVertex].Pos + (verticesTerr[terrVertex].Normal * debugArrowSize),
												   Vector2f(0.5f, 1.0f));
		
			indicesDebug[i] = i;
			indicesDebug[i + 1] = i + 1;
			indicesDebug[i + 2] = i + 2;
		}
		//DoToEach([](VertexPosColor & vpc) { cout << "Pos: " << vpc.Pos.x << ", " << vpc.Pos.y << ", " << vpc.Pos.z << "\n"; return true; }, 3, verticesDebug, nVerticesDebug);

		RenderDataHandler::CreateVertexBuffer(vboDebug, verticesDebug, nVerticesDebug, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
		RenderDataHandler::CreateIndexBuffer(iboDebug, indicesDebug, nIndicesDebug, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

		debugMesh = new Mesh(PrimitiveTypes::Triangles);
		//debugMesh->SetMaterial(matDebug);
		debugMesh->SetMaterial(matSky);
		VertexIndexData vid2(nVerticesDebug, vboDebug, nIndicesDebug, iboDebug);
		debugMesh->SetVertexIndexData(&vid2, 1);

		#pragma endregion
	}

	#pragma region Create sky primitives
	
	nVerticesSky = 6 * 6;
	verticesSky = new VertexPosTex1[nVerticesSky];


	//Use macros to save time typing.

	#define MVPT1(x, y, z, s, t) (VertexPosTex1(Vector3f(x, y, z), Vector2f(s, t)))
	#define VS(index) (verticesSky[index])
	
	
	VS(0) = MVPT1(-1, -1, -1, 0, 0);
	VS(1) = MVPT1(-1, 1, -1, 0, 1);
	VS(2) = MVPT1(1, 1, -1, 1, 1);
	
	VS(3) = MVPT1(-1, -1, -1, 0, 0);
	VS(4) = MVPT1(1, -1, -1, 1, 0);
	VS(5) = MVPT1(1, 1, -1, 1, 1);
	

	VS(6) = MVPT1(-1, -1, 1, 0, 0);
	VS(7) = MVPT1(-1, 1, 1, 0, 1);
	VS(8) = MVPT1(1, 1, 1, 1, 1);
	
	VS(9) = MVPT1(-1, -1, 1, 0, 0);
	VS(10) = MVPT1(1, -1, 1, 1, 0);
	VS(11) = MVPT1(1, 1, 1, 1, 1);

	

	VS(12) = MVPT1(-1, -1, -1, 0, 0);
	VS(13) = MVPT1(1, -1, -1, 0, 1);
	VS(14) = MVPT1(1, -1, 1, 1, 1);
	
	VS(15) = MVPT1(-1, -1, -1, 0, 0);
	VS(16) = MVPT1(-1, -1, 1, 1, 0);
	VS(17) = MVPT1(1, -1, 1, 1, 1);
	

	VS(18) = MVPT1(-1, 1, -1, 0, 0);
	VS(19) = MVPT1(1, 1, -1, 0, 1);
	VS(20) = MVPT1(1, 1, 1, 1, 1);
	
	VS(21) = MVPT1(-1, 1, -1, 0, 0);
	VS(22) = MVPT1(-1, 1, 1, 1, 0);
	VS(23) = MVPT1(1, 1, 1, 1, 1);



	VS(24) = MVPT1(-1, -1, -1, 0, 0);
	VS(25) = MVPT1(-1, 1, -1, 1, 0);
	VS(26) = MVPT1(-1, 1, 1, 1, 1);

	VS(27) = MVPT1(-1, -1, -1, 0, 0);
	VS(28) = MVPT1(-1, -1, 1, 0, 1);
	VS(29) = MVPT1(-1, 1, 1, 1, 1);
	

	VS(30) = MVPT1(1, -1, -1, 0, 0);
	VS(31) = MVPT1(1, 1, -1, 1, 0);
	VS(32) = MVPT1(1, 1, 1, 1, 1);

	VS(33) = MVPT1(1, -1, -1, 0, 0);
	VS(34) = MVPT1(1, -1, 1, 0, 1);
	VS(35) = MVPT1(1, 1, 1, 1, 1);



	RenderDataHandler::CreateVertexBuffer(vboSky, verticesSky, nVerticesSky, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
	
	skyMesh = new Mesh(PrimitiveTypes::Triangles);
	skyMesh->SetMaterial(matSky);
	VertexIndexData vid3(nVerticesSky, vboSky);
	skyMesh->SetVertexIndexData(&vid3, 1);

	#pragma endregion

	#pragma region Create PPE primitives

	nVerticesPPE = 6;
	verticesPPE = new VertexPosTex1[nVerticesPPE];
	verticesPPE[0] = VertexPosTex1(Vector3f(-1, -1, 0.9999f), Vector2f(0, 0));
	verticesPPE[1] = VertexPosTex1(Vector3f(-1, 1, 0.9999f), Vector2f(0, 1));
	verticesPPE[2] = VertexPosTex1(Vector3f(1, 1, 0.9999f), Vector2f(1, 1));
	verticesPPE[3] = VertexPosTex1(Vector3f(-1, -1, 0.9999f), Vector2f(0, 0));
	verticesPPE[4] = VertexPosTex1(Vector3f(1, -1, 0.9999f), Vector2f(1, 0));
	verticesPPE[5] = VertexPosTex1(Vector3f(1, 1, 0.9999f), Vector2f(1, 1));

	RenderDataHandler::CreateVertexBuffer(vboPPE, verticesPPE, nVerticesPPE, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

	ppeMesh = new Mesh(PrimitiveTypes::Triangles);
	ppeMesh->SetMaterial(ppe);
	VertexIndexData vid4(nVerticesPPE, vboPPE);
	ppeMesh->SetVertexIndexData(&vid4, 1);

	#pragma endregion

	delete[] verticesTerr, verticesSky, verticesDebug;
	delete[] indicesTerr, indicesDebug;
	verticesTerr = 0; verticesSky = 0; verticesDebug = 0;
	indicesTerr = 0; indicesDebug = 0;
	
	#pragma region Oculus Rift

	//GeneralOculus::InitializeRiftSystem();
	//orDevice = GeneralOculus::GetDevice(0);
	//if (orDevice.get() != 0) orDevice->StartAutoCalibration();

	#pragma endregion

	#pragma region Camera

	cam.SetWindow(GetWindow());

	cam.SetRotSpeed(CamRotSpd);
	cam.SetMoveSpeed(CamMoveSpd);

	cam.SetPosition(Vector3f(tSizeX * 0.5f, tSizeY * 0.5f, 0.0f));
	cam.SetRotation(Vector3f(1, 0, 0), Vector3f(0, 0, 1), false);

	cam.Info.Width = GetWindow()->getSize().x;
	cam.Info.Height = GetWindow()->getSize().y;
	cam.Info.FOV = ToRadian(45.0f);
	cam.Info.zFar = SkySize * 4.0f;
	cam.Info.zNear = 1;

	#pragma endregion
}

void TerrainWorld::UpdateWorld(float elapsedSeconds)
{
	if (orDevice.get() != 0) orDevice->UpdateDevice();

	//End the world if there was an error.

	if (glError)
	{
		EndWorld();
		return;
	}

	//Update the camera.

	
	if (cam.Update(elapsedSeconds, orDevice))
	{
		EndWorld();
	}

	cam.Info.Width = GetWindow()->getSize().x;
	cam.Info.Height = GetWindow()->getSize().y;


	//Update the lighting.

	const float lightChangeRate = 0.01f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
	{
		dirLight->AmbientIntensity += lightChangeRate;
		if (dirLight->AmbientIntensity > 1.0f)
		{
			dirLight->AmbientIntensity -= 1.0f;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
	{
		dirLight->AmbientIntensity -= lightChangeRate;
		if (dirLight->AmbientIntensity < 0.0f)
		{
			dirLight->AmbientIntensity += 1.0f;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
	{
		dirLight->DiffuseIntensity += lightChangeRate;
		if (dirLight->DiffuseIntensity > 1.0f)
		{
			dirLight->DiffuseIntensity -= 1.0f;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
	{
		dirLight->DiffuseIntensity -= lightChangeRate;
		if (dirLight->DiffuseIntensity < 0.0f)
		{
			dirLight->DiffuseIntensity += 1.0f;
		}
	}
	matTerr->Enable();
	matTerr->SetDirectionalLight(*dirLight);


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		skyBrightness += lightChangeRate;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		skyBrightness -= lightChangeRate;
	}
	matSky->Enable();
	matSky->SetBrightness(skyBrightness);
}

void TerrainWorld::RenderWorld(float elapsedSeconds)
{
	cam.SetPosition(cam.GetPosition().ComponentProduct(Vector3f(terrSpacing, terrSpacing, terrHeight)));


	//Get rendering data.

	Matrix4f viewM, projM, objM, wvp;
	cam.GetViewTransform(viewM);
	projM.SetAsPerspProj(cam.Info);

	TransformObject trns;
	trns.SetRotation(Vector3f());

	RenderInfo info(this, &cam, &trns, &wvp, &objM, &viewM, &projM);
	if (false){
	RenderDataHandler::BindFrameBuffer(ppe->GetFrameBuffer());

	//Clear the screen.
	RenderSettings::Clearable clearables[] = { RenderSettings::Clearable::COLOR, RenderSettings::Clearable::DEPTH };
	RenderSettings::ClearScreen(clearables, 2);


	//Draw the sky.

	trns.SetPosition(Vector3f(tSizeX * terrSpacing * 0.5f, tSizeY * terrSpacing * 0.5f, 0.0f));
	trns.SetScale(SkySize);
	trns.GetWorldTransform(objM);

	wvp.SetAsWVP(projM, viewM, objM);

	matSky->Enable();
	skyMesh->Render(info);


	//Draw the terrain.

	trns.SetPosition(Vector3f(0, 0, -CamHeight));
	trns.SetScale(1.0f);
	trns.GetWorldTransform(objM);

	wvp.SetAsWVP(projM, viewM, objM);

	matTerr->Enable();
	terrainMesh->Render(info);


	//Draw the debug stuff.

	//matDebug->Enable();
	//debugMesh->Render(info);


	RenderDataHandler::BindFrameBuffer();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ppe->Enable();
	ppeMesh->Render(info);


	GetWindow()->display();


	cam.SetPosition(cam.GetPosition().ComponentProduct(Vector3f(1.0f / terrSpacing, 1.0f / terrSpacing, 1.0f / terrHeight)));
}

void TerrainWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	cam.Info.Width = newWidth;
	cam.Info.Height = newHeight;
	ppe->SetNewViewportSize(Vector2i(newWidth, newHeight));
}
*/