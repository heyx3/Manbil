#pragma once

#include "OpenGLIncludes.h"
#include "RenderInfo.h"
#include "Material.h"
#include "Math/Vectors.h"
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
	Foliage(std::vector<Vector3f> foliageBasePoses, float foliageScale, Material * foliageMat = 0);
	~Foliage(void);

	Foliage(const Foliage & cpy); //This function intentionally left unimplemented.


	bool HasError(void) const { return !errorMsg.empty(); }
	std::string GetError(void) const { return errorMsg; }
	void ClearError(void) const { errorMsg.clear(); }


    void SetWaveSpeed(float value) { Mat->SetUniformF("waveSpeed", &value, 1); }
    void SetWaveScale(float value) { Mat->SetUniformF("waveScale", &value, 1); }
    void SetLeanAwayMaxDist(float value) { Mat->SetUniformF("leanMaxDist", &value, 1); }
    void SetTexture(RenderObjHandle tex) { Mat->SetTexture(tex, 0); }


	bool Render(const RenderInfo & info);


private:

    bool madeMaterial;

	mutable std::string errorMsg;

	RenderObjHandle vbo, ibo;
	int nVertices, nIndices;
};