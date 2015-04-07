#pragma once

#include "../Basic Rendering/RenderIOAttributes.h"
#include "../../Math/Lower Math/Vectors.h"


//The basic GPU particle system is just an extension of the DataNode system.
struct DataLine;

//Different output channels for GPU particle data.
enum GPUPOutputs
{
    //Size: 3.
    GPUP_WORLDPOSITION,
    //Size: 1.
    GPUP_QUADROTATION,
    //Size: 4.
    GPUP_COLOR,
    //Size: 2.
    GPUP_SIZE,
};

//Gets whether the given output line is a valid size for the given GPU particle data type.
bool IsValidGPUPOutput(const DataLine& outputData, GPUPOutputs outputType);


//TODO: Don't use the geometry shader; use quads.
//TODO: Instead of having a single "ParticleVertex" class, allow the user to customize the class by specifying the number of random seeds to generate per particle.

//The vertex that is used for particles.
struct ParticleVertex
{
public:
    Vector2f ParticleID;
    Vector4f RandSeeds1;
    Vector2f RandSeeds2;
    ParticleVertex(Vector2f particleID = Vector2f(),
                   Vector4f randSeeds1 = Vector4f(0.5f, 0.5f, 0.5f, 0.5f),
                   Vector2f randSeeds2 = Vector2f(0.5f, 0.5f))
        : ParticleID(particleID), RandSeeds1(randSeeds1), RandSeeds2(randSeeds2) { }
    static RenderIOAttributes GetVertexInputData(void);
    static RenderIOAttributes GetGeoInputData(void);
    static RenderIOAttributes GetFragInputData(void);
};