#pragma once

#include "OpenGLIncludes.h"
#include "RenderInfo.h"
#include "Material.h"
#include "Math/Vectors.h"
#include <vector>


//Represents a single clump of foliage.
class Foliage
{
public:

    Material * Mat;

    //Gets the single rendering pass needed to render the foliage.
    static RenderingPass GetFoliageRenderer(void);


    //If no foliage material is provided, one will be generated.
	Foliage(std::vector<Vector3f> foliageBasePoses, float foliageScale, Material * foliageMat = 0);
	~Foliage(void);

	Foliage(const Foliage & cpy); //This function intentionally left unimplemented.


	bool HasError(void) const { return !errorMsg.empty(); }
	void ClearError(void) const { errorMsg.clear(); }
	std::string GetError(void) const { return errorMsg; }


    void SetWaveSpeed(float value) { Mat->SetUniformF("waveSpeed", &value, 1); }
    void SetWaveScale(float value) { Mat->SetUniformF("waveScale", &value, 1); }
    void SetTexture(RenderObjHandle tex) { Mat->SetTexture(tex, 0); }


	bool Render(const RenderInfo & info);


private:

	mutable std::string errorMsg;

	RenderObjHandle vbo, ibo;
	int nVertices, nIndices;
};