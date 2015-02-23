#pragma once

#include "GPUParticleDefines.h"
#include "../../Material.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"
#include "../Basic Rendering/MeshData.h"


//Generates a material that calculates and renders GPU particles from a vertex buffer of points.
//The particles are purely kinematic (i.e. pre-computed and not affected by dynamic physics).
//Each particle in the shader has a "Particle ID" -- a vec2 that is unique to each particle.
//The Particle ID can be used as a UV for a lookup into texture data.
//Vertex inputs:
//  0: Particle ID
//  1: Random seed values between 0 and 1 (size 4)
//  2: More random seed values between 0 and 1 (size 2)
//Vertex outputs/Geometry inputs:
//  0: Particle ID
//  1: Random seed values
//  2: More random seed values
//Geometry outputs/Fragment inputs:
//  0: Particle ID
//  1: Random seed values
//  2: More random seed values
//  3: Particle quad's UVs
class GPUParticleGenerator
{
public:

    typedef std::unordered_map<GPUPOutputs, DataLine> GPUPOuts;


    //Powers of 4, for GPU optimization reasons.
    enum NumberOfParticles
    {
        NOP_1,
        NOP_4,
        NOP_16,
        NOP_64,
        NOP_256,
        NOP_1024,
        NOP_4096,
        NOP_16384,
        NOP_65536,
        NOP_262144,
        NOP_1048576,
    };
    //Gets the number of particles as an unsigned integer.
    static unsigned int GetNumbParticles(NumberOfParticles number);
    //Gets the length/width of a texture used for particle data lookup.
    static unsigned int GetParticleDataLength(NumberOfParticles number);


    //Gets whether the given GPUP output type is required for generating particles.
    static bool IsOutputRequired(GPUPOutputs output);

    static ShaderGenerator::GeneratedMaterial GenerateMaterial(GPUPOuts outputs,
                                                               UniformDictionary& outUniforms,
                                                               BlendMode mode);
    static void GenerateGPUPParticles(MeshData& outMesh, NumberOfParticles numb, int randSeed = 12365);
};