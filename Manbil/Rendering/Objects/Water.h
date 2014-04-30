#pragma once

#include "WaterRendering.h"

#include "../../Mesh.h"
#include "../../Material.h"
#include "../../Math/HigherMath.hpp"
#include "../Texture Management/TextureManager.h"
#include "../../OptionalValue.h"


//Represents a flowing body of water.
class Water
{
public:

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
        const Array2D<float> * SeedValues;
        TextureManager * TexManager;
        SeedmapWaterCreationArgs(const Array2D<float> * seedValues = 0, TextureManager * texManager = 0) : SeedValues(seedValues), TexManager(texManager) { }
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
        waterMesh.Uniforms.FloatArrayUniforms["dropoffPoints_timesSinceCreated_heights_periods"].Location =
            UniformList::FindUniform("dropoffPoints_timesSinceCreated_heights_periods", fArrUs).Loc;
        waterMesh.Uniforms.FloatArrayUniforms["sourcesXY_speeds"].Location =
            UniformList::FindUniform("sourcesXY_speeds", fArrUs).Loc;
        waterMesh.Uniforms.FloatArrayUniforms["flow_amplitude_period"].Location =
            UniformList::FindUniform("flow_amplitude_period", fArrUs).Loc;
        waterMesh.Uniforms.FloatArrayUniforms["timesSinceCreated"].Location =
            UniformList::FindUniform("timesSinceCreated", fArrUs).Loc;
    }

    //Gets the water mesh without updating its uniforms.
    const Mesh & GetMesh(void) const { return waterMesh; }
    //Updates the water mesh's uniforms with the current ripple/flow data and then returns the mesh.
    Mesh & UpdateGetMesh() { UpdateMeshUniforms(); return waterMesh; }

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
    void SetSeededWaterSeed(sf::Texture * image, bool deletePreviousSeedTex, Vector2i resolution);

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


    Mesh waterMesh;
};