#pragma once
/*
#include "sfmlopenglworld.h"

#include "Vectors.h"
#include "TerrainWalkCamera.h"
#include "PhongLitTexture.h"
#include "UnlitTexture.h"
#include "BareColor.h"
#include "Mesh.h"
#include "PPETest.h"

class TerrainWorld : public SFMLOpenGLWorld
{
public:

	TerrainWorld(void);
	~TerrainWorld(void) { DeleteData(); }

protected:

	virtual void InitializeWorld(void) override;

	virtual void UpdateWorld(float elapsedSeconds) override;
	virtual void RenderWorld(float elapsedSeconds) override;

	virtual void OnInitializeError(std::string errorMsg) override;

	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

	virtual void OnWorldEnd(void) override { DeleteData(); }

private:

	void DeleteData(void);

	int nVerticesTerr, nVerticesSky, nVerticesDebug, nVerticesPPE, nIndicesTerr, nIndicesDebug;

	sf::Image * brickTex, * grassTex, * skyTex, * heightTex, * distortTex;
	
	Mesh * terrainMesh, * skyMesh, * debugMesh, * ppeMesh;

	VertexPosTex1Normal * verticesTerr;
	VertexPosTex1 * verticesSky;
	VertexPosTex1 * verticesPPE;
	VertexPosTex1 * verticesDebug;
	int * indicesTerr, * indicesDebug;

	Terrain * terr;

	TerrainWalkCamera cam;

	bool glError;

	BufferObjHandle vboTerr, vboSky, vboDebug, vboPPE, iboTerr, iboDebug;

	PhongLitTexture * matTerr;
	UnlitTexture * matSky;
	PPETest * ppe;
	//BareColor * matDebug;

	DirectionalLight * dirLight;
	float skyBrightness;

	std::shared_ptr<OculusDevice> orDevice;
};
*/