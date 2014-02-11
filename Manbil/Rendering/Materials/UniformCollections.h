#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "../../OpenGLIncludes.h"
#include <SFML/Graphics/Texture.hpp>
#include "../../Math/Matrix4f.h"


#define U_UMAP(Type) (std::unordered_map<std::string, Type>)
typedef std::shared_ptr<sf::Texture> SFTexPtr;




//Represents a single float/vec2/vec3/vec4 uniform.
struct UniformValue
{
public:
    float Value[4];
    int NData;
    std::string Name;
    UniformLocation Location;
    UniformValue(float value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(1) { Value[0] = value; }
    UniformValue(Vector2f value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(2) { Value[0] = value.x; Value[1] = value.y; }
    UniformValue(Vector3f value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(3) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; }
    UniformValue(Vector4f value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(4) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; Value[3] = value.w; }
    UniformValue(float * value, int nData, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(nData) { for (int i = 0; i < nData; ++i) Value[i] = value[i]; }
};

//Represents an array of float/vec2/vec3/vec4 uniforms.
struct UniformArrayValue
{
public:
    float * Values;
    int NumbValues, BasicTypesPerValue;
    UniformLocation Location;
    std::string Name;
    UniformArrayValue(float * values, int nValues, int nBasicTypesPerValue, UniformLocation loc, std::string name) : Name(name), Location(loc), Values(values), NumbValues(nValues), BasicTypesPerValue(nBasicTypesPerValue) { }
};

//Represents a uniform mat4.
struct UniformMatrixValue
{
public:
    Matrix4f Value;
    UniformLocation Location;
    std::string Name;
    UniformMatrixValue(const Matrix4f & value, UniformLocation loc, std::string name) : Name(name), Location(loc), Value(value) { }
};

//Represents a 2D texture sampler uniform.
struct UniformSamplerValue
{
public:
    SFTexPtr Texture;
    UniformLocation Location;
    std::string Name;
    UniformSamplerValue(SFTexPtr texture, UniformLocation loc, std::string name) : Name(name), Location(loc), Texture(texture) { }
};




//Represents a list of uniforms.
struct UniformList
{
public:
    std::vector<std::string> FloatUniforms, FloatArrayUniforms, MatrixUniforms, TextureUniforms;
};

//Represents a collection of uniform locations.
struct UniformDictionary
{
public:
    U_UMAP(UniformValue) FloatUniforms;
    U_UMAP(UniformArrayValue) FloatArrayUniforms;
    U_UMAP(UniformMatrixValue) MatrixUniforms;
    U_UMAP(UniformSamplerValue) TextureUniforms;
};