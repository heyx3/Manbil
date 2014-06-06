#pragma once

#include <memory>

#include "../../Materials/UniformCollections.h"
#include "../../Texture Management/TextureManager.h"
#include "../../Materials/Data Nodes/DataNodeIncludes.h"
#include "../GPUParticleDefines.h"


//"HGP" stands for "High-level Gpu Particle".


//Represents a property of a particle that changes over time.
//The "ComponentSize" generic parameter is the size of the float output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
class ChronologicalHGPComponent
{
public:

    //The exception that is thrown if an instance of this component fails a sanity check.
    static const int EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT = 19285;


    static const ShaderInNode ParticleIDInput = ShaderInNode(2, 0, 0, 0, 0),
                              ParticleRandSeedInput = ShaderInNode(1, 1, 1, 0, 1);
    static const DataLine ParticleUVs = DataLine(DataNodePtr(new FragmentInputNode(ParticleVertex::GetAttributeData())), 2);


    std::string GetError(void) const { return errorMsg; }
    bool HasError(void) const { return !errorMsg.empty(); }


    //Creates the series of DataNodes that effectively creates this component.
    //Has an output size of "ComponentSize".
    virtual DataLine GetComponentOutput(void) const = 0;

    //Initializes any OpenGL data necessary for this component and stores relevant uniform data into the given UniformDictionary.
    virtual void InitializeComponent(TextureManager & manager, UniformDictionary & params) = 0;


protected:

    mutable std::string errorMsg;

    //If the given test is false, sets the error message to the given message and throws EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT.
    void Assert(bool test, std::string errMsg = "UNKNOWN ERROR") const
    {
        if (!test)
        {
            errorMsg = errMsg;
            throw EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT;
        }
    }
    //Asserts that the given vector has the given size.
    void Assert(const VectorF & v, std::string nameOfVector, int size = ComponentSize)
    {
        Assert(v.GetSize() == size, std::string() + "'" + nameOfVector + "' is size " + std::to_string(v.GetSize()) + ", not size " + std::to_string(size) + "!");
    }
};


//The size of the component's output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
//Represents a constant value, independent of time.
class ConstantHGPComponent : public ChronologicalHGPComponent<ComponentSize>
{
public:

    VectorF ConstantValue;

    ConstantHGPComponent(const VectorF & constantValue) : ConstantValue(constantValue) { }

    virtual DataLine GetComponentOutput(void) const override
    {
        Assert(ConstantValue, "ConstantValue");
        return DataLine(ConstantValue);
    }
    virtual void InitializeComponent(TextureManager & mngr, UniformDictionary & params) override { }
};


//The size of the component's output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
//Represents a randomly-chosen constant value (between two values).
class ConstantRangeHGPComponent : public ChronologicalHGPComponent<ComponentSize>
{
public:

    VectorF MinValue, MaxValue;

    ConstantRangeHGPComponent(const VectorF & minValue, const VectorF & maxValue) : MinValue(minValue), MaxValue(maxValue) { }

    virtual DataLine GetComponentOutput(void) const override
    {
        Assert(MinValue, "MinValue");
        Assert(MaxValue, "MaxValue");
        return DataLine(DataNodePtr(new InterpolateNode(DataLine(MinValue), DataLine(MaxValue), ParticleRandSeedInput)), 0);
    }
    virtual void InitializeComponent(TextureManager & mngr, UniformDictionary & params) override { }
};


