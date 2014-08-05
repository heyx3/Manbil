#pragma once

#include "HGPComponentManager.h"


//TODO: Move into .cpp.


//Outputs the world position given a constant acceleration and initial velocity/position.
class ConstantAccelerationHGPComponent : public HGPOutputComponent<3>
{
public:

    HGPComponentPtr(3) GetAcceleration(void) const { return acceleration; }
    void SetAcceleration(HGPComponentPtr(3) newAccel)
    {
        acceleration->RemoveParent(this);
        acceleration = newAccel;
        acceleration->AddParent(this);
        UpdateComponentOutput();
    }
    HGPComponentPtr(3) GetInitialVelocity(void) const { return initialVelocity; }
    void SetInitialVelocity(HGPComponentPtr(3) newInitialVel)
    {
        initialVelocity->RemoveParent(this);
        initialVelocity = newInitialVel;
        initialVelocity->AddParent(this);
        UpdateComponentOutput();
    }
    HGPComponentPtr(3) GetInitialPosition(void) const { return initialPosition; }
    void SetInitialPosition(HGPComponentPtr(3) newInitialPos)
    {
        initialPosition->RemoveParent(this);
        initialPosition = newInitialPos;
        initialPosition->AddParent(this);
        UpdateComponentOutput();
    }


    ConstantAccelerationHGPComponent(HGPComponentManager & manager,
                                     HGPComponentPtr(3) accel, HGPComponentPtr(3) initialVel, HGPComponentPtr(3) initialPos)
        : acceleration(accel), initialVelocity(initialVel), initialPosition(initialPos), HGPOutputComponent(manager)
    {
        acceleration->AddParent(this);
        initialVelocity->AddParent(this);
        initialPosition->AddParent(this);
    }


    virtual void SwapOutSubComponent(HGPComponentPtr(1) oldC, HGPComponentPtr(1) newC) override
    {
        acceleration->SwapOutSubComponent(oldC, newC);
        initialVelocity->SwapOutSubComponent(oldC, newC);
        initialPosition->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(2) oldC, HGPComponentPtr(2) newC) override
    {
        acceleration->SwapOutSubComponent(oldC, newC);
        initialVelocity->SwapOutSubComponent(oldC, newC);
        initialPosition->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(3) oldC, HGPComponentPtr(3) newC) override
    {
        if (oldC.get() == acceleration.get())
            SetAcceleration(newC);
        else acceleration->SwapOutSubComponent(oldC, newC);

        if (oldC.get() == initialVelocity.get())
            SetInitialVelocity(newC);
        else initialVelocity->SwapOutSubComponent(oldC, newC);

        if (oldC.get() == initialPosition.get())
            SetInitialPosition(newC);
        else initialPosition->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(4) oldC, HGPComponentPtr(4) newC) override
    {
        acceleration->SwapOutSubComponent(oldC, newC);
        initialVelocity->SwapOutSubComponent(oldC, newC);
        initialPosition->SwapOutSubComponent(oldC, newC);
    }


protected:

    virtual DataLine GenerateComponentOutput(void) const override;

    virtual void InitializeComponent(void) override
    {
        acceleration->InitializeComponent();
        initialVelocity->InitializeComponent();
        initialPosition->InitializeComponent();
    }
    virtual void UpdateComponent(void) override
    {
        acceleration->UpdateComponent();
        initialVelocity->UpdateComponent();
        initialPosition->UpdateComponent();
    }

private:

    mutable DataNode::Ptr velocityMult, accelMult, addResult;
    HGPComponentPtr(3) acceleration, initialVelocity, initialPosition;
};


//Outputs the world position given a constant velocity and initial position.
class ConstantVelocityComponent : public HGPOutputComponent<3>
{
public:

    HGPComponentPtr(3) GetInitialPos(void) const { return initialPos; }
    void SetInitialPos(HGPComponentPtr(3) newInitialPos)
    {
        initialPos->RemoveParent(this);
        initialPos = newInitialPos;
        initialPos->AddParent(this);
        UpdateComponentOutput();
    }
    HGPComponentPtr(3) GetVelocity(void) const { return velocity; }
    void SetVelocity(HGPComponentPtr(3) newVelocity)
    {
        velocity->RemoveParent(this);
        velocity = newVelocity;
        velocity->AddParent(this);
        UpdateComponentOutput();
    }


    ConstantVelocityComponent(HGPComponentManager & manager, HGPComponentPtr(3) initialPosition, HGPComponentPtr(3) _velocity)
        : HGPOutputComponent(manager), initialPos(initialPosition), velocity(_velocity)
    {
        initialPos->AddParent(this);
        velocity->AddParent(this);
    }


    virtual void SwapOutSubComponent(HGPComponentPtr(1) oldC, HGPComponentPtr(1) newC) override
    {
        initialPos->SwapOutSubComponent(oldC, newC);
        velocity->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(2) oldC, HGPComponentPtr(2) newC) override
    {
        initialPos->SwapOutSubComponent(oldC, newC);
        velocity->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(3) oldC, HGPComponentPtr(3) newC) override
    {
        if (initialPos.get() == oldC.get())
            SetInitialPos(newC);
        else initialPos->SwapOutSubComponent(oldC, newC);

        if (velocity.get() == oldC.get())
            SetVelocity(newC);
        else velocity->SwapOutSubComponent(oldC, newC);
    }
    virtual void SwapOutSubComponent(HGPComponentPtr(4) oldC, HGPComponentPtr(4) newC) override
    {
        initialPos->SwapOutSubComponent(oldC, newC);
        velocity->SwapOutSubComponent(oldC, newC);
    }

    virtual void InitializeComponent(void) override
    {
        velocity->InitializeComponent();
        initialPos->InitializeComponent();
    }
    virtual void UpdateComponent(void) override
    {
        velocity->UpdateComponent();
        initialPos->UpdateComponent();
    }

protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        multTime = DataNode::Ptr(new MultiplyNode(HGPGlobalData::ParticleElapsedTime,
                                                  velocity->GetComponentOutput()));
        addMultTime = DataNode::Ptr(new AddNode(initialPos->GetComponentOutput(),
                                                DataLine(addMultTime->GetName())));
        return DataLine(addMultTime->GetName());
    }


private:

    mutable DataNode::Ptr multTime, addMultTime;
    HGPComponentPtr(3) initialPos, velocity;
};



//Outputs a random world position inside a sphere.
class SpherePositionComponent : public HGPOutputComponent<3>
{
public:

    Vector3f GetSphereCenter(void) const { return sphereCenter; }
    void SetSphereCenter(Vector3f newCenter) { sphereCenter = newCenter; UpdateComponentOutput(); }
    float GetSphereRadius(void) const { return sphereRadius; }
    void SetSphereRadius(float newRadius) { sphereRadius = newRadius; UpdateComponentOutput(); }

    //Gets an array of the four rand seed indexes this SpherePositionComponent uses.
    const unsigned int * GetRandSeeds(void) const { return randSeeds; }
    void SetRandSeeds(const unsigned int newSeeds[4]) { for (unsigned int i = 0; i < 4; ++i) randSeeds[i] = newSeeds[i]; UpdateComponentOutput(); }

    
    SpherePositionComponent(HGPComponentManager & manager, Vector3f _sphereCenter, float _sphereRadius, const unsigned int _randSeeds[4])
        : HGPOutputComponent(manager), sphereCenter(_sphereCenter), sphereRadius(_sphereRadius)
    {
        for (unsigned int i = 0; i < 4; ++i)
            randSeeds[i] = _randSeeds[i];
    }


protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        combineSeeds = DataNode::Ptr(new CombineVectorNode(HGPGlobalData::GetRandSeed(randSeeds[0]),
                                                           HGPGlobalData::GetRandSeed(randSeeds[1]),
                                                           HGPGlobalData::GetRandSeed(randSeeds[2]),
                                                           GetName() + "_combineSeeds"));
        interpolateSeeds = DataNode::Ptr(new InterpolateNode(DataLine(Vector3f(-1.0f, -1.0f, -1.0f)),
                                                             DataLine(Vector3f(1.0f, 1.0f, 1.0f)),
                                                             DataLine(combineSeeds->GetName()),
                                                             InterpolateNode::InterpolationType::IT_Linear,
                                                             GetName() + "_interpolated"));
        normalizeInterp = DataNode::Ptr(new NormalizeNode(DataLine(interpolateSeeds->GetName())));
        multiplyRadius = DataNode::Ptr(new MultiplyNode(DataLine(sphereRadius),
                                                        HGPGlobalData::GetRandSeed(randSeeds[3]),
                                                        DataLine(normalizeInterp->GetName()),
                                                        GetName() + "_multiplyRadius"));
        finalAdd = DataNode::Ptr(new AddNode(DataLine(sphereCenter), DataLine(multiplyRadius->GetName())));
        
        return DataLine(finalAdd->GetName());
    }

private:
    
    mutable DataNode::Ptr combineSeeds, interpolateSeeds, normalizeInterp,
                          multiplyRadius, finalAdd;

    Vector3f sphereCenter;
    float sphereRadius;

    unsigned int randSeeds[4];
};


//Outputs a random world position inside a cube.
class CubePositionComponent : public HGPOutputComponent<3>
{
public:

    Vector3f GetMin(void) const { return min; }
    Vector3f GetMax(void) const { return max; }
    void SetMin(Vector3f value) { min = value; UpdateComponentOutput(); }
    void SetMax(Vector3f value) { max = value; UpdateComponentOutput(); }

    //Gets an array of the three rand seed indexes this CubePositionComponent uses.
    const unsigned int * GetRandSeeds(void) const { return randSeeds; }
    void SetRandSeeds(const unsigned int newSeeds[3]) { for (unsigned int i = 0; i < 3; ++i) randSeeds[i] = newSeeds[i]; UpdateComponentOutput(); }

    
    CubePositionComponent(HGPComponentManager & manager, Vector3f _min, Vector3f _max, const unsigned int _randSeeds[3])
        : HGPOutputComponent(manager), min(_min), max(_max)
    {
        for (unsigned int i = 0; i < 3; ++i)
            randSeeds[i] = _randSeeds[i];
    }

protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        randSeedsPtr = DataNode::Ptr(new CombineVectorNode(HGPGlobalData::GetRandSeed(randSeeds[0]),
                                                           HGPGlobalData::GetRandSeed(randSeeds[1]),
                                                           HGPGlobalData::GetRandSeed(randSeeds[2]),
                                                           GetName() + "_randSeeds"));
        interpolateSeeds = DataNode::Ptr(new InterpolateNode(DataLine(min), DataLine(max),
                                                             DataLine(randSeedsPtr->GetName()),
                                                             InterpolateNode::IT_Linear));
        
        return DataLine(interpolateSeeds->GetName());
    }


private:

    mutable DataNode::Ptr randSeedsPtr, interpolateSeeds;

    Vector3f min;
    Vector3f max;
    unsigned int randSeeds[3];
};