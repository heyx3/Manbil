#include "Vertices.h"


bool VertexAttributes::operator==(const VertexAttributes & other) const
{
    unsigned int numb1 = GetNumbAttributes(),
                 numb2 = other.GetNumbAttributes();

    if (numb1 != numb2) return false;

    for (unsigned int attr = 0; attr < numb1; ++attr)
        if (GetAttributeSize(attr) != other.GetAttributeSize(attr) ||
            GetAttributeNormalized(attr) != other.GetAttributeNormalized(attr))
            return false;

    return true;
}

bool VertexAttributes::EnableAttributes(void) const
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
bool VertexAttributes::DisableAttributes(void) const
{
    unsigned int i = 0;
    for (; i < MAX_ATTRIBUTES && IsValidAttribute(i); ++i)
        glDisableVertexAttribArray(i);
    return i > 0;
}