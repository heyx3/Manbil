#include "Vertices.h"


ShaderInOutAttributes::ShaderInOutAttributes(int attribSize, bool normalized, std::string name)
{
    attributeSizes[0] = attribSize;
    attributeNormalized[0] = normalized;
    attributeNames[0] = name;
    attributeSizes[1] = -1;
}
ShaderInOutAttributes::ShaderInOutAttributes(int attribSize1, int attribSize2,
                                   bool normalized1, bool normalized2,
                                   std::string name1, std::string name2)
{
    attributeSizes[0] = attribSize1;
    attributeSizes[1] = attribSize2;
    attributeSizes[2] = -1;
    attributeNormalized[0] = normalized1;
    attributeNormalized[1] = normalized2;
    attributeNames[0] = name1;
    attributeNames[1] = name2;
}
ShaderInOutAttributes::ShaderInOutAttributes(int attribSize1, int attribSize2, int attribSize3,
                                   bool normalized1, bool normalized2, bool normalized3,
                                   std::string name1, std::string name2, std::string name3)
{
    attributeSizes[0] = attribSize1;
    attributeSizes[1] = attribSize2;
    attributeSizes[2] = attribSize3;
    attributeSizes[3] = -1;
    attributeNormalized[0] = normalized1;
    attributeNormalized[1] = normalized2;
    attributeNormalized[2] = normalized3;
    attributeNames[0] = name1;
    attributeNames[1] = name2;
    attributeNames[2] = name3;
}
ShaderInOutAttributes::ShaderInOutAttributes(int attribSize1, int attribSize2, int attribSize3, int attribSize4,
                                   bool normalized1, bool normalized2, bool normalized3, bool normalized4,
                                   std::string name1, std::string name2, std::string name3, std::string name4)
{
    attributeSizes[0] = attribSize1;
    attributeSizes[1] = attribSize2;
    attributeSizes[2] = attribSize3;
    attributeSizes[3] = attribSize4;
    attributeSizes[4] = -1;
    attributeNormalized[0] = normalized1;
    attributeNormalized[1] = normalized2;
    attributeNormalized[2] = normalized3;
    attributeNormalized[3] = normalized4;
    attributeNames[0] = name1;
    attributeNames[1] = name2;
    attributeNames[2] = name3;
    attributeNames[3] = name4;
}
ShaderInOutAttributes::ShaderInOutAttributes(int attribSize1, int attribSize2, int attribSize3, int attribSize4, int attribSize5,
                                   bool normalized1, bool normalized2, bool normalized3, bool normalized4, bool normalized5,
                                   std::string name1, std::string name2, std::string name3, std::string name4, std::string name5)
{
    attributeSizes[0] = attribSize1;
    attributeSizes[1] = attribSize2;
    attributeSizes[2] = attribSize3;
    attributeSizes[3] = attribSize4;
    attributeSizes[4] = attribSize5;
    attributeNormalized[0] = normalized1;
    attributeNormalized[1] = normalized2;
    attributeNormalized[2] = normalized3;
    attributeNormalized[3] = normalized4;
    attributeNormalized[4] = normalized5;
    attributeNames[0] = name1;
    attributeNames[1] = name2;
    attributeNames[2] = name3;
    attributeNames[3] = name4;
    attributeNames[4] = name5;
}
ShaderInOutAttributes::ShaderInOutAttributes(const ShaderInOutAttributes & cpy)
{
    for (unsigned int i = 0; i < MAX_ATTRIBUTES; ++i)
    {
        attributeSizes[i] = cpy.attributeSizes[i];
        attributeNormalized[i] = cpy.attributeNormalized[i];
        attributeNames[i] = cpy.attributeNames[i];
    }
}


bool ShaderInOutAttributes::operator==(const ShaderInOutAttributes & other) const
{
    unsigned int numb1 = GetNumbAttributes(),
                 numb2 = other.GetNumbAttributes();

    if (numb1 != numb2) return false;

    for (unsigned int attr = 0; attr < numb1; ++attr)
    {
        if (GetAttributeSize(attr) != other.GetAttributeSize(attr) ||
            GetAttributeNormalized(attr) != other.GetAttributeNormalized(attr) ||
            GetAttributeName(attr).compare(other.GetAttributeName(attr)) != 0)
        {
            return false;
        }
    }

    return true;
}

bool ShaderInOutAttributes::EnableAttributes(void) const
{
    //Enable the attribute slots and count the size of the vertex class.
    GLsizei stride = 0;
    //Get it? "indEX"?
    for (unsigned int indX = 0; indX < MAX_ATTRIBUTES && IsValidAttribute(indX); ++indX)
    {
        glEnableVertexAttribArray(indX);
        stride += attributeSizes[indX];
    }
    stride *= sizeof(float);

    //Set up the attribute values.
    unsigned int pos = 0;
    for (unsigned int indX = 0; indX < MAX_ATTRIBUTES && IsValidAttribute(indX); ++indX)
    {
        glVertexAttribPointer(indX, attributeSizes[indX], GL_FLOAT,
                              (attributeNormalized[indX] ? GL_TRUE : GL_FALSE), stride, (GLvoid*)pos);
        pos += sizeof(float) * attributeSizes[indX];
    }

    return stride > 0;
}
bool ShaderInOutAttributes::DisableAttributes(void) const
{
    unsigned int i = 0;
    for (; i < MAX_ATTRIBUTES && IsValidAttribute(i); ++i)
        glDisableVertexAttribArray(i);
    return i > 0;
}