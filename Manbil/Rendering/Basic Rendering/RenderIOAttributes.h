#pragma once

#include <vector>


//Represents information about a set of inputs or outputs to a shader.
//Examples of this are vertex data, vertex shader inputs/outputs, and
//    fragment shader inputs/outputs.
//Each input (or "attribute") can be named for convenience; the name should be a valid variable name.
//These attributes represent floats/vectors of various sizes.
//For example, a certain mesh may have vertices with three attributes: position, UV, and normal.
//The position and normal are size 3 (x, y, and z), while the UV is size 2 (x and y).
//It is assumed that the attributes are ordered in the same way they were given to this class.
//TODO: Add the ability for integer vertex attributes (needed for skeletal meshes). Either: 1) give each attribute some kind of "Type" enum, or 2) list float attributes and int attributes separately.
class RenderIOAttributes
{
public:
    
    struct Attribute
    {
    public:
        unsigned int Size;
        bool IsNormalized;
        std::string Name;
        Attribute(unsigned int size, bool isNormalized, std::string name)
            : Size(size), IsNormalized(isNormalized), Name(name) { }
        Attribute(void) { }
    };


    //Gets the maximum-allowable number of vertex attributes according to OpenGL.
    static unsigned int GetMaxAttributes(void);


    RenderIOAttributes(void) { }
    RenderIOAttributes(Attribute attr);
    RenderIOAttributes(Attribute attr1, Attribute attr2);
    RenderIOAttributes(Attribute attr1, Attribute attr2, Attribute attr3);
    RenderIOAttributes(Attribute attr1, Attribute attr2, Attribute attr3, Attribute attr4);
    RenderIOAttributes(Attribute attr1, Attribute attr2, Attribute attr3,
                       Attribute attr4, Attribute attr5);
    RenderIOAttributes(const std::vector<Attribute>& _attributes);


    bool operator==(const RenderIOAttributes& other) const;
    bool operator!=(const RenderIOAttributes& other) const { return !operator==(other); }


    const std::vector<Attribute>& GetAttributes(void) const { return attributes; }

    void SetAttributes(const std::vector<Attribute>& newAttributes);
    
    unsigned int GetNumbAttributes(void) const { return attributes.size(); }
    const Attribute& GetAttribute(unsigned int index) const { return attributes[index]; }

    //Gets the total cumulative size of all the attributes together, in bytes.
    unsigned int GetTotalIOSize(void) const { return ioSize; }

    //Sets up OpenGL to render a set of vertices with these exact attributes.
    //Must be called AFTER binding the vertex buffer that has these attributes!
    void EnableAttributes(void) const;
    //Cleans up after rendering the type of vertices represented by this instance.
    void DisableAttributes(void) const;


private:

    std::vector<Attribute> attributes;
    unsigned int ioSize;
};