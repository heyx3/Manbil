#pragma once

#include "GPUParticleDefines.h"
#include "../../Material.h"
#include "../Materials/Data Nodes/ShaderGenerator.h"


//Generates a material that calculates and renders GPU particles from a vertex buffer of points.
//The particles are purely kinematic (i.e. pre-computed and not affected by dynamic physics).
class GPUParticleGenerator
{
public:

    //UVs are the first vertex output to the fragment shader; they are a vec2.
    static const unsigned int VertexOutput_UVs = 1;

    //Gets whether the given GPUP output type is required for generating particles.
    static bool IsOutputRequired(GPUPOutputs output);

    static ShaderGenerator::GeneratedMaterial GenerateGPUParticleMaterial(std::unordered_map<GPUPOutputs, DataLine> outputs, UniformDictionary & outUniforms, RenderingModes mode);
};