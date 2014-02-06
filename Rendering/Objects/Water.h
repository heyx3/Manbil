#pragma once

#include "../Rendering.hpp"
#include "../../Math/HigherMath.hpp"

//Represents a flowing body of water.
//Color data is stored in u_sampler0, and normal map is stored in u_sampler1.
class Water
{
public:

    //TODO: Change to use one uber-shader that has ripples, flows, AND seeded heightmaps.

    //Different kinds of water motion.
    enum WaterTypes
    {
        //Moves in a direction.
        Directed,
        //Ripples outward from several spots.
        Rippling,
        //Ripples wildly; uses a "heightmap" as a seed value for each vertex's rippling.
        SeededHeightmap,
    };

    //Data that only applies to directional water.
    struct DirectionalWaterArgs
    {
    public:
        //The direction and magnitude of the water flow (in object space).
        //The magnitude is in units per second.
        Vector2f Flow;
        float Amplitude;
        DirectionalWaterArgs(Vector2f flow, float amplitude) : Flow(flow), Amplitude(amplitude) { }
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
        RippleWaterArgs(Vector3f source, float dropoffPoint, float height, float period, float speed) : Source(source), Period(period), Speed(speed), DropoffPoint(dropoffPoint), Amplitude(height), TimeSinceCreated(0.0f) { }
    };

    //Data that only applies to seeded heightmap water.
    struct SeededWaterArgs
    {
    public:
        float Amplitude, Period, Speed;
        SeededWaterArgs(float amplitude, float period, float speed) : Amplitude(amplitude), Period(period), Speed(speed) { }
    };


    Material * Mat;
    TransformObject Transform;


    //Makes a new Water object that uses circular ripples.
    Water(unsigned int size, unsigned int maxRipples, Vector3f pos = Vector3f());
    //Makes a new Water object that uses directional water.
    Water(unsigned int size, DirectionalWaterArgs mainFlow, unsigned int maxRipples, Vector3f pos = Vector3f());
    //Makes a new Water object that uses seeded heightmap water. The heightmap must be square.
    Water(const Fake2DArray<float> & seedValues, Vector3f pos = Vector3f());

    ~Water(void);
    

    bool HasError(void) const { return !errorMsg.empty(); }
    const std::string & GetErrorMessage(void) const { return errorMsg; }
    void ClearErrorMessage(void) { errorMsg.clear(); }

    const Mesh & GetMesh(void) const { return waterMesh; }
    Mesh & GetMesh(void) { return waterMesh; }

    WaterTypes GetWaterType(void) const { return waterType; }


    //Adds another ripple to the water.
    //This function only applies to rippling water.
    //Returns the id for the created ripple, or -1 if it was unsuccessful.
    int AddRipple(const RippleWaterArgs & args);
    //Changes the water ripples with the given ID.
    //Returns false if this water isn't Rippling or the given id isn't found; returns true otherwise.
    bool ChangeRipple(int element, const RippleWaterArgs & args);

    //Adds a new flow to the water.
    //This function only applies to directional water.
    //Returns the id for the created flow, or -1 if this water isn't Directional water.
    int AddFlow(const DirectionalWaterArgs & args);
    //Changes he water flow with the given ID.
    //Returns false if this water isn't Directional or the given id isn't found; returns true otherwise.
    bool ChangeFlow(int element, const DirectionalWaterArgs & args);

    //Changes the properties of the water.
    //Returns false if this water isn't SeededHeightmap; returns true otherwise.
    bool SetSeededWater(const SeededWaterArgs & args);
    //Changes the heightmap used to seed this water.
    //Returns false if this water isn't SeededHeightmap; returns true otherwise.
    bool SetSeededWaterSeed(RenderObjHandle image, Vector2i resolution);

    //TODO: Allow ripples to be stopped, and track in the shader how long ago they were stopped. Maybe use negative "TimeSinceCreated" values?

    //The following functions return whether the uniform was set successfully.

    void SetLighting(const Materials::LitTexture_DirectionalLight light) { Materials::LitTexture_SetUniforms(waterMesh, light); }


    void Update(float elapsedTime);
    bool Render(const RenderInfo & info);


private:

    WaterTypes waterType;
    std::string errorMsg;

    //Ripple stuff.
    int currentRippleIndex;
    int maxRipples;
    int nextRippleID;
    int totalRipples;
    int * rippleIDs;
    Vector4f * dp_tsc_h_p; //If you want an explanation for the weird name, look at the shader.
    Vector3f * sXY_sp;

    //Flow stuff.
    struct DirectionalWaterArgsElement { public: DirectionalWaterArgs Args; int Element; DirectionalWaterArgsElement(const DirectionalWaterArgs & a, int e) : Args(a), Element(e) { } };
    int currentFlowIndex;
    int maxFlows;
    int nextFlowID;
    int totalFlows;
    std::vector<DirectionalWaterArgsElement> flows;


    Mesh waterMesh;

    //Gets the single RenderingPass needed to render rippling water.
    static RenderingPass GetRippleWaterRenderer(int maxRipples);
    //Gets the single RenderingPass needed to render directional water.
    static RenderingPass GetDirectionalWaterRenderer(int maxFlows);
    //Gets the single RenderingPass needed to render seeded heightmap water.
    static RenderingPass GetSeededHeightRenderer(void);
};