#pragma once

#include <string>
#include "Math/Vectors.h"
#include "Material.h"


//Contains different useful materials.
struct Materials
{
	static const MaterialShaders UnlitTexture, LitTexture, BareColor;

	//The exact struct that the "LitTexture" material uses.
	struct LitTexture_DirectionalLight
	{
		Vector3f Dir, Col;
		float Ambient, Diffuse, Specular;
		float SpecularIntensity;
	};
	//Gets references to the directional light uniforms for the given LitTexture material. Returns whether it was successful.
	static bool LitTexture_GetUniforms(Material & mat);
	//Sets the direcitonal light data for the given LitTexture material. Returns whether it was successful.
	static bool LitTexture_SetUniforms(Material & mat, const LitTexture_DirectionalLight & lightData);
};