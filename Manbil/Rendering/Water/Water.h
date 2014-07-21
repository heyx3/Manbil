#pragma once

#include "WaterRendering.h"

#include "../../Mesh.h"
#include "../../Material.h"
#include "../../Math/HigherMath.hpp"
#include "../../OptionalValue.h"
#include "../Texture Management/MTexture2D.h"


//The Vertex struct used for water. Inputs:
//0: Pos (size 3)
//1: UV (size 2)
//2: Normal (size 3)
//3: Random seeds useful for WaterSurfaceDistortNode (size 2)
struct WaterVertex
{
    Vector3f Pos;
    Vector2f TexCoord;
    Vector2f RandSeeds;

    WaterVertex(Vector3f pos = Vector3f(), Vector2f texCoord = Vector2f(), Vector2f randSeeds = Vector2f(0.0f, 0.0f))
        : Pos(pos), TexCoord(texCoord), RandSeeds(randSeeds)
    {

    }

    static VertexAttributes GetAttributeData(void) { return VertexAttributes(3, 2, 2, false, false, false); }
};


//Represents a flowing body of water.
//TODO: Create static strings that store the uniform names.
class Water
{
public:

    UniformDictionary Params;

    //Data that only applies to directional water.
    struct DirectionalWaterArgs
    {
    public:
        //The direction and magnitude of the water flow in object space.
        //The magnitude is in units per second.
        Vector2f Flow;
        float Amplitude, Period;
        float TimeSinceCreated;
        DirectionalWaterArgs(Vector2f flow, float amplitude, float period)
            : Flow(flow), Amplitude(amplitude), Period(period), TimeSinceCreated(0.0f) { }
    };

    //Data that only applies to rippling water.
    struct RippleWaterArgs
    {
    public:
        //The position (in world space) of the ripple's source.
        Vector3f Source;
        //The distance from the source at which the amplitude becomes 0.
        float DropoffPoint;
        //The time since this ripple was created.
        float TimeSinceCreated;
        //The height of the waves.
        float Amplitude;
        //The wavelength of the waves.
        float Period;
        //The speed of the waves.
        float Speed;
        RippleWaterArgs(Vector3f source, float dropoffPoint, float height, float period, float speed)
            : Source(source), Period(period), Speed(speed), DropoffPoint(dropoffPoint), Amplitude(height), TimeSinceCreated(0.0f) { }
    };

    //Data that only applies to seeded heightmap water.
    struct SeededWaterArgs
    {
    public:
        float Amplitude, Period, Speed;
        SeededWaterArgs(float amplitude, float period, float speed) : Amplitude(amplitude), Period(period), Speed(speed) { }
    };


    TransformObject & GetTransform(void) { return waterMesh.Transform; }
    const TransformObject & GetTransform(void) const { return waterMesh.Transform; }


    struct RippleWaterCreationArgs
    {
    public:
        unsigned int MaxRipples;
        RippleWaterCreationArgs(unsigned int maxRipples = 0) : MaxRipples(maxRipples) { }
    };
    struct DirectionalWaterCreationArgs
    {
    public:
        unsigned int MaxFlows;
        DirectionalWaterCreationArgs(unsigned int maxFlows = 0) : MaxFlows(maxFlows) { }
    };
    struct SeedmapWaterCreationArgs
    {
    public:
        const MTexture2D * SeedValues;
        SeedmapWaterCreationArgs(const MTexture2D * seedValues = 0) : SeedValues(seedValues) { }
    };
    //Creates a new Water object.
    Water(unsigned int size, Vector3f pos, Vector3f scale,
          OptionalValue<RippleWaterCreationArgs> rippleArgs,
          OptionalValue<DirectionalWaterCreationArgs> directionArgs,
          OptionalValue<SeedmapWaterCreationArgs> seedmapArgs);
    //Destroys this water, releasing all related rendering memory (Material, index/vertex buffers, etc.)
    ~Water(void);

    //Gets the location of the water-related uniforms from the given material.
    void UpdateUniformLocations(const Material * mat)
    {
        std::vector<UniformList::Uniform> fArrUs = mat->GetUniforms(RenderPasses::BaseComponents).FloatArrayUniforms;
        Params.FloatArrayUniforms["dropoffPoints_timesSinceCreated_heights_periods"].Location =
            UniformList::FindUniform("dropoffPoints_timesSinceCreated_heights_periods", fArrUs).Loc;
        Params.FloatArrayUniforms["sourcesXY_speeds"].Location =
            UniformList::FindUniform("sourcesXY_speeds", fArrUs).Loc;
        Params.FloatArrayUniforms["flow_amplitude_period"].Location =
            UniformList::FindUniform("flow_amplitude_period", fArrUs).Loc;
        Params.FloatArrayUniforms["timesSinceCreated"].Location =
            UniformList::FindUniform("timesSinceCreated", fArrUs).Loc;
    }

    //Gets the water mesh without updating its uniforms.
    const Mesh & GetMesh(void) const { return waterMesh; }

    //Adds another ripple to the water.
    //Returns the id for the created ripple, or -1 if it was unsuccessful.
    unsigned int AddRipple(const RippleWaterArgs & args);
    //Changes the water ripple with the given ID.
    //Returns false if the given id isn't found; returns true otherwise.
    bool ChangeRipple(unsigned int element, const RippleWaterArgs & args);

    //Adds a new flow to the water.
    //Returns the id for the created flow, or -1 if this water isn't Directional water.
    unsigned int AddFlow(const DirectionalWaterArgs & args);
    //Changes the water flow with the given ID.
    //Returns false if the given id isn't found; returns true otherwise.
    bool ChangeFlow(unsigned int element, const DirectionalWaterArgs & args);

    //Changes the properties of the water.
    void SetSeededWater(const SeededWaterArgs & args);
    //Changes the heightmap used to seed this water.
    void SetSeededWaterSeed(const MTexture2D * newSeedValues);

    //TODO: Allow ripples to be stopped, and track in the shader how long ago they were stopped using negative "timeSinceCreated" values.

    void Update(float elapsedTime);


private:

    void UpdateMeshUniforms(void);

    //Ripple stuff.
    unsigned int currentRippleIndex;
    unsigned int maxRipples;
    unsigned int nextRippleID;
    unsigned int totalRipples;
    unsigned int * rippleIDs;
    //Uniform data is compressed into vectors instead of individual floats for faster handling by the GPU.
    Vector4f * dp_tsc_h_p;
    Vector3f * sXY_sp;

    //Flow stuff.
    unsigned int currentFlowIndex;
    unsigned int maxFlows;
    unsigned int nextFlowID;
    unsigned int totalFlows;
    unsigned int * flowIDs;
    //Uniform data is compressed into vectors instead of individual floats for faster handling by the GPU.
    Vector4f * f_a_p;
    float * tsc;

    const MTexture2D * seedTex;
    Mesh waterMesh;
};