#include "RenderIOAttributes.h"

#include <assert.h>
#include "../../OpenGLIncludes.h"



unsigned int GetIOSize(const std::vector<RenderIOAttributes::Attribute>& attrs)
{
    unsigned int size = 0;
    for (int i = 0; i < attrs.size(); ++i)
        size += sizeof(float) * attrs[i].Size;
    return size;
}


unsigned int RenderIOAttributes::GetMaxAttributes(void)
{
    int n;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);

    assert(n >= 0);
    return (unsigned int)n;
}

RenderIOAttributes::RenderIOAttributes(Attribute attr)
{
    attributes.insert(attributes.end(), attr);
    ioSize = GetIOSize(attributes);
}
RenderIOAttributes::RenderIOAttributes(Attribute attr1, Attribute attr2)
{
    attributes.reserve(2);
    attributes.insert(attributes.end(), attr1);
    attributes.insert(attributes.end(), attr2);
    ioSize = GetIOSize(attributes);
}
RenderIOAttributes::RenderIOAttributes(Attribute attr1, Attribute attr2, Attribute attr3)
{
    attributes.reserve(3);
    attributes.insert(attributes.end(), attr1);
    attributes.insert(attributes.end(), attr2);
    attributes.insert(attributes.end(), attr3);
    ioSize = GetIOSize(attributes);
}
RenderIOAttributes::RenderIOAttributes(Attribute attr1, Attribute attr2,
                                       Attribute attr3, Attribute attr4)
{
    attributes.reserve(4);
    attributes.insert(attributes.end(), attr1);
    attributes.insert(attributes.end(), attr2);
    attributes.insert(attributes.end(), attr3);
    attributes.insert(attributes.end(), attr4);
    ioSize = GetIOSize(attributes);
}
RenderIOAttributes::RenderIOAttributes(Attribute attr1, Attribute attr2, Attribute attr3,
                                       Attribute attr4, Attribute attr5)
{
    attributes.reserve(5);
    attributes.insert(attributes.end(), attr1);
    attributes.insert(attributes.end(), attr2);
    attributes.insert(attributes.end(), attr3);
    attributes.insert(attributes.end(), attr4);
    attributes.insert(attributes.end(), attr5);
    ioSize = GetIOSize(attributes);
}
RenderIOAttributes::RenderIOAttributes(const std::vector<Attribute>& _attributes)
    : attributes(_attributes)
{
    ioSize = GetIOSize(attributes); 
}

bool RenderIOAttributes::operator==(const RenderIOAttributes& other) const
{
    if (attributes.size() != other.attributes.size())
    {
        return false;
    }

    for (int i = 0; i < attributes.size(); ++i)
    {
        if (attributes[i].Size != other.attributes[i].Size ||
            attributes[i].IsNormalized != other.attributes[i].IsNormalized ||
            attributes[i].Name != other.attributes[i].Name)
        {
            return false;
        }
    }

    return true;
}

void RenderIOAttributes::SetAttributes(const std::vector<Attribute>& newAttributes)
{
    ioSize = GetIOSize(newAttributes);
    attributes = newAttributes;
}

void RenderIOAttributes::EnableAttributes(void) const
{
    //Enable the attribute slots and count the size of the vertex class.
    GLsizei stride = 0;
    //Get it? "indEX"?
    for (unsigned int indX = 0; indX < attributes.size(); ++indX)
    {
        glEnableVertexAttribArray(indX);
        stride += attributes[indX].Size;
    }
    stride *= sizeof(float);

    //Set up the attribute values.
    unsigned int pos = 0;
    for (unsigned int indX = 0; indX < attributes.size(); ++indX)
    {
        glVertexAttribPointer(indX, attributes[indX].Size, GL_FLOAT,
                              (attributes[indX].IsNormalized ? GL_TRUE : GL_FALSE),
                              stride, (GLvoid*)pos);
        pos += sizeof(float) * attributes[indX].Size;
    }
}
void RenderIOAttributes::DisableAttributes(void) const
{
    for (unsigned int i = 0; i < attributes.size(); ++i)
        glDisableVertexAttribArray(i);
}