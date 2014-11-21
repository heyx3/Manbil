#pragma once

#include "Math/Lower Math/Vectors.h"
#include "OpenGLIncludes.h"
#include "IO/DataSerialization.h"



//Represents a vertex type. Each vertex type has a set of attributes.
//For example, a 3D mesh may have vertices with three attributes: position, UV, and normal.
//Each attribute has a certain "size", i.e. the number of floats it has.
//For example, surface normals have three floats, while UV coordinates have two.
//This class makes the assumption that the size of the vertex object
//    is exactly equal to the cumulative size of the attributes.
//For example, if the vertex only has UV and Normal, it should be the size of (2 + 3) * sizeof(float).
//It also assumes that the attributes are ordered in the order this class was given.
//TODO: This is used as input/output data specification for all the different kinds of shaders, so rename this something like "ShaderInOutAttributes".
class ShaderInOutAttributes : public ISerializable
{
public:
    
    //The maximum-allowable number of vertex attributes (independent of size).
    //TODO: There is probably an OpenGL constant that can be queried to ensure no instances are ever constructed with too many values.
    static const int MAX_ATTRIBUTES = 5;


    ShaderInOutAttributes(void) { attributeSizes[0] = -1; }
    ShaderInOutAttributes(int attribSize, bool normalized, std::string name);
    ShaderInOutAttributes(int attribSize1, int attribSize2, bool normalized1, bool normalized2, std::string name1, std::string name2);
    ShaderInOutAttributes(int attribSize1, int attribSize2, int attribSize3, bool normalized1, bool normalized2, bool normalized3,
                     std::string name1, std::string name2, std::string name3);
    ShaderInOutAttributes(int attribSize1, int attribSize2, int attribSize3, int attribSize4,
                     bool normalized1, bool normalized2, bool normalized3, bool normalized4,
                     std::string name1, std::string name2, std::string name3, std::string name4);
    ShaderInOutAttributes(int attribSize1, int attribSize2, int attribSize3, int attribSize4, int attribSize5,
                     bool normalized1, bool normalized2, bool normalized3, bool normalized4, bool normalized5,
                     std::string name1, std::string name2, std::string name3, std::string name4, std::string name5);
    ShaderInOutAttributes(const ShaderInOutAttributes & cpy);

    
    //Gets the number of attributes for the vertex type this instance represents.
    unsigned int GetNumbAttributes(void) const
    {
        unsigned int n = 0;
        while (n < MAX_ATTRIBUTES && IsValidAttribute(n))
            ++n;
        return n;
    }
    //Returns 0 if the given index doesn't point to a valid attribute. Otherwise, returns the size of the attribute.
    unsigned int GetAttributeSize(unsigned int index) const { return (IsValidAttribute(index) ? (unsigned int)attributeSizes[index] : 0); }
    //Returns whether the given attribute will be normalized.
    bool GetAttributeNormalized(unsigned int index) const { return attributeNormalized[index]; }
    //Returns the name of the given attribute.
    std::string GetAttributeName(unsigned int index) const { return attributeNames[index]; }


    //Checks whether the two attributes have the exact same attribute properties.
    bool operator==(const ShaderInOutAttributes & other) const;


    //Prepares the type of vertices represented by this instance to be used by OpenGL.
    //Returns whether the operation was successful.
    //MUST BE CALLED *AFTER* BINDING VERTEX BUFFER!
    bool EnableAttributes(void) const;
    //Cleans up after using the type of vertices represented by this instance.
    //Returns whether the operation was successful.
    bool DisableAttributes(void) const;

    virtual bool WriteData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadData(DataReader * reader, std::string & outError) override;


private:

    //Gets whether the given attribute index points to a valid attribute.
    bool IsValidAttribute(unsigned int index) const { return index < MAX_ATTRIBUTES && attributeSizes[index] >= 1 && attributeSizes[index] <= 4; }

    //The number of floats in each attribute. A valid value is between 1 and 4, inclusive.
    //Any invalid value in this array is treated as the end of the valid values
    //   (e.x. if the third entry is -1, then this array only has two entries).
    int attributeSizes[MAX_ATTRIBUTES];
    //Parallel to "attributeSizes". Indicates whether each attribute should be normalized.
    bool attributeNormalized[MAX_ATTRIBUTES];
    //The name of each attribute in the shader code.
    std::string attributeNames[MAX_ATTRIBUTES];
};



//Commonly-used vertex structs. The general order for attributes is as follows:
//Position
//UV
//Normal
//Color
//The structs' attribute ordering is also exposed in their names.
//TODO: For each struct, add static getters like "int GetPosVertexInput()" for every attribute. Then change the various worlds to use them.


struct VertexPos
{
	Vector3f Pos;
	VertexPos(Vector3f pos = Vector3f()) : Pos(pos) { }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, false, "in_pos"); }
};

struct VertexPosColor
{
	Vector3f Pos;
	Vector3f Col;

    VertexPosColor(Vector3f pos = Vector3f(), Vector3f color = Vector3f(1.0f, 1.0f, 1.0f)) : Pos(pos), Col(color) { }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 3, false, false, "in_pos", "in_color"); }
};

struct VertexPosTex1
{
	Vector3f Pos;
	Vector2f TexCoords;
    VertexPosTex1(Vector3f pos = Vector3f(), Vector2f texCoords = Vector2f()) : Pos(pos), TexCoords(texCoords) { }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 2, false, false, "in_pos", "in_uv"); }
};

struct VertexPosTex2
{
	Vector3f Pos;
	Vector2f TexCoords1, TexCoords2;
    VertexPosTex2(Vector3f pos = Vector3f(), Vector2f texCoords1 = Vector2f(), Vector2f texCoords2 = Vector2f()) : Pos(pos), TexCoords1(texCoords1), TexCoords2(texCoords2) { }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 2, 2, false, false, false, "in_pos", "in_uv1", "in_uv2"); }
};

struct VertexPosNormal
{
    Vector3f Pos, Normal;
    VertexPosNormal(Vector3f pos = Vector3f(), Vector3f normal = Vector3f(0.0f, 0.0f, 1.0f)) : Pos(pos), Normal(normal) { }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 3, false, true, "in_pos", "in_normal"); }
};

struct VertexPosTex1Normal
{
	Vector3f Pos;
	Vector2f TexCoords;
	Vector3f Normal;
	VertexPosTex1Normal(Vector3f pos = Vector3f(), Vector2f texCoords = Vector2f(), Vector3f normal = Vector3f(0, 0, 1)) : Pos(pos), TexCoords(texCoords), Normal(normal) { }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 2, 3, false, false, true, "in_pos", "in_uv", "in_normal"); }
};

struct VertexPosTex1NormalColor
{
    Vector3f Pos;
    Vector2f TexCoords;
    Vector3f Normal;
    Vector4f Color;
    VertexPosTex1NormalColor(Vector3f pos = Vector3f(), Vector2f texCoords = Vector2f(), Vector3f normal = Vector3f(0, 0, 1), Vector4f color = Vector4f(0, 0, 0, 1))
        : Pos(pos), TexCoords(texCoords), Normal(normal), Color(color)
    {

    }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 2, 3, 4, false, false, true, false, "in_pos", "in_uv", "in_normal", "in_color"); }
};