#pragma once

#include <unordered_map>

#include "../../Math/Lower Math/Matrix4f.h"
#include "UniformValueVectorArray.h"
#include "UniformValueSubroutine.h"


//"Uniforms" are customizable parameters in OpenGL shaders.
//This file provides a way to store collections of them.


enum UniformTypes
{
    UT_VALUE_F,
    UT_VALUE_I,
    UT_VALUE_F_ARRAY,
    UT_VALUE_I_ARRAY,
    UT_VALUE_MAT4,
    UT_VALUE_SAMPLER2D,
    UT_VALUE_SAMPLER3D,
    UT_VALUE_SAMPLERCUBE,
    UT_VALUE_SUBROUTINE,
};


struct Uniform;
typedef std::vector<Uniform> UniformList;
typedef std::unordered_map<std::string, Uniform> UniformDictionary;


//A shader parameter.
struct Uniform
{
public:

    static std::string ToString(UniformTypes t);
    static UniformTypes FromString(const std::string& str);

    //Returns 0 if the given uniform isn't found.
    static const Uniform* Find(const UniformList& uniforms, const std::string& name);
    //Returns 0 if the given uniform isn't found.
    static Uniform* Find(UniformList& uniforms, const std::string& name);
    
    //Adds the uniforms in "src" to the "dest" collection.
    //Optionally overwrites uniforms that already exist.
    static void AddUniforms(const UniformDictionary& src, UniformDictionary& dest,
                            bool overwriteDuplicates);
    //Adds the uniforms in "src" to the "dest" collection.
    //Optionally overwrites uniforms that already exist.
    static void AddUniforms(const UniformList& src, UniformDictionary& dest,
                            bool overwriteDuplicates);

    static UniformList MakeList(const UniformDictionary& dict);
    static UniformDictionary MakeDict(const UniformList& list);


    std::string Name;
    UniformTypes Type;

    UniformLocation Loc = -1;


    //Use a union of the different kinds of uniform values.
    //Unfortunately, unions with complex objects aren't supported by the compiler,
    //    so here we use a byte array with the size of the largest type.

    char ByteData[sizeof(UniformValueSubroutine)];

    //Double-check that this is in fact the largest type available.
    static_assert(sizeof(UniformValueSubroutine) > sizeof(VectorF) &&
                   sizeof(UniformValueSubroutine) > sizeof(VectorI) &&
                   sizeof(UniformValueSubroutine) > sizeof(UniformValueArrayF) &&
                   sizeof(UniformValueSubroutine) > sizeof(UniformValueArrayI) &&
                   sizeof(UniformValueSubroutine) > sizeof(Matrix4f) &&
                   sizeof(UniformValueSubroutine) > sizeof(RenderObjHandle),
                  "UniformValueSubroutine isn't the biggest uniform value type anymore");
    
    //Provides a reference to this uniform's byte data, casted to the expected type.
#define ASSERT_TYPE(t) assert(Type == UT_VALUE_ ## t);
#define ASSERT_TEX assert(Type == UT_VALUE_SAMPLER2D || Type == UT_VALUE_SAMPLER3D || Type == UT_VALUE_SAMPLERCUBE);
    VectorF& Float(void)                     { ASSERT_TYPE(F);          return *(VectorF*)ByteData; }
    VectorI& Int(void)                       { ASSERT_TYPE(I);          return *(VectorI*)ByteData; }
    UniformValueArrayF& FloatArray(void)     { ASSERT_TYPE(F_ARRAY);    return *(UniformValueArrayF*)ByteData; }
    UniformValueArrayI& IntArray(void)       { ASSERT_TYPE(I_ARRAY);    return *(UniformValueArrayI*)ByteData; }
    Matrix4f& Matrix(void)                   { ASSERT_TYPE(MAT4);       return *(Matrix4f*)ByteData; }
    RenderObjHandle& Tex(void)               { ASSERT_TEX;              return *(RenderObjHandle*)ByteData; }
    UniformValueSubroutine& Subroutine(void) { ASSERT_TYPE(SUBROUTINE); return *(UniformValueSubroutine*)ByteData; }
    
    const VectorF& Float(void) const                     { ASSERT_TYPE(F);          return *(VectorF*)ByteData; }
    const VectorI& Int(void) const                       { ASSERT_TYPE(I);          return *(VectorI*)ByteData; }
    const UniformValueArrayF& FloatArray(void) const     { ASSERT_TYPE(F_ARRAY);    return *(UniformValueArrayF*)ByteData; }
    const UniformValueArrayI& IntArray(void) const       { ASSERT_TYPE(I_ARRAY);    return *(UniformValueArrayI*)ByteData; }
    const Matrix4f& Matrix(void) const                   { ASSERT_TYPE(MAT4);       return *(Matrix4f*)ByteData; }
    const RenderObjHandle& Tex(void) const               { ASSERT_TEX;              return *(RenderObjHandle*)ByteData; }
    const UniformValueSubroutine& Subroutine(void) const { ASSERT_TYPE(SUBROUTINE); return *(UniformValueSubroutine*)ByteData; }
#undef ASSERT_TYPE
#undef ASSERT_TEX
    

    //Makes a VectorF uniform.
    static Uniform MakeF(const std::string& name, size_t size, const float* vals,
                         UniformLocation loc = -1);
    //Makes a VectorI uniform.
    static Uniform MakeI(const std::string& name, size_t size, const int* vals,
                         UniformLocation loc = -1);

    //Makes an empty Matrix4f uniform.
    Uniform(void) : Uniform("", UT_VALUE_MAT4) { }
    //Makes any uniform except the VectorF or VectorI types.
    Uniform(std::string name, UniformTypes t, UniformLocation loc = -1);


    void GetDeclaration(std::string& outDecl) const;
};