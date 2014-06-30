#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "../../OpenGLIncludes.h"
#include "../Texture Management/ManbilTexture.h"
#include "../../Math/Matrix4f.h"
#include "Data Nodes/Vector.h"


#define U_UMAP(Type) (std::unordered_map<std::string, Type>)
typedef std::shared_ptr<sf::Texture> SFTexPtr;


//TODO: Add matrix array uniforms. Add matrix array uniform set function to RenderDataHandler. Finally, make sure that Material handles matrix array uniforms.


//Represents a single float/vec2/vec3/vec4 uniform.
struct UniformValueF
{
public:
    float Value[4];
    unsigned int NData;
    std::string Name;
    UniformLocation Location;
    UniformValueF(float value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(1) { Value[0] = value; }
    UniformValueF(Vector2f value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(2) { Value[0] = value.x; Value[1] = value.y; }
    UniformValueF(Vector3f value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(3) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; }
    UniformValueF(Vector4f value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(4) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; Value[3] = value.w; }
    UniformValueF(const float * value = 0, unsigned int nData = 0, std::string name = "", UniformLocation loc = -1)
        : Name(name), Location(loc), NData(nData)
    {
        for (unsigned int i = 0; i < nData; ++i) Value[i] = value[i];
    }
    void SetValue(float value) { NData = 1; Value[0] = value; }
    void SetValue(Vector2f value) { NData = 2; Value[0] = value.x; Value[1] = value.y; }
    void SetValue(Vector3f value) { NData = 3; Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; }
    void SetValue(Vector4f value) { NData = 4; Value[0] = value.x; Value[1] = value.y; Value[2] = value.z;  Value[3] = value.w; }
    std::string GetDeclaration(void) const { return "uniform " + VectorF(NData).GetGLSLType() + " " + Name + ";"; }
};

//Represents an array of float/vec2/vec3/vec4 uniforms.
struct UniformArrayValueF
{
public:
    float * Values;
    unsigned int NumbValues, BasicTypesPerValue;
    UniformLocation Location;
    std::string Name;
    UniformArrayValueF(const float * values = 0, unsigned int nValues = 0, unsigned int nBasicTypesPerValue = 0, std::string name = "", UniformLocation loc = -1)
        : Name(name), Location(loc), Values(0), NumbValues(nValues), BasicTypesPerValue(nBasicTypesPerValue)
    {
        if (values != 0)
        {
            unsigned int count = NumbValues * BasicTypesPerValue;
            Values = new float[count];
            for (unsigned int i = 0; i < count; ++i) Values[i] = values[i];
        }
    }
    UniformArrayValueF(const UniformArrayValueF & cpy)
        : UniformArrayValueF(cpy.Values, cpy.NumbValues, cpy.BasicTypesPerValue, cpy.Name, cpy.Location) { }
    ~UniformArrayValueF(void)
    {
        if (Values != 0)
            delete[] Values;
    }
    void SetData(const float * values = 0, int nValues = -1, int nBasicTypesPerValue = -1)
    {
        if (nValues > 0 && nBasicTypesPerValue > 0)
        {
            delete[] values;
            NumbValues = nValues;
            BasicTypesPerValue = nBasicTypesPerValue;
            values = new float[nValues * nBasicTypesPerValue];
        }
        for (unsigned int i = 0; i < NumbValues * BasicTypesPerValue; ++i) Values[i] = values[i];
    }
    UniformArrayValueF & operator=(const UniformArrayValueF & cpy)
    {
        Name = cpy.Name;
        Location = cpy.Location;
        NumbValues = cpy.NumbValues;
        BasicTypesPerValue = cpy.BasicTypesPerValue;
        
        if (cpy.Values != 0)
        {
            unsigned int count = NumbValues * BasicTypesPerValue;
            Values = new float[count];
            for (unsigned int i = 0; i < count; ++i) Values[i] = cpy.Values[i];
        }

        return *this;
    }
    std::string GetDeclaration(void) const { return "uniform " + VectorF(BasicTypesPerValue).GetGLSLType() + " " + Name + "[" + std::to_string(NumbValues) + "];"; }
};


//Represents a single int/ivec2/ivec3/ivec4 uniform.
struct UniformValueI
{
public:
    int Value[4];
    unsigned int NData;
    std::string Name;
    UniformLocation Location;
    UniformValueI(int value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(1) { Value[0] = value; }
    UniformValueI(Vector2i value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(2) { Value[0] = value.x; Value[1] = value.y; }
    UniformValueI(Vector3i value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(3) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; }
    UniformValueI(Vector4i value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(4) { Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; Value[3] = value.w; }
    UniformValueI(const int * value = 0, unsigned int nData = 0, std::string name = "", UniformLocation loc = -1)
        : Name(name), Location(loc), NData(nData)
    {
        for (unsigned int i = 0; i < nData; ++i) Value[i] = value[i];
    }
    void SetValue(int value) { NData = 1; Value[0] = value; }
    void SetValue(Vector2i value) { NData = 2; Value[0] = value.x; Value[1] = value.y; }
    void SetValue(Vector3i value) { NData = 3; Value[0] = value.x; Value[1] = value.y; Value[2] = value.z; }
    void SetValue(Vector4i value) { NData = 4; Value[0] = value.x; Value[1] = value.y; Value[2] = value.z;  Value[3] = value.w; }
    std::string GetDeclaration(void) const { return "uniform " + VectorI(NData).GetGLSLType() + " " + Name + ";"; }
};

//Represents an array of int/ivec2/ivec3/ivec4 uniforms.
struct UniformArrayValueI
{
public:
    int * Values;
    unsigned int NumbValues, BasicTypesPerValue;
    UniformLocation Location;
    std::string Name;
    UniformArrayValueI(const int * values = 0, unsigned int nValues = 0, unsigned int nBasicTypesPerValue = 0, std::string name = "", UniformLocation loc = -1)
        : Name(name), Location(loc), Values(0), NumbValues(nValues), BasicTypesPerValue(nBasicTypesPerValue)
    {
        if (values != 0)
        {
            unsigned int count = NumbValues * BasicTypesPerValue;
            Values = new int[count];
            for (unsigned int i = 0; i < count; ++i) Values[i] = values[i];
        }
    }
    UniformArrayValueI(const UniformArrayValueI & cpy) : UniformArrayValueI(cpy.Values, cpy.NumbValues, cpy.BasicTypesPerValue, cpy.Name, cpy.Location) { }
    ~UniformArrayValueI(void) { if (Values != 0) delete[] Values; }
    void SetData(const int * values = 0, int nValues = -1, int nBasicTypesPerValue = -1)
    {
        if (nValues > 0 && nBasicTypesPerValue > 0)
        {
            delete[] values;
            NumbValues = nValues;
            BasicTypesPerValue = nBasicTypesPerValue;
            values = new int[nValues * nBasicTypesPerValue];
        }
        for (unsigned int i = 0; i < NumbValues * BasicTypesPerValue; ++i) Values[i] = values[i];
    }
    UniformArrayValueI & operator=(const UniformArrayValueI & cpy)
    {
        Name = cpy.Name;
        Location = cpy.Location;
        NumbValues = cpy.NumbValues;
        BasicTypesPerValue = cpy.BasicTypesPerValue;

        if (cpy.Values != 0)
        {
            unsigned int count = NumbValues * BasicTypesPerValue;
            Values = new int[count];
            for (unsigned int i = 0; i < count; ++i) Values[i] = cpy.Values[i];
        }

        return *this;
    }
    std::string GetDeclaration(void) const { return "uniform " + VectorI(BasicTypesPerValue).GetGLSLType() + " " + Name + "[" + std::to_string(NumbValues) + "];"; }
};

//Represents a uniform mat4.
struct UniformMatrixValue
{
public:
    Matrix4f Value;
    UniformLocation Location;
    std::string Name;
    UniformMatrixValue(void) { }
    UniformMatrixValue(const Matrix4f & value, std::string name, UniformLocation loc)
        : Name(name), Location(loc), Value(value) { }
    std::string GetDeclaration(void) const { return "uniform mat4 " + Name + ";"; }
};

//Represents a 2D texture sampler uniform.
struct UniformSamplerValue
{
public:
    ManbilTexture1 * Texture;
    UniformLocation Location;
    std::string Name;
    UniformSamplerValue(ManbilTexture1 * texture, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Texture(texture) { }
    UniformSamplerValue(std::string name = "") : Name(name), Location(-1), Texture(0) { }
    std::string GetDeclaration(void) const { return "uniform sampler2D " + Name + ";"; }
};


//Represents a definition of a subroutine.
struct SubroutineDefinition
{
public:

    //An input into the subroutine.
    struct Parameter
    {
        unsigned int Size;
        std::string Name;
        Parameter(unsigned int size = 0, std::string name = "") : Size(size), Name(name) { }
        std::string GetDeclaration(void) const { return VectorF(Size).GetGLSLType() + " " + Name; }
    };

    //The size of the subroutine's output (float, vec2, vec3, or vec4).
    unsigned int ReturnValueSize;
    //The name of this subroutine definition.
    std::string Name;
    //The inputs to this subroutine.
    std::vector<Parameter> Params;

    //The shader this uniform resides in.
    ShaderHandler::Shaders Shader;


    SubroutineDefinition(ShaderHandler::Shaders shader = ShaderHandler::Shaders::SH_Vertex_Shader, unsigned int returnValSize = 0,
                         std::string name = "", std::vector<Parameter> params = std::vector<Parameter>())
        : Shader(shader), ReturnValueSize(returnValSize), Name(name), Params(params) { }


    std::string GetDefinition(void) const;
};

//Represents a subroutine.
struct UniformSubroutineValue
{
public:

    //The definition of the subroutine.
    //Multiple values can have the same definition, so point to a managed, heap-allocated definition.
    std::shared_ptr<SubroutineDefinition> Definition;

    //The name of the uniform definition.
    std::string Name;
    //The location of the uniform in the GLSL program.
    UniformLocation Location;

    //The values this subroutine uniform can have.
    std::vector<std::string> PossibleValues;
    //Parallel to "PossibleValues". Provides the id for each coroutine function.
    std::vector<RenderObjHandle> PossibleValueIDs;
    //The index in the "PossibleValues" collection corresponding to the current value of the subroutine.
    unsigned int ValueIndex = 0;

    //Creates a new instance. Fills in the "PossibleValueIDs" vector with default values of 0.
    UniformSubroutineValue(std::shared_ptr<SubroutineDefinition> definition = std::shared_ptr<SubroutineDefinition>(),
                           std::vector<std::string> possibleValues = std::vector<std::string>(),
                           unsigned int currentValueIndex = 0, std::string name = "", UniformLocation location = 0)
        : Definition(definition), PossibleValues(possibleValues), ValueIndex(currentValueIndex), Name(name), Location(location)
    {
        PossibleValueIDs.reserve(PossibleValues.size());
        for (unsigned int i = 0; i < PossibleValues.size(); ++i)
            PossibleValueIDs.insert(PossibleValueIDs.end(), 0);
    }

    std::string GetDeclaration(void) const { return "subroutine uniform " + Definition->Name + " " + Name + ";"; }
};




//Represents a list of uniforms.
struct UniformList
{
public:
    struct Uniform { public: std::string Name; UniformLocation Loc; Uniform(std::string name, UniformLocation loc = -1) : Name(name), Loc(loc) { } };
    std::vector<Uniform> FloatUniforms, FloatArrayUniforms,
                         MatrixUniforms, TextureUniforms,
                         IntUniforms, IntArrayUniforms,
                         SubroutineUniforms;
    //Returns "Uniform("", -1)" if the given name isn't found.
    static Uniform FindUniform(std::string name, const std::vector<Uniform> & toSearch)
    {
        for (unsigned int i = 0; i < toSearch.size(); ++i)
            if (toSearch[i].Name == name)
                return toSearch[i];
        return Uniform("");
    }
};

//Represents a collection of uniform locations.
//TODO: Change it so that the DataNodes write to UniformLists instead of UniformDictionaries.
struct UniformDictionary
{
public:
    U_UMAP(UniformValueF) FloatUniforms;
    U_UMAP(UniformArrayValueF) FloatArrayUniforms;
    U_UMAP(UniformValueI) IntUniforms;
    U_UMAP(UniformArrayValueI) IntArrayUniforms;
    U_UMAP(UniformMatrixValue) MatrixUniforms;
    U_UMAP(UniformSamplerValue) TextureUniforms;
    U_UMAP(UniformSubroutineValue) SubroutineUniforms;

    void AddUniforms(const UniformDictionary & other, bool overwriteDuplicates);
    void ClearUniforms(void);

    unsigned int GetNumbUniforms(void) const;
};