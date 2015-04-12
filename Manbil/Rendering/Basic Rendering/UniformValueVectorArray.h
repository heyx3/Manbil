#pragma once

#include "GLVectors.h"
#include "OpenGLIncludes.h"


//Some kind of vector type, like signed-integer or floating-point.
template<typename GLVectorType, typename ComponentType>
//An array of vectors of some type (float, int, etc).
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



//An array of float vectors.
typedef UniformValueVectorArray<VectorF, float> UniformValueArrayF;
//An array of signed integer vectors.
typedef UniformValueVectorArray<VectorI, int> UniformValueArrayI;
