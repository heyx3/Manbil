#pragma once

#include "Vectors.h"
#include <string>


//Specifies a constant, linear, and quadratic component for the dropoff of a light's intensity.
struct LightAttenuationConstants
{
	float Constant;
	float Linear;
	float Quad;
	LightAttenuationConstants(float cst, float lin, float qu) : Constant(cst), Linear(lin), Quad(qu) { }

	//Takes the distance to the light and the full strength of the light, and return the new strength of the light.
	float GetAttenuation(float distanceToLight, float fullStrength) const
	{
		return fullStrength / (Constant + (Linear * distanceToLight) + (Quad * distanceToLight * distanceToLight));
	}
};


//The basic data necessary for any kind of light source.
struct BaseLight
{
	Vector3f Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight(float diffuseIntensity, Vector3f color = Vector3f(1, 1, 1), float ambientIntensity = 0.001f)
		: Color(color), AmbientIntensity(ambientIntensity), DiffuseIntensity(diffuseIntensity) { }
};


//A global, omni-directional light.
struct DirectionalLight : public BaseLight
{
	Vector3f Direction;

	DirectionalLight(float diffuseIntensity, float ambientIntensity = 0.001f, Vector3f col = Vector3f(1, 1, 1),
					 Vector3f dir = Vector3f(1, 0, 0))
		: BaseLight(diffuseIntensity, col, ambientIntensity), Direction(dir.Normalized()){ }

	/*Gets the declaration for this structure in the shader language. Calls itself "DirLight".
	  Uses the following names for data:
	  "Dir", "Col", "Ambient", "Diffuse".
	*/
	static std::string ShaderStructDeclaration(void)
	{
		return std::string("struct DirLight { vec3 Dir; vec3 Col; float Ambient; float Diffuse; };\n");
	}
};
//A light source with a specific position and attenuation.
struct PointLight : public BaseLight
{
	Vector3f Position;
	LightAttenuationConstants Attenuation;

	PointLight(Vector3f position, LightAttenuationConstants atten,
			  float diffuseIntensity, float ambientIntensity = 0.001f,
			  Vector3f col = Vector3f(1, 1, 1))
			  : BaseLight(diffuseIntensity, col, ambientIntensity), Position(position), Attenuation(atten) { }

	/*Gets the declaration for this structure in the shader language. Calls itself "PointLight".
	  Uses the following names for data:
	  "Pos", "Col", "Ambient", "Diffuse", "Atten.Const", "Atten.Linear", "Atten.Quad".
	*/
	static std::string ShaderStructDeclaration(void)
	{
		return std::string("struct PointLight { vec3 Pos; vec3 Col; float Ambient, float Diffuse, struct { float Const; float Linear; float Quad; } Atten; };\n");
	}
};

//Generates snippets of shader code.
namespace ShaderCodeGenerator
{
	//Holds the names/expressions representing different shader variables/uniforms used for Phong lighting.
	struct PhongLightingVarInfo
	{
		//The name of the vec4 to create and store the final light value in.
		const char * outColName;

		//A shader expression/name for getting the position of the camera in the world.
		const char * eyePos;
		//A shader expression/name for getting the world position of this fragment.
		const char * fragWorldPos;
		//A shader expression/name for getting the direction of the light hitting this fragment.
		const char * lightDir;
		//A shader expression/name for getting the surface normal for this fragment.
		const char * surfaceNormal;
		//A shader expression/name for getting the specular intensity for this fragment.
		const char * specularIntensity;
		//A shader expression/name for getting the specular power for this fragment.
		const char * specularPower;
		
		//The name of the shader struct holding the light data -- "Col", "Ambient", and "Diffuse".
		const char * lightStructName;

		PhongLightingVarInfo(const char * _outCol, const char * _eyePos, const char * _fragWorldPos,
							 const char * _lightDir, const char * _surfaceNorm, const char * _specIntensity,
							 const char * _specPower, const char * _lightStructName)
							 : outColName(_outCol), eyePos(_eyePos), fragWorldPos(_fragWorldPos),
							 lightDir(_lightDir), surfaceNormal(_surfaceNorm), specularIntensity(_specIntensity),
							 specularPower(_specPower), lightStructName(_lightStructName) { }
	};
	//Generates the lighting code for a basic ambient, diffuse, and specular lighting system.
	//"tempVarsSuffix" is a suffix for temp variable names used in this snippet.
	//   Providing a unique suffix prevents this code snippet from using temp variables
	//   that have already been declared by other shader code.
	void GetPhongLightingCode(std::string & out, char * tempVarsSuffix, PhongLightingVarInfo & info);
}