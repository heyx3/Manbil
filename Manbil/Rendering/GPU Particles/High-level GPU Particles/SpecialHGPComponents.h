#pragma once

#include "HGPComponentManager.h"


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
        return DataLine(DataNodePtr(new AddNode(initialPos->GetComponentOutput(),
                                                DataLine(DataNodePtr(new MultiplyNode(HGPGlobalData::ParticleElapsedTime,
                                                                                      velocity->GetComponentOutput())), 0))), 0);
    }


private:

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
    
    SpherePositionComponent(HGPComponentManager & manager, Vector3f _sphereCenter, float _sphereRadius)
        : HGPOutputComponent(manager), sphereCenter(_sphereCenter), sphereRadius(_sphereRadius)
    {

    }


protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        DataLine dir(DataNodePtr(new NormalizeNode(
                                    DataLine(DataNodePtr(new RemapNode(HGPGlobalData::ParticleRandSeedInputs,
                                                                       DataLine(0.0f), DataLine(1.0f),
                                                                       DataLine(-1.0f), DataLine(1.0f))), 0))), 0);
        DataLine radius(DataNodePtr(new MultiplyNode(DataLine(sphereRadius),
                                                     DataLine(DataNodePtr(new WhiteNoiseNode(HGPGlobalData::ParticleRandSeedInputs)), 0))), 0);
        return DataLine(DataNodePtr(new AddNode(DataLine(sphereCenter),
                                                DataLine(DataNodePtr(new MultiplyNode(dir, radius)), 0))), 0);
    }

private:

    Vector3f sphereCenter;
    float sphereRadius;
};


//Outputs a random world position inside a cube.
class CubePositionComponent : public HGPOutputComponent<3>
{
public:

    Vector3f GetMin(void) const { return min; }
    Vector3f GetMax(void) const { return max; }
    void SetMin(Vector3f value) { min = value; UpdateComponentOutput(); }
    void SetMax(Vector3f value) { max = value; UpdateComponentOutput(); }
    
    CubePositionComponent(HGPComponentManager & manager, Vector3f _min, Vector3f _max)
        : HGPOutputComponent(manager), min(_min), max(_max)
    {

    }

protected:

    virtual DataLine GenerateComponentOutput(void) const override
    {
        return DataLine(DataNodePtr(new InterpolateNode(DataLine(min), DataLine(max), HGPGlobalData::ParticleRandSeedInputs, DataLine(1.0f))), 0);
    }


private:

    Vector3f min;
    Vector3f max;
};