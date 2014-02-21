#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "../../OpenGLIncludes.h"
#include <SFML/Graphics/Texture.hpp>
#include "../../Math/Matrix4f.h"
#include "Data Nodes/Vector.h"


#define U_UMAP(Type) (std::unordered_map<std::string, Type>)
typedef std::shared_ptr<sf::Texture> SFTexPtr;




//Represents a single float/vec2/vec3/vec4 uniform.
struct UniformValue
{
public:
    float Value[4];
    unsigned int NData;
    std::string Name;
    UniformLocation Location;
    UniformValue(float value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(1) { Value[0] = value; }
    UniformValue(Vector2f value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(2) { Value[0] = value.x; Value[1] = value.y; }
    UniformValue(Vector3f value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(3) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; }
    UniformValue(Vector4f value, UniformLocation loc, std::string name) : Name(name), Location(loc), NData(4) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; Value[3] = value.w; }
    UniformValue(const float * value = 0, unsigned int nData = 0, UniformLocation loc = 0, std::string name = "") : Name(name), Location(loc), NData(nData) { for (int i = 0; i < nData; ++i) Value[i] = value[i]; }
    std::string GetDeclaration(void) const { return "uniform " + Vector(NData).GetGLSLType() + " " + Name + ";"; }
};

//Represents an array of float/vec2/vec3/vec4 uniforms.
struct UniformArrayValue
{
public:
    float * Values;
    unsigned int NumbValues, BasicTypesPerValue;
    UniformLocation Location;
    std::string Name;
    UniformArrayValue(float * values = 0, unsigned int nValues = 0, unsigned  int nBasicTypesPerValue = 0, UniformLocation loc = 0, std::string name = "") : Name(name), Location(loc), Values(values), NumbValues(nValues), BasicTypesPerValue(nBasicTypesPerValue) { }
    std::string GetDeclaration(void) const { return "uniform " + Vector(BasicTypesPerValue).GetGLSLType() + " " + Name + "[" + std::to_string(NumbValues) + "];"; }
};

//Represents a uniform mat4.
struct UniformMatrixValue
{
public:
    Matrix4f Value;
    UniformLocation Location;
    std::string Name;
    UniformMatrixValue(void) { }
    UniformMatrixValue(const Matrix4f & value, UniformLocation loc, std::string name) : Name(name), Location(loc), Value(value) { }
    std::string GetDeclaration(void) const { return "uniform mat4 " + Name + ";"; }
};

//Represents a 2D texture sampler uniform.
struct UniformSamplerValue
{
public:
    sf::Texture * Texture;
    UniformLocation Location;
    std::string Name;
    UniformSamplerValue(sf::Texture * texture = 0, UniformLocation loc = 0, std::string name = "") : Name(name), Location(loc), Texture(texture) { }
    std::string GetDeclaration(void) const { return "uniform sampler2D " + Name + ";"; }
};




//Represents a list of uniforms.
struct UniformList
{
public:
    struct Uniform { public: std::string Name; UniformLocation Loc; Uniform(std::string name, UniformLocation loc) : Name(name), Loc(loc) { } };
    std::vector<Uniform> FloatUniforms, FloatArrayUniforms,
                         MatrixUniforms, TextureUniforms;
    //Returns "Uniform("", 0)" if the given name isn't found.
    static Uniform FindUniform(std::string name, const std::vector<Uniform> & toSearch)
    {
        for (int i = 0; i < toSearch.size(); ++i)
            if (toSearch[i].Name == name)
                return toSearch[i];
        return Uniform("", 0);
    }
};

//Represents a collection of uniform locations.
//TODO: Change it so that the DataNodes write to UniformLists instead of UniformDictionaries.
struct UniformDictionary
{
public:
    U_UMAP(UniformValue) FloatUniforms;
    U_UMAP(UniformArrayValue) FloatArrayUniforms;
    U_UMAP(UniformMatrixValue) MatrixUniforms;
    U_UMAP(UniformSamplerValue) TextureUniforms;
};