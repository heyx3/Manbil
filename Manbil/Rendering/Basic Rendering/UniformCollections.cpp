#include "UniformCollections.h"

#include <iostream>

std::string Uniform::ToString(UniformTypes t)
{
    switch (t)
    {
        case UT_VALUE_F: return "Float";
        case UT_VALUE_I: return "Int";
        case UT_VALUE_F_ARRAY: return "FloatArray";
        case UT_VALUE_I_ARRAY: return "IntArray";
        case UT_VALUE_MAT4: return "Mat4";
        case UT_VALUE_SAMPLER2D: return "Tex2D";
        case UT_VALUE_SAMPLER3D: return "Tex3D";
        case UT_VALUE_SAMPLERCUBE: return "TexCube";
        case UT_VALUE_SUBROUTINE: return "Subroutine";
        default: return false;
    }
}
UniformTypes Uniform::FromString(const std::string& str)
{
    if (str == "Float")
        return UT_VALUE_F;
    else if (str == "Int")
        return UT_VALUE_I;
    else if (str == "FloatArray")
        return UT_VALUE_F_ARRAY;
    else if (str == "IntArray")
        return UT_VALUE_I_ARRAY;
    else if (str == "Mat4")
        return UT_VALUE_MAT4;
    else if (str == "Tex2D")
        return UT_VALUE_SAMPLER2D;
    else if (str == "Tex3D")
        return UT_VALUE_SAMPLER3D;
    else if (str == "TexCube")
        return UT_VALUE_SAMPLERCUBE;
    else if (str == "Subroutine")
        return UT_VALUE_SUBROUTINE;

    assert(false);
    return UT_VALUE_F;
}

const Uniform* Uniform::Find(const UniformList& uniforms, const std::string& name)
{
    for (unsigned int i = 0; i < uniforms.size(); ++i)
        if (uniforms[i].Name == name)
            return &uniforms[i];

    return 0;
}
Uniform* Uniform::Find(UniformList& uniforms, const std::string& name)
{
    for (unsigned int i = 0; i < uniforms.size(); ++i)
        if (uniforms[i].Name == name)
            return &uniforms[i];

    return 0;
}
void Uniform::AddUniforms(const UniformDictionary& src, UniformDictionary& dest, bool overwrite)
{
    for (auto iter = src.begin(); iter != src.end(); ++iter)
    {
        if (overwrite || dest.find(iter->first) == dest.end())
        {
            dest[iter->first] = iter->second;
        }
    }
}
void Uniform::AddUniforms(const UniformList& src, UniformDictionary& dest, bool overwrite)
{
    for (unsigned int i = 0; i < src.size(); ++i)
    {
        if (overwrite || dest.find(src[i].Name) == dest.end())
        {
            dest[src[i].Name] = src[i];
        }
    }
}

UniformList Uniform::MakeList(const UniformDictionary& dict)
{
    UniformList list;
    for (auto it = dict.begin(); it != dict.end(); ++it)
        list.push_back(it->second);
    return list;
}
UniformDictionary Uniform::MakeDict(const UniformList& list)
{
    UniformDictionary dict;
    for (unsigned int i = 0; i < list.size(); ++i)
        dict[list[i].Name] = list[i];
    return dict;
}


Uniform Uniform::MakeF(const std::string& name, size_t size, const float* vals, UniformLocation loc)
{
    Uniform u;
    u.Name = name;
    u.Type = UT_VALUE_F;
    u.Loc = loc;

    memset(u.ByteData, 0, sizeof(u.ByteData));
    switch (size)
    {
    case 1: u.Float().SetValue(vals[0]); break;
    case 2: u.Float().SetValue(Vector2f(vals[0], vals[1])); break;
    case 3: u.Float().SetValue(Vector3f(vals[0], vals[1], vals[2])); break;
    case 4: u.Float().SetValue(Vector4f(vals[0], vals[1], vals[2], vals[3])); break;
    default: std::cout << "ERROR: " << size << "\n";
    }
    //u.Float().SetValue(size, vals);

    return u;
}
Uniform Uniform::MakeI(const std::string& name, size_t size, const int* vals, UniformLocation loc)
{
    Uniform u;
    u.Name = name;
    u.Type = UT_VALUE_I;
    u.Loc = loc;

    memset(u.ByteData, 0, sizeof(u.ByteData));
    u.Int().SetValue(size, vals);

    return u;
}
Uniform::Uniform(std::string name, UniformTypes t, UniformLocation loc)
    : Name(name), Type(t), Loc(loc)
{
    assert(Type != UT_VALUE_F && Type != UT_VALUE_I);

    memset(ByteData, 0, sizeof(ByteData));

    switch (Type)
    {
        case UT_VALUE_F:
            Float() = VectorF();
            break;
        case UT_VALUE_I:
            Int() = VectorI();
            break;
        case UT_VALUE_F_ARRAY:
            FloatArray() = UniformValueArrayF();
            break;
        case UT_VALUE_I_ARRAY:
            IntArray() = UniformValueArrayI();
            break;

        case UT_VALUE_MAT4:
            Matrix() = Matrix4f();
            break;

        case UT_VALUE_SAMPLER2D:
        case UT_VALUE_SAMPLER3D:
        case UT_VALUE_SAMPLERCUBE:
            Tex() = INVALID_RENDER_OBJ_HANDLE;
            break;

        case UT_VALUE_SUBROUTINE:
            Subroutine() = UniformValueSubroutine();
            break;

        default:
            assert(false);
            break;
    }
}


void Uniform::GetDeclaration(std::string& outDecl) const
{
    switch (Type)
    {
        case UT_VALUE_F:
            outDecl += "uniform ";
            outDecl += Float().GetGLSLType();
            outDecl += " ";
            outDecl += Name;
            break;
        case UT_VALUE_I:
            outDecl += "uniform ";
            outDecl += Int().GetGLSLType();
            outDecl += " ";
            outDecl += Name;
            break;

        case UT_VALUE_F_ARRAY:
            outDecl += "uniform ";
            outDecl += VectorF::GetGLSLType(FloatArray().NComponentsPerValue);
            outDecl += " ";
            outDecl += Name;
            outDecl += "[";
            outDecl += std::to_string(FloatArray().NValues);
            outDecl += "]";
            break;
        case UT_VALUE_I_ARRAY:
            outDecl += "uniform ";
            outDecl += VectorI::GetGLSLType(IntArray().NComponentsPerValue);
            outDecl += " ";
            outDecl += Name;
            outDecl += "[";
            outDecl += std::to_string(IntArray().NValues);
            outDecl += "]";
            break;

        case UT_VALUE_MAT4:
            outDecl += "uniform mat4 ";
            outDecl += Name;
            break;

        case UT_VALUE_SAMPLER2D:
            outDecl += "uniform sampler2D ";
            outDecl += Name;
            break;
        case UT_VALUE_SAMPLER3D:
            outDecl += "uniform sampler3D ";
            outDecl += Name;
            break;
        case UT_VALUE_SAMPLERCUBE:
            outDecl += "uniform samplerCube ";
            outDecl += Name;
            break;

        case UT_VALUE_SUBROUTINE:
            const UniformValueSubroutine& uvs = Subroutine();
            outDecl += "subroutine ";
            outDecl += VectorF::GetGLSLType(uvs.Definition.ReturnValueSize);
            outDecl += " ";
            outDecl += uvs.Definition.Name;
            outDecl += "(";
            for (unsigned int i = 0; i < uvs.Definition.Params.size(); ++i)
            {
                if (i > 0)
                {
                    outDecl += ", ";
                }
                outDecl += uvs.Definition.Params[i].GetDeclaration();
            }
            outDecl += ");\n";
            outDecl += "subroutine uniform ";
            outDecl += uvs.Definition.Name;
            outDecl += " ";
            outDecl += Name;
            break;
    }
    
    outDecl += ";";
}