#pragma once

#include "HGPOutputComponent.h"


//Component that outputs a particle's worl position given its acceleration and its starting position/velocity.
class KinematicsHGPComponent : public HGPOutputComponent<3>
{
public:

    HGPComponentPtr(3) Acceleration, InitialVelocity, InitialPosition;
    HGPComponentPtr(1) Duration;

    KinematicsHGPComponent(TextureManager & mngr, UniformDictionary & params,
                           HGPComponentPtr(3) accel, HGPComponentPtr(3) initialVel, HGPComponentPtr(3) initialPos, HGPComponentPtr(1) duration)
        : Acceleration(accel), InitialVelocity(initialVel), InitialPosition(initialPos), Duration(duration), HGPOutputComponent(mngr, params)
    {

    }


protected:

    virtual DataLine GenerateComponentOutput(void) const override;
    virtual void InitializeComponent(void) override { Acceleration->InitializeComponent(); InitialVelocity->InitializeComponent(); InitialPosition->InitializeComponent(); }
    virtual void UpdateComponent(void) override { Acceleration->UpdateComponent(); InitialVelocity->UpdateComponent(); InitialPosition->UpdateComponent(); }
};


//Outputs a random world position inside a sphere.
class SpherePositionComponent : public HGPOutputComponent<3>
{
public:

    Vector3f GetSphereCenter(void) const { return sphereCenter; }
    void SetSphereCenter(Vector3f newCenter) { sphereCenter = newCenter; UpdateComponentOutput(); }
    float GetSphereRadius(void) const { return sphereRadius; }
    void SetSphereRadius(float newRadius) { sphereRadius = newRadius; UpdateComponentOutput(); }
    
    SpherePositionComponent(TextureManager & mngr, UniformDictionary & params, Vector3f _sphereCenter, float _sphereRadius)
        : HGPOutputComponent(mngr, params), sphereCenter(_sphereCenter), sphereRadius(_sphereRadius)
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
    
    CubePositionComponent(TextureManager & mngr, UniformDictionary & params, Vector3f _min, Vector3f _max)
        : HGPOutputComponent(mngr, params), min(_min), max(_max)
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