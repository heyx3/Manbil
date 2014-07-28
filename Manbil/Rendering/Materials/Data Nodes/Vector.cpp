#include "Vector.h"

#include <assert.h>


std::string VectorF::GetGLSLType(void) const
{
    switch (size)
    {
        case 1: return "float";
        case 2: return "vec2";
        case 3: return "vec3";
        case 4: return "vec4";
        default:
            assert(false);
            return "invalid_type_name";
    }
}

VectorF VectorF::operator+(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] + other.values[i];
    return retVal;
}
VectorF VectorF::operator-(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] - other.values[i];
    return retVal;
}
VectorF VectorF::operator*(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] * other.values[i];
    return retVal;
}
VectorF VectorF::operator/(const VectorF & other) const
{
    assert(size == other.size);

    VectorF retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] / other.values[i];
    return retVal;
}

bool VectorF::operator==(const VectorF& other) const
{
    if (size != other.size)
        return false;

    for (unsigned int i = 0; i < size; ++i)
        if (values[i] != other.values[i])
            return false;

    return true;
}
bool VectorF::operator!=(const VectorF& other) const
{
    if (size == other.size)
        return false;

    for (unsigned int i = 0; i < size; ++i)
        if (values[i] == other.values[i])
            return false;

    return true;
}

bool VectorF::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteUInt(size, "size", outError))
    {
        outError = "Error writing size of vector (" + std::to_string(size) + "): " + outError;
        return false;
    }

    for (unsigned int el = 0; el < size; ++el)
    {
        std::string name = (el == 0) ? "x" : ((el == 1) ? "y" : ((el == 2) ? "z" : "w"));
        if (!writer->WriteFloat(values[el], name, outError))
        {
            outError = "Error writing " + name + " value '" + std::to_string(values[el]) + "': " + outError;
            return false;
        }
    }

    return true;
}
bool VectorF::ReadData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> trySize = reader->ReadUInt(outError);
    if (!trySize.HasValue())
    {
        outError = "Error reading size of VectorF: " + outError;
        return false;
    }

    size = trySize.GetValue();
    for (unsigned int el = 0; el < size; ++el)
    {
        MaybeValue<float> tryValue = reader->ReadFloat(outError);
        if (!tryValue.HasValue())
        {
            outError = "Error reading " + std::string() + 
                ((el == 0) ? "x" : ((el == 1) ? "y" : ((el == 2) ? "z" : "w"))) + "value: " + outError;
            return false;
        }

        values[el] = tryValue.GetValue();
    }

    return true;
}



std::string VectorI::GetGLSLType(void) const
{
    switch (size)
    {
        case 1: return "int";
        case 2: return "ivec2";
        case 3: return "ivec3";
        case 4: return "ivec4";
        default:
            assert(false);
            return "invalid_type_name";
    }
}

VectorI VectorI::operator+(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] + other.values[i];
    return retVal;
}
VectorI VectorI::operator-(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] - other.values[i];
    return retVal;
}
VectorI VectorI::operator*(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] * other.values[i];
    return retVal;
}
VectorI VectorI::operator/(const VectorI & other) const
{
    assert(size == other.size);

    VectorI retVal(size);
    for (unsigned int i = 0; i < size; ++i)
        retVal.values[i] = values[i] / other.values[i];
    return retVal;
}

bool VectorI::operator==(const VectorI& other) const
{
    if (size != other.size)
        return false;

    for (unsigned int i = 0; i < size; ++i)
        if (values[i] != other.values[i])
            return false;

    return true;
}
bool VectorI::operator!=(const VectorI& other) const
{
    if (size == other.size)
        return false;

    for (unsigned int i = 0; i < size; ++i)
        if (values[i] == other.values[i])
            return false;

    return true;
}

bool VectorI::WriteData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteUInt(size, "size", outError))
    {
        outError = "Error writing size of vector (" + std::to_string(size) + "): " + outError;
        return false;
    }

    for (unsigned int el = 0; el < size; ++el)
    {
        std::string name = (el == 0) ? "x" : ((el == 1) ? "y" : ((el == 2) ? "z" : "w"));
        if (!writer->WriteInt(values[el], name, outError))
        {
            outError = "Error writing " + name + " value '" + std::to_string(values[el]) + "': " + outError;
            return false;
        }
    }

    return true;
}
bool VectorI::ReadData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> trySize = reader->ReadUInt(outError);
    if (!trySize.HasValue())
    {
        outError = "Error reading size of VectorI: " + outError;
        return false;
    }

    size = trySize.GetValue();
    for (unsigned int el = 0; el < size; ++el)
    {
        MaybeValue<int> tryValue = reader->ReadInt(outError);
        if (!tryValue.HasValue())
        {
            outError = "Error reading " + std::string() +
                ((el == 0) ? "x" : ((el == 1) ? "y" : ((el == 2) ? "z" : "w"))) + "value: " + outError;
            return false;
        }

        values[el] = tryValue.GetValue();
    }

    return true;
}