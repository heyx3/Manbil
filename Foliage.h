#pragma once

#include "OpenGLIncludes.h"
#include "RenderInfo.h"
#include "Material.h"
#include "Vectors.h"
#include <vector>


//Represents a clump of foliage.
//This class handles all aspects of rendering itself;
//    just use the static methods to prepare foliage to be drawn.
class Foliage
{
public:
	
	static void StartFoliageRendering(bool enableVertexAttributes);
	static void EndFoliageRendering(bool disableVertexAttributes);

	static void SetFoliageTexture(BufferObjHandle texObj);
	static void SetFoliageTextureUnit(int unit = 0);

	static const Material& GetFoliageMaterial(void);


	Foliage(std::vector<Vector3f> foliageBasePoses, float foliageScale);
	Foliage(const Foliage & cpy); //Don't actually implement this function. That way, any attempts to copy this object result in a compile-time error.
	~Foliage(void);

	bool HasRenderError(void) const { return !errorMsg.empty(); }

	void ClearRenderError(void) { errorMsg.clear(); }
	std::string GetRenderError(void) const { return errorMsg; }

	bool Render(const RenderInfo & info);

private:

	static bool foliageInitialized;

	std::string errorMsg;

	BufferObjHandle vbo, ibo;
	int nVertices, nIndices;
};