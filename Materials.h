#pragma once

#include <string>
#include "Vectors.h"

class Material;

//Represents the combiniation of a vertex shader and a fragment shader.
struct MaterialShaders
{
	std::string VertexShader, FragmentShader;
	MaterialShaders(std::string vs, std::string fs) : VertexShader(vs), FragmentShader(fs) { }
};


//Contains different useful materials.
struct Materials
{
	static const MaterialShaders UnlitTexture, LitTexture, BareColor;

	//The exact struct that the "LitTexture" material uses. In the shader it is called "DirectionalLight".
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