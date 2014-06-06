#pragma once

#include "../../Vertices.h"


//The GPU particle system in Manbil piggy-backs off of the DataNode system for generating materials.
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
bool IsValidGPUPOutput(const DataLine & outputData, GPUPOutputs outputType);


//The vertex that is used for particles.
//TODO: Allow the user to choose whether to use the geometry shader on points or to just use quads.
struct ParticleVertex
{
public:
    Vector2f ParticleID;
    float RandSeed;
    ParticleVertex(Vector2f particleID = Vector2f(), float randSeed = 0.5f) : ParticleID(particleID), RandSeed(randSeed) { }
    static VertexAttributes GetAttributeData(void) { return VertexAttributes(2, 1, false, false); }
};