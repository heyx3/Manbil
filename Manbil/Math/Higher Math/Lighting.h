#pragma once

#include "../Lower Math/Vectors.h"
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

	DirectionalLight(float diffuseIntensity = 0.8f, float ambientIntensity = 0.2f, Vector3f col = Vector3f(1, 1, 1),
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