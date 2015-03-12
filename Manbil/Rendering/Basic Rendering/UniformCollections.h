#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "OpenGLIncludes.h"
#include "../../Math/Lower Math/Matrix4f.h"
#include "GLVectors.h"


//"Uniforms" are customizable parameters in OpenGL shaders.
//This file provides various data structures that facilitate their definition and use.


//TODO: Consider adding 1D texture uniforms.


#pragma region Template definition for UniformValueF and UniformValueI

//A version of "GLSLVector" such as "VectorF" or "VectorI".
template<typename GLVectorType, typename MyComponent,
         typename MyVec2, typename MyVec3, typename MyVec4>
//A single vector of some type (float, int, etc).
struct UniformValueVector
{
public:

    MyComponent Value[4];
    //The number of components in this vector.
    unsigned int NData;
    std::string Name;
    UniformLocation Location;

    UniformValueVector(MyComponent value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(1)
    {
        memcpy(Value, &value, sizeof(MyComponent));
    }
    UniformValueVector(MyVec2 value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(2)
    {
        memcpy(Value, &value.x, sizeof(MyVec2));
    }
    UniformValueVector(MyVec3 value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(3)
    {
        memcpy(Value, &value.y, sizeof(MyVec3));
    }
    UniformValueVector(MyVec4 value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), NData(4)
    {
        memcpy(Value, &value.z, sizeof(MyVec4));
    }
    UniformValueVector(const MyComponent* value = 0, unsigned int nData = 0,
                       std::string name = "", UniformLocation loc = -1)
        : Name(name), Location(loc), NData(nData)
    {
        assert(value == 0 || (nData >= 1 && nData <= 4));
        if (value != 0)
        {
            memcpy(Value, value, sizeof(value));
        }
    }

    void SetValue(MyComponent value)
    {
        NData = 1;
        memcpy(Value, &value, sizeof(MyComponent));
    }
    void SetValue(MyVec2 value)
    {
        NData = 2;
        memcpy(Value, &value.x, sizeof(MyVec2));
    }
    void SetValue(MyVec3 value)
    {
        NData = 3;
        memcpy(Value, &value.x, sizeof(MyVec3));
    }
    void SetValue(MyVec4 value)
    {
        NData = 4;
        memcpy(Value, &value.x, sizeof(MyVec4));
    }

    std::string GetDeclaration(void) const
    {
        return "uniform " + GLVectorType::GetGLSLType(NData) + " " + Name + ";";
    }
};

#pragma endregion

//A single float vector.
typedef UniformValueVector<VectorF, float, Vector2f, Vector3f, Vector4f> UniformValueF;
//A single signed integer vector.
typedef UniformValueVector<VectorI, int, Vector2i, Vector3i, Vector4i> UniformValueI;



#pragma region Template definition for UniformValueArrayF and UniformValueArrayI

//A version of "GLSLVector" such as "VectorF" or "VectorI".
template<typename GLVectorType, typename ComponentType>
//Represents an array of vectors of some type (float, int, etc).
struct UniformValueVectorArray
{
private:

    typedef UniformValueVectorArray<GLVectorType, ComponentType> ThisType;


public:

    ComponentType* Values;
    unsigned int NValues, NComponentsPerValue;
    std::string Name;
    UniformLocation Location;


    UniformValueVectorArray(const ComponentType* values = 0, unsigned int nValues = 0,
                            unsigned int nComponentsPerValue = 0, std::string name = "",
                            UniformLocation loc = -1)
        : Name(name), Location(loc), Values(0),
          NValues(nValues), NComponentsPerValue(nComponentsPerValue)
    {
        if (values != 0)
        {
            unsigned int count = NValues * NComponentsPerValue;
            Values = new ComponentType[count];
            for (unsigned int i = 0; i < count; ++i)
            {
                Values[i] = values[i];
            }
        }
    }
    UniformValueVectorArray(const ThisType& cpy) { operator=(cpy); }
    UniformValueVectorArray(ThisType&& other) { *this = std::move(other); }

    ThisType& operator=(const ThisType& cpy)
    {
        Name = cpy.Name;
        Location = cpy.Location;
        NValues = cpy.NValues;
        NComponentsPerValue = cpy.NComponentsPerValue;

        Values = new ComponentType[NValues * NComponentsPerValue];
        assert(sizeof(Values) == sizeof(cpy.Values));
        memcpy(Values, cpy.Values, sizeof(ComponentType) * sizeof(Values));

        return *this;
    }
    ThisType& operator=(ThisType&& other)
    {
        Name = other.Name;
        Location = other.Location;
        Values = other.Values;
        NValues = other.NValues;
        NComponentsPerValue = other.NComponentsPerValue;
        
        other.Values = 0;
        other.Location = -1;
        other.NValues = 0;
        other.NComponentsPerValue = 0;
        other.Name.clear();

        return *this;
    }

    ~UniformValueVectorArray(void)
    {
        if (Values != 0)
        {
            delete[] Values;
        }
    }


    //Copies this value's data to the given instance.
    void CopyTo(ThisType& other) const
    {
        other = ThisType(Values, NValues, NComponentsPerValue, Name, Location);
    }

    void SetData(const ComponentType* values = 0, unsigned int nValues = 0,
                 unsigned int nComponentsPerValue = 0)
    {
        //If the array needs to be resized, do so.
        if (values != 0)
        {
            if ((nValues * nComponentsPerValue) != (NValues * NComponentsPerValue))
            {
                delete[] Values;
                Values = new ComponentType[nValues * nComponentsPerValue];
            }
        }
        else
        {
            Values = new ComponentType[nValues * nComponentsPerValue];
        }
        
        //Now copy the values in.
        NValues = nValues;
        NComponentsPerValue = nComponentsPerValue;
        memcpy(Values, values, sizeof(ComponentType) * NValues * NComponentsPerValue);
    }


    std::string GetDeclaration(void) const
    {
        assert(NValues > 0);
        return "uniform " + GLVectorType::GetGLSLType(NComponentsPerValue) +
                  " " + Name + "[" + std::to_string(NValues) + "];";
    }
};

#pragma endregion

//An array of float vectors.
typedef UniformValueVectorArray<VectorF, float> UniformValueArrayF;
//An array of signed integer vectors.
typedef UniformValueVectorArray<VectorI, int> UniformValueArrayI;



//Represents a 4x4 matrix.
struct UniformValueMatrix4f
{
public:

    Matrix4f Value;
    UniformLocation Location;
    std::string Name;

    UniformValueMatrix4f(void) { }
    UniformValueMatrix4f(const Matrix4f& value, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Value(value) { }

    std::string GetDeclaration(void) const { return "uniform mat4 " + Name + ";"; }
};



//Represents a 2D texture sampler.
struct UniformValueSampler2D
{
public:

    RenderObjHandle Texture;
    UniformLocation Location;
    std::string Name;

    UniformValueSampler2D(RenderObjHandle textureHandle, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Texture(textureHandle) { }
    UniformValueSampler2D(std::string name = "") : Name(name), Location(-1), Texture(0) { }

    std::string GetDeclaration(void) const { return "uniform sampler2D " + Name + ";"; }
};


//Represents a 3D texture sampler.
struct UniformValueSampler3D
{
public:

    RenderObjHandle Texture;
    UniformLocation Location;
    std::string Name;

    UniformValueSampler3D(RenderObjHandle textureHandle, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Texture(textureHandle) { }
    UniformValueSampler3D(std::string name = "") : Name(name), Location(-1), Texture(0) { }

    std::string GetDeclaration(void) const { return "uniform sampler3D " + Name + ";"; }
};


//Represents a cubemap texture sampler.
struct UniformValueSamplerCubemap
{
public:

    RenderObjHandle Texture;
    UniformLocation Location;
    std::string Name;

    UniformValueSamplerCubemap(RenderObjHandle textureHandle, std::string name, UniformLocation loc = -1)
        : Name(name), Location(loc), Texture(textureHandle) { }
    UniformValueSamplerCubemap(std::string name = "") : Name(name), Location(-1), Texture(0) { }

    std::string GetDeclaration(void) const { return "uniform samplerCube " + Name + ";"; }
};



//Represents a definition of a subroutine value (i.e. a function with a specific signature).
//Each definition is a specific possible value of a "subroutine" uniform.
struct SubroutineDefinition
{
public:

    //A potential input into the subroutine.
    struct Parameter
    {
        unsigned int Size;
        std::string Name;
        Parameter(unsigned int size = 0, std::string name = "") : Size(size), Name(name) { }
        std::string GetDeclaration(void) const { return VectorF::GetGLSLType(Size) + " " + Name; }
    };

    //The size of the subroutine's output (float, vec2, vec3, or vec4).
    unsigned int ReturnValueSize;
    //The name of this subroutine definition.
    std::string Name;
    //The inputs to this subroutine.
    std::vector<Parameter> Params;

    //The shader this uniform resides in.
    Shaders Shader;


    SubroutineDefinition(Shaders shader = Shaders::SH_VERTEX, unsigned int returnValSize = 0,
                         std::string name = "",
                         std::vector<Parameter> params = std::vector<Parameter>())
        : Shader(shader), ReturnValueSize(returnValSize), Name(name), Params(params) { }


    std::string GetDefinition(void) const;
};

//Represents a subroutine.
//Subroutines are essentially OpenGL's version of function pointers.
struct UniformValueSubroutine
{
public:

    typedef std::shared_ptr<SubroutineDefinition> SubroutineDefinitionPtr;


    //The definition of the subroutine.
    //Multiple values can have the same definition, so point to a managed, heap-allocated definition.
    SubroutineDefinitionPtr Definition;

    //The name of the uniform definition.
    std::string Name;
    //The location of the uniform in the GLSL program.
    UniformLocation Location;

    //The values (i.e. functions) this subroutine uniform can have.
    std::vector<std::string> PossibleValues;
    //Parallel to "PossibleValues". Provides the id for each coroutine function.
    std::vector<RenderObjHandle> PossibleValueIDs;
    //The index in the "PossibleValues" collection corresponding to the current value of the subroutine.
    unsigned int ValueIndex = 0;

    //Creates a new instance. Fills in the "PossibleValueIDs" vector with default values of 0.
    UniformValueSubroutine(SubroutineDefinitionPtr definition = SubroutineDefinitionPtr(),
                           std::vector<std::string> possibleValues = std::vector<std::string>(),
                           unsigned int currentValueIndex = 0, std::string name = "",
                           UniformLocation location = 0);

    std::string GetDeclaration(void) const;
};



//A collection of all uniforms a shader posesses.
struct UniformList
{
public:

    struct Uniform
    {
    public:
        std::string Name;
        UniformLocation Loc;
        Uniform(std::string name, UniformLocation loc = -1) : Name(name), Loc(loc) { }
    };

    std::vector<Uniform> Floats, Ints, FloatArrays, IntArrays, Matrices, Subroutines,
                         Texture2Ds, Texture3Ds, TextureCubemaps;

    //Returns "Uniform("", -1)" if the given name isn't found.
    Uniform FindUniform(std::string name) const;
};

//Represents a collection of uniform locations.
//TODO: Change it so that the DataNodes write to UniformLists instead of UniformDictionaries.
struct UniformDictionary
{
public:

#define UMAP(Type) std::unordered_map<std::string, Type>
    UMAP(UniformValueF)              Floats;
    UMAP(UniformValueArrayF)         FloatArrays;
    UMAP(UniformValueI)              Ints;
    UMAP(UniformValueArrayI)         IntArrays;
    UMAP(UniformValueMatrix4f)       Matrices;
    UMAP(UniformValueSampler2D)      Texture2Ds;
    UMAP(UniformValueSampler3D)      Texture3Ds;
    UMAP(UniformValueSamplerCubemap) TextureCubemaps;
    UMAP(UniformValueSubroutine)     Subroutines;
#undef UMAP

    //Adds the given uniforms to this collection, optionally overwriting duplicates.
    void AddUniforms(const UniformDictionary& other, bool overwriteDuplicates);
    //Clears all uniforms from this collection.
    void ClearUniforms(void);

    //Counts the total number of uniforms in this collection.
    unsigned int GetNumbUniforms(void) const;
};