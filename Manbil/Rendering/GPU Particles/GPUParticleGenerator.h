#pragma once

#include "GPUParticleDefines.h"
#include "../../Material.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"


//Generates a material that calculates and renders GPU particles from a vertex buffer of points.
//The particles are purely kinematic (i.e. pre-computed and not affected by dynamic physics).
//Each particle in the shader has a "Particle ID" -- a vec2 that is unique to each particle.
//The Particle ID can be used as a UV for a lookup into texture data or as a seed value for randomization.
//Vertex inputs:
//  0: Particle ID
//Vertex outputs/Geometry inputs:
//  0: Particle ID
//Geometry outputs/Fragment inputs:
//  0: Particle ID
//  1: Particle quad's UVs
class GPUParticleGenerator
{
public:

    //The vertex that is used for this particle generator.
    struct Vertex
    {
    public:
        Vector2f ParticleID;
        Vertex(Vector2f particleID = Vector2f()) : ParticleID(particleID) { }
        static VertexAttributes GetAttributeData(void) { return VertexAttributes(2, false); }
    };

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
    unsigned int ToInt(NumberOfParticles number)
    {
        switch (number)
        {
            case NumberOfParticles::NOP_1: return 1;
            case NumberOfParticles::NOP_4: return 4;
            case NumberOfParticles::NOP_16: return 16;
            case NumberOfParticles::NOP_64: return 64;
            case NumberOfParticles::NOP_256: return 256;
            case NumberOfParticles::NOP_1024: return 1024;
            case NumberOfParticles::NOP_4096: return 4096;
            case NumberOfParticles::NOP_16384: return 16384;
            case NumberOfParticles::NOP_65536: return 65536;
            case NumberOfParticles::NOP_262144: return 262144;
            case NumberOfParticles::NOP_1048576: return 1048576;
        }
    }


    //Gets whether the given GPUP output type is required for generating particles.
    static bool IsOutputRequired(GPUPOutputs output);

    static ShaderGenerator::GeneratedMaterial GenerateGPUParticleMaterial(std::unordered_map<GPUPOutputs, DataLine> outputs, UniformDictionary & outUniforms, RenderingModes mode);
    static RenderObjHandle GenerateGPUPParticles(NumberOfParticles numb);
};