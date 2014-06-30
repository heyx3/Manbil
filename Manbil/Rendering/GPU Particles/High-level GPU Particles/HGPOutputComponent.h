#pragma once

#include <memory>

#include "../../../Math/Higher Math/Gradient.h"
#include "../../Materials/UniformCollections.h"
#include "../../Texture Management/TextureManager.h"
#include "../../Materials/Data Nodes/DataNodeIncludes.h"
#include "../GPUParticleDefines.h"
#include "../../Texture Management/TextureConverters.h"


class HGPComponentManager;

//TODO: Pull stuff out into the .cpp file. You can apparently do this even with templates?



//"HGP" stands for "High-level Gpu Particle".
//An HGP system has a bunch of different "components" for position, color, size, and rotation.
//The base class for HGP components is a template, so it can't store global information correctly.
//Instead, store it in this namespace.
namespace HGPGlobalData
{
    //Each component has a unique ID, just like with DataNodes.
    //This value cannot be static because HGPOutputComponent is a template, not a class.
    extern unsigned int NextHGPComponentID;

    //The exception that is thrown if an instance of this component fails a sanity check.
    extern const int EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT;

    extern const DataLine ParticleIDInput, ParticleRandSeedInputs1, ParticleRandSeedInputs2, ParticleUVs;
    extern const DataNodePtr ParticleRandSeedComponents1, ParticleRandSeedComponents2;

    //Assuming the given value is between 0 and 5, gets the corresponding particle rand seed input.
    extern DataLine GetRandSeed(unsigned int randSeedIndex);
    
    extern const std::string ParticleElapsedTimeUniformName;
    extern const DataLine ParticleElapsedTime;

    extern TextureManager & GetTexManager(HGPComponentManager & manager);
    extern UniformDictionary & GetParams(HGPComponentManager & manager);
    extern const DataLine & GetTimeLerp(HGPComponentManager & manager);
}


//Quality settings for a texture used in HGP particles.
struct HGPTextureQuality
{
public:

    TextureSettings::TextureFiltering FilterQuality;
    unsigned int Width;
    HGPTextureQuality(TextureSettings::TextureFiltering filterQuality, unsigned int width)
        : FilterQuality(filterQuality), Width(width)
    {

    }
};



#define HGPComponentPtr(ComponentSize) std::shared_ptr<HGPOutputComponent<ComponentSize>>
#define HGPConstComponentPtr(ComponentSize) std::shared_ptr<const HGPOutputComponent<ComponentSize>>


//The size of the component's float output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
//Represents some property of a particle.
class HGPOutputComponent
{
public:

    HGPComponentManager & Manager;
    TextureManager & GetTexManager(void) const { return HGPGlobalData::GetTexManager(Manager); }
    UniformDictionary & GetParams(void) const { return HGPGlobalData::GetParams(Manager); }


    HGPOutputComponent(HGPComponentManager & manager)
        : Manager(manager)
    {
        id = HGPGlobalData::NextHGPComponentID;
        HGPGlobalData::NextHGPComponentID += 1;
    }
    HGPOutputComponent(const HGPOutputComponent & cpy); //Intentionally not implemented.


    //Gets whether this component represents a constant value that doesn't change at run-time.
    //Default behavior: return false.
    virtual bool IsConstant(void) const { return false; }


    void AddParent(HGPOutputComponent<1>* parent)
    {
        if (std::find(parents1.begin(), parents1.end(), parent) == parents1.end())
            parents1.insert(parents1.end(), parent);
    }
    void AddParent(HGPOutputComponent<2>* parent)
    {
        if (std::find(parents2.begin(), parents2.end(), parent) == parents2.end())
            parents2.insert(parents2.end(), parent);
    }
    void AddParent(HGPOutputComponent<3>* parent)
    {
        if (std::find(parents3.begin(), parents3.end(), parent) == parents3.end())
            parents3.insert(parents3.end(), parent);
    }
    void AddParent(HGPOutputComponent<4>* parent)
    {
        if (std::find(parents4.begin(), parents4.end(), parent) == parents4.end())
            parents4.insert(parents4.end(), parent);
    }
    void RemoveParent(HGPOutputComponent<1>* parent)
    {
        parents1.erase(std::find(parents1.begin(), parents1.end(), parent));
    }
    void RemoveParent(HGPOutputComponent<2>* parent)
    {
        parents2.erase(std::find(parents2.begin(), parents2.end(), parent));
    }
    void RemoveParent(HGPOutputComponent<3>* parent)
    {
        parents3.erase(std::find(parents3.begin(), parents3.end(), parent));
    }
    void RemoveParent(HGPOutputComponent<4>* parent)
    {
        parents4.erase(std::find(parents4.begin(), parents4.end(), parent));
    }


    //Gets the tree of DataNodes that effectively creates this component's behavior.
    //Has an output size of "ComponentSize".
    DataLine GetComponentOutput(void) const
    {
        if (!createdComponentYet)
        {
            createdComponentYet = true;
            UpdateComponentOutput();
        }
        return componentOutput;
    }

    unsigned int GetUniqueID(void) const { return id; }
    std::string GetUniqueIDStr(void) const { return std::to_string(id); }

    std::string GetError(void) const { return errorMsg; }
    bool HasError(void) const { return !errorMsg.empty(); }


    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    virtual void SwapOutSubComponent(HGPComponentPtr(1) oldC, HGPComponentPtr(1) newC) { }
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    virtual void SwapOutSubComponent(HGPComponentPtr(2) oldC, HGPComponentPtr(2) newC) { }
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    virtual void SwapOutSubComponent(HGPComponentPtr(3) oldC, HGPComponentPtr(3) newC) { }
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    virtual void SwapOutSubComponent(HGPComponentPtr(4) oldC, HGPComponentPtr(4) newC) { }


    //Initializes any OpenGL data necessary for this component and stores relevant uniform data into the given UniformDictionary.
    virtual void InitializeComponent(void) { }
    //Updates any param values necessary for this component and updates relevant uniform data in the given UniformDictionary.
    virtual void UpdateComponent(void) { }

    //Updates this component's DataLine output.
    //Must be called when a property of this component that impacts the output DataLine is changed.
    void UpdateComponentOutput(void) const
    {
        componentOutput = GenerateComponentOutput();

        for (unsigned int i = 0; i < parents1.size(); ++i)
            parents1[i]->UpdateComponentOutput();
        for (unsigned int i = 0; i < parents2.size(); ++i)
            parents2[i]->UpdateComponentOutput();
        for (unsigned int i = 0; i < parents3.size(); ++i)
            parents3[i]->UpdateComponentOutput();
        for (unsigned int i = 0; i < parents4.size(); ++i)
            parents4[i]->UpdateComponentOutput();
    }


protected:

    mutable std::string errorMsg;


    //Generates the output for this component's behavior.
    virtual DataLine GenerateComponentOutput(void) const = 0;


    //If the given test is false, sets the error message to the given message and throws EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT.
    void Assert(std::string errMsg = "UNKNOWN ERROR", bool test = false) const
    {
        if (!test)
        {
            errorMsg = errMsg;
            throw HGPGlobalData::EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT;
        }
    }
    //Asserts that the given vector has the given size.
    void Assert(const VectorF & v, std::string nameOfVector, unsigned int size = ComponentSize) const
    {
        Assert(std::string() + "'" + nameOfVector + "' is size " + std::to_string(v.GetSize()) +
                   ", not size " + std::to_string(size) + "!",
               v.GetSize() == size);
    }


private:

    mutable DataLine componentOutput;
    unsigned int id;

    mutable bool createdComponentYet = false;

    std::vector<HGPOutputComponent<1>*> parents1;
    std::vector<HGPOutputComponent<2>*> parents2;
    std::vector<HGPOutputComponent<3>*> parents3;
    std::vector<HGPOutputComponent<4>*> parents4;
};



//The size of the component's output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
//Represents a constant value, independent of time.
class ConstantHGPComponent : public HGPOutputComponent<ComponentSize>
{
public:

    VectorF GetConstantValue(void) const { return constValue; }
    void SetConstantValue(VectorF newVal) const { constValue = newVal; UpdateComponentOutput(); }

    ConstantHGPComponent(const VectorF & constantValue, HGPComponentManager & manager) : HGPOutputComponent(manager), constValue(constantValue) { }

    virtual bool IsConstant(void) const override { return true; }


protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        Assert(constValue, "ConstantValue", ComponentSize);
        return DataLine(constValue);
    }

private:

    VectorF constValue;
};



//The size of the component's output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
//Represents a gradient value over time.
class GradientHGPComponent : public HGPOutputComponent<ComponentSize>
{
public:

    const Gradient<ComponentSize> & GetGradientValue(void) const { return gradientValue; }
    const HGPTextureQuality & GetGradientTextureQuality(void) const { return gradientTexQuality; }
    void SetGradientValue(const Gradient<ComponentSize> & newGradient) const { gradientValue = newGradient; UpdateComponentOutput(); }
    void SetGradientTextureQuality(const HGPTextureQuality & newTexQuality) const { gradientTexQuality = newTexQuality; UpdateComponentOutput(); }

    std::string GetSamplerName(void) const { return std::string("u_gradientSampler") + GetUniqueIDStr(); }
    

    GradientHGPComponent(const Gradient<ComponentSize> & gradientVal, const HGPTextureQuality & gradientTextureQuality, HGPComponentManager & manager)
        : HGPOutputComponent(manager), gradientValue(gradientVal), gradientTexQuality(gradientTextureQuality)
    {

    }


    virtual void InitializeComponent(void) override
    {
        //Create the texture.
        lookupTexID = GetTexManager().CreateTexture();
        Assert(std::string() + "Texture creation failed. Texture width: " + std::to_string(gradientTexQuality.Width),
               lookupTexID != TextureManager::UNUSED_ID);

        //Generate the texture data.
        Array2D<VectorF> texOut(gradientTexQuality.Width, 1);
        GenerateTextureData(texOut.GetArray());

        //Convert the texture data from an array to a texture.
        Array2D<Vector4f> texColor4f(0, 0);
        switch (ComponentSize)
        {
            case 1:
                texColor4f.Fill([&texOut](Vector2i loc, Vector4f * outVal)
                {
                    *outVal = Vector4f(texOut[loc].GetValue()[0], 0.0f, 0.0f, 0.0f);
                });
                break;
            case 2:
                texColor4f.Fill([&texOut](Vector2i loc, Vector4f * outVal)
                {
                    const float * values = texOut[loc].GetValue();
                    *outVal = Vector4f(values[0], values[1], 0.0f, 0.0f);
                });
                break;
            case 3:
                texColor4f.Fill([&texOut](Vector2i loc, Vector4f * outVal)
                {
                    const float * values = texOut[loc].GetValue();
                    *outVal = Vector4f(values[0], values[1], values[2], 0.0f);
                });
                break; 
            case 4:
                texColor4f.Fill([&texOut](Vector2i loc, Vector4f * outVal)
                {
                    const float * values = texOut[loc].GetValue();
                    *outVal = Vector4f(values[0], values[1], values[2], values[3]);
                });
                break;

            default: assert(false);
        }
        ManbilTexture1 * tex = GetTexManager()[lookupTexID];
        Assert("Uh-oh: texture was not found in the manager immediately after succesfully creating it!", tex != 0);
        tex->SetData(texColor4f);

        //Set the texture quality.
        tex->SetFiltering(gradientTexQuality.FilterQuality);
        tex->SetWrapping(TextureSettings::TextureWrapping::TW_CLAMP);

        //Set the texture data.
        Params.TextureUniforms[GetSamplerName()].Texture = tex;

    }
    virtual void UpdateComponent(void) override
    {
        Params.TextureUniforms[GetSamplerName()].Texture = Manager[lookupTexID];
    }

protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        DataLine uvLookup(DataNodePtr(new CombineVectorNode(HGPGlobalData::GetTimeLerp(Manager), DataLine(VectorF(0.5f)))), 0);
        DataLine textureSample(DataNodePtr(new TextureSampleNode(uvLookup, GetSamplerName())), TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllChannels));

        switch (ComponentSize)
        {
            case 1: return DataLine(DataNodePtr(new SwizzleNode(textureSample, SwizzleNode::Components::C_X)), 0);
            case 2: return DataLine(DataNodePtr(new SwizzleNode(textureSample, SwizzleNode::Components::C_X, SwizzleNode::Components::C_Y)), 0);
            case 3: return DataLine(DataNodePtr(new SwizzleNode(textureSample, SwizzleNode::Components::C_X, SwizzleNode::Components::C_Y, SwizzleNode::Components::C_Z)), 0);
            case 4: return DataLine(DataNodePtr(new SwizzleNode(textureSample, SwizzleNode::Components::C_X, SwizzleNode::Components::C_Y, SwizzleNode::Components::C_Z, SwizzleNode::Components::C_W)), 0);
            default:
                Assert(std::string() + "Invalid ComponentSize value of " + std::to_string(ComponentSize) + "; must be between 1-4 inclusive!");
                return DataLine(DataNodePtr(), 666);
        }
    }

private:

    //Given a float array of size "LookupTextureWidth", fills it with the values of the gradient.
    void GenerateTextureData(VectorF * values) const
    {
        const float toTValue = 1.0f / (float)(LookupTextureWidth - 1);
        for (unsigned int i = 0; i < LookupTextureWidth; ++i)
            gradientValue.GetValue(i * toTValue, values[i].GetValue());
    }

    Gradient<ComponentSize> gradientValue;
    HGPTextureQuality gradientTexQuality;

    unsigned int lookupTexID;
};


//The size of the component's output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
//Represents a value that is just the output of a DataLine.
class ExpressionHGPComponent : public HGPOutputComponent<ComponentSize>
{
public:

    DataLine GetExpression(void) const { return expression; }
    void SetExpression(DataLine newExpr)
    {
        Assert("The new expression has a size of " + std::to_string(newExpr.GetDataLineSize()) + " instead of the required size of " + std::to_string(ComponentSize),
               newExpr.GetDataLineSize() == ComponentSize);
        expression = newExpr;
        UpdateComponentOutput();
    }

    ExpressionHGPComponent(HGPComponentManager & manager, DataLine _expression)
        : HGPOutputComponent(manager), expression(_expression)
    {
        Assert("The expression has a size of " + std::to_string(expression.GetDataLineSize()) + " instead of the required size of " + std::to_string(ComponentSize),
               expression.GetDataLineSize() == ComponentSize);
    }


    virtual bool IsConstant(void) const override { return expression.IsConstant(); }


protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        return expression;
    }


private:

    DataLine expression;
};



//The size of the component's output (float, vec2, vec3, or vec4).
template<unsigned int ComponentSize>
//Represents a value that is a certain randomized interpolation between two given values.
class RandomizedHGPComponent : public HGPOutputComponent<ComponentSize>
{
public:

    HGPComponentPtr(ComponentSize) GetMin(void) const { return min; }
    HGPComponentPtr(ComponentSize) GetMax(void) const { return max; }
    void SetMin(HGPComponentPtr(ComponentSize) newMin)
    {
        min->RemoveParent(this);
        min = newMin;
        min->AddParent(this);
        UpdateComponentOutput();
    }
    void SetMax(HGPComponentPtr(ComponentSize) newMax)
    {
        max->RemoveParent(this);
        max = newMax;
        max->AddParent(this);
        UpdateComponentOutput();
    }


    unsigned int GetRandSeedIndex(void) const { return randSeedIndex; }
    void SetRandSeedIndex(unsigned int newVal) const { randSeedIndex = newVal; UpdateComponentOutput(); }


    RandomizedHGPComponent(HGPComponentManager & manager,
                           HGPComponentPtr(ComponentSize) _min, HGPComponentPtr(ComponentSize) _max, const unsigned int _randSeedIndex[ComponentSize])
        : HGPOutputComponent(manager), min(_min), max(_max)
    {
        for (unsigned int i = 0; i < ComponentSize; ++i)
            randSeedIndex[i] = _randSeedIndex[i];

        min->AddParent(this);
        max->AddParent(this);
    }


    virtual bool IsConstant(void) const override { return min->IsConstant() && max->IsConstant(); }


    virtual void InitializeComponent(void) override
    {
        min->InitializeComponent();
        max->InitializeComponent();
    }
    virtual void UpdateComponent(void) override
    {
        min->UpdateComponent();
        max->UpdateComponent();
    }


    //The "SwapOutSubComponent" functions are a bit of a thorny problem, because we don't know the size of ComponentSize here.
    //My solution was to define dummy "SetMin/SetMax" functions that in reality will never be called.

private:
    void SetMin(HGPComponentPtr(ComponentSize == 1 ? 2 : (ComponentSize == 2 ? 3 : (ComponentSize == 3 ? 4 : 1))) newMin)
    {
        Assert(std::string() + "ComponentSize is " + std::to_string(ComponentSize) + ", but the min was set to one of unwrapped size " + std::to_string(ComponentSize + 1));
    }
    void SetMin(HGPComponentPtr(ComponentSize == 1 ? 3 : (ComponentSize == 2 ? 4 : (ComponentSize == 3 ? 1 : 2))) newMin)
    {
        Assert(std::string() + "ComponentSize is " + std::to_string(ComponentSize) + ", but the min was set to one of unwrapped size " + std::to_string(ComponentSize + 2));
    }
    void SetMin(HGPComponentPtr(ComponentSize == 1 ? 4 : (ComponentSize == 2 ? 1 : (ComponentSize == 3 ? 2 : 3))) newMin)
    {
        Assert(std::string() + "ComponentSize is " + std::to_string(ComponentSize) + ", but the min was set to one of unwrapped size " + std::to_string(ComponentSize + 3));
    }
    void SetMax(HGPComponentPtr(ComponentSize == 1 ? 2 : (ComponentSize == 2 ? 3 : (ComponentSize == 3 ? 4 : 1))) newMin)
    {
        Assert(std::string() + "ComponentSize is " + std::to_string(ComponentSize) + ", but the max was set to one of unwrapped size " + std::to_string(ComponentSize + 1));
    }
    void SetMax(HGPComponentPtr(ComponentSize == 1 ? 3 : (ComponentSize == 2 ? 4 : (ComponentSize == 3 ? 1 : 2))) newMin)
    {
        Assert(std::string() + "ComponentSize is " + std::to_string(ComponentSize) + ", but the max was set to one of unwrapped size " + std::to_string(ComponentSize + 2));
    }
    void SetMax(HGPComponentPtr(ComponentSize == 1 ? 4 : (ComponentSize == 2 ? 1 : (ComponentSize == 3 ? 2 : 3))) newMin)
    {
        Assert(std::string() + "ComponentSize is " + std::to_string(ComponentSize) + ", but the max was set to one of unwrapped size " + std::to_string(ComponentSize + 3));
    }
public:

    virtual void SwapOutSubComponent(HGPComponentPtr(1) oldC, HGPComponentPtr(1) newC) override
    {
        if ((void*)oldC.get() == (void*)min.get())
            SetMin(newC);
        else min->SwapOutSubComponent(oldC, newC);

        if ((void*)oldC.get() == (void*)max.get())
            SetMax(newC);
        else max->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(2) oldC, HGPComponentPtr(2) newC) override
    {
        if ((void*)oldC.get() == (void*)min.get())
            SetMin(newC);
        else min->SwapOutSubComponent(oldC, newC);

        if ((void*)oldC.get() == (void*)max.get())
            SetMax(newC);
        else max->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(3) oldC, HGPComponentPtr(3) newC) override
    {
        if ((void*)oldC.get() == (void*)min.get())
            SetMin(newC);
        else min->SwapOutSubComponent(oldC, newC);

        if ((void*)oldC.get() == (void*)max.get())
            SetMax(newC);
        else max->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(4) oldC, HGPComponentPtr(4) newC) override
    {
        if ((void*)oldC.get() == (void*)min.get())
            SetMin(newC);
        else min->SwapOutSubComponent(oldC, newC);

        if ((void*)oldC.get() == (void*)max.get())
            SetMax(newC);
        else max->SwapOutSubComponent(oldC, newC);
    }

protected:
    
    virtual DataLine GenerateComponentOutput(void) const override
    {
        std::vector<DataLine> randSeeds;
        for (unsigned int i = 0; i < ComponentSize; ++i)
            randSeeds.insert(randSeeds.end(), HGPGlobalData::GetRandSeed(randSeedIndex[i]));

        DataLine finalSeed(DataNodePtr(new CombineVectorNode(randSeeds)), 0);

        return DataLine(DataNodePtr(new InterpolateNode(min->GetComponentOutput(), max->GetComponentOutput(), finalSeed, InterpolateNode::InterpolationType::IT_Linear)), 0);
    }

private:

    unsigned int randSeedIndex[ComponentSize];
    HGPComponentPtr(ComponentSize) min, max;
};