#pragma once

/*

#include "OpenGLIncludes.h"
#include "RenderInfo.h"
#include "Material.h"
#include "Math/Vectors.h"
#include "Mesh.h"
#include <vector>


//Represents a group of foliage.
//A single piece of foliage is here defined as two flat quads,
//    perpendicular to each other, with their top edges waving over time.
class Foliage
{
public:

    Material * Mat;

    //Gets the single rendering pass needed to render the foliage.
    static RenderingPass GetFoliageRenderer(void);


    //If no foliage material is provided, one will be generated.
    //If this Foliage generates its own Material,
    //    the Material will be destroyed when this Foliage object is deleted.
	Foliage(std::vector<Vector3f> foliageBasePoses, Vector2f foliageScale, Material * foliageMat = 0);
	~Foliage(void);

    //This function intentionally left unimplemented.
	Foliage(const Foliage & cpy);


	bool HasError(void) const { return !errorMsg.empty(); }
	std::string GetError(void) const { return errorMsg; }
	void ClearError(void) const { errorMsg.clear(); }

    const Mesh & GetMesh(void) const { return folMesh; }
    Mesh & GetMesh(void) { return folMesh; }

    void SetWaveSpeed(float value) { folMesh.FloatUniformValues["waveSpeed"].Data[0] = value; }
    void SetWaveScale(float value) { folMesh.FloatUniformValues["waveScale"].Data[0] = value; }
    void SetLeanAwayMaxDist(float value) { folMesh.FloatUniformValues["leanMaxDist"].Data[0] = value; }
    void SetBrightness(float value) { folMesh.FloatUniformValues["brightness"].Data[0] = value; }
    void SetTexture(RenderObjHandle tex) { folMesh.TextureSamplers[0][0] = tex; }


	bool Render(const RenderInfo & info);


private:

    bool madeMaterial;
    Mesh folMesh;

	mutable std::string errorMsg;
};

*/