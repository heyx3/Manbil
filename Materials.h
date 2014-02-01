#pragma once

#include <string>
#include "Math/Vectors.h"
#include "Material.h"
#include "Mesh.h"


typedef std::unordered_map<std::string, Mesh::UniformValue<float>> FloatUniforms;
typedef std::unordered_map<std::string, Mesh::UniformValue<int>> IntUniforms;
typedef std::unordered_map<std::string, Matrix4f> MatUniforms;


//Contains different useful materials.
struct Materials
{
	static const MaterialShaders UnlitTexture, LitTexture, BareColor;
    static void GetDefaultUniforms_BareColor(FloatUniforms & floats, IntUniforms & ints, MatUniforms & mats) { }
    static void GetDefaultUniforms_UnlitTexture(FloatUniforms & floats, IntUniforms & ints, MatUniforms & mats) { float f = 1.0f; floats["brightness"] = Mesh::UniformValue<float>(&f, 1); }
    static void GetDefaultUniforms_LitTexture(FloatUniforms & floats, IntUniforms & ints, MatUniforms & mats);

	//The exact struct that the "LitTexture" material uses.
	struct LitTexture_DirectionalLight
	{
		Vector3f Dir, Col;
		float Ambient, Diffuse, Specular;
		float SpecularIntensity;
        LitTexture_DirectionalLight(void) : Dir(1.0f, 0.0f, 0.0f), Col(1.0f, 1.0f, 1.0f), Ambient(0.1f), Diffuse(0.9f), Specular(1.0f), SpecularIntensity(32.0f) { }
    };
    //Gets references to the directional light uniforms for the given LitTexture material. Returns whether it was successful.
    static bool LitTexture_GetUniforms(Material & mat);
    //Sets the direcitonal light data for the given LitTexture material. Returns whether it was successful.
    static bool LitTexture_SetUniforms(Material & mat, const LitTexture_DirectionalLight & lightData);
    //Gets references to the directional light uniforms for the given LitTexture material2. Returns whether it was successful.
    static bool LitTexture_GetUniforms(Material2 & mat);
    //Sets the direcitonal light data for the given LitTexture material2. Returns whether it was successful.
    static bool LitTexture_SetUniforms(Material2 & mat, const LitTexture_DirectionalLight & lightData);
    //Sets the given mesh's light data to the given value.
    static void LitTexture_SetUniforms(Mesh & mesh, const LitTexture_DirectionalLight & lightData);
};