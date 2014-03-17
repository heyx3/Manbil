#pragma once

#include "../Rendering.hpp"
#include "../../Math/HigherMath.hpp"
#include "../Texture Management/TextureManager.h"
#include "../../OptionalValue.h"

#include "../Materials/Data Nodes/Miscellaneous/VectorComponentsNode.h"
#include "../Materials/Data Nodes/Math/RemapNode.h"
#include "../Materials/Data Nodes/Parameters/ShaderInNodes.h"
#include "../Materials/Data Nodes/Parameters/TimeNode.h"
#include "../Materials/Data Nodes/Math/AddNode.h"



//Represents a flowing body of water.
//TODO: Try optionally using displacement mapping instead of actually moving the vertices, and see if there is a performance difference.
class Water
{
public:

    typedef std::unordered_map<RenderingChannels, DataLine> RenderChannels;

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
        const Fake2DArray<float> * SeedValues;
        TextureManager * TexManager;
        SeedmapWaterCreationArgs(const Fake2DArray<float> * seedValues = 0, TextureManager * texManager = 0) : SeedValues(seedValues), TexManager(texManager) { }
    };
    //Creates a new Water object.
    Water(unsigned int size, Vector3f pos, Vector3f scale,
          OptionalValue<RippleWaterCreationArgs> rippleArgs,
          OptionalValue<DirectionalWaterCreationArgs> directionArgs,
          OptionalValue<SeedmapWaterCreationArgs> seedmapArgs,
          RenderingModes mode, bool useLighting, LightSettings settings,
          RenderChannels & channels);
    //Destroys this water, releasing all related rendering memory (Material, index/vertex buffers, etc.)
    ~Water(void);
    

    bool HasError(void) const { return !errorMsg.empty(); }
    const std::string & GetErrorMessage(void) const { return errorMsg; }
    void ClearErrorMessage(void) { errorMsg.clear(); }

    const Mesh & GetMesh(void) const { return waterMesh; }
    Mesh & GetMesh(void) { return waterMesh; }

    const Material * GetMaterial(void) const { return waterMat; }
    Material * GetMaterial(void) { return waterMat; }


    //Adds another ripple to the water.
    //Returns the id for the created ripple, or -1 if it was unsuccessful.
    int AddRipple(const RippleWaterArgs & args);
    //Changes the water ripple with the given ID.
    //Returns false if the given id isn't found; returns true otherwise.
    bool ChangeRipple(int element, const RippleWaterArgs & args);

    //Adds a new flow to the water.
    //Returns the id for the created flow, or -1 if this water isn't Directional water.
    int AddFlow(const DirectionalWaterArgs & args);
    //Changes the water flow with the given ID.
    //Returns false if the given id isn't found; returns true otherwise.
    bool ChangeFlow(int element, const DirectionalWaterArgs & args);

    //Changes the properties of the water.
    void SetSeededWater(const SeededWaterArgs & args);
    //Changes the heightmap used to seed this water.
    void SetSeededWaterSeed(sf::Texture * image, bool deletePreviousSeedTex, Vector2i resolution);

    //TODO: Allow ripples to be stopped, and track in the shader how long ago they were stopped using negative "timeSinceCreated" values.

    void SetLighting(const DirectionalLight & light);


    void Update(float elapsedTime);
    bool Render(const RenderInfo & info);


private:


    std::string errorMsg;

    //Ripple stuff.
    int currentRippleIndex;
    int maxRipples;
    int nextRippleID;
    int totalRipples;
    int * rippleIDs;
    //Uniform data is compressed into vectors instead of individual floats for faster handling by the GPU.
    Vector4f * dp_tsc_h_p;
    Vector3f * sXY_sp;

    //Flow stuff.
    int currentFlowIndex;
    int maxFlows;
    int nextFlowID;
    int totalFlows;
    int * flowIDs;
    //Uniform data is compressed into vectors instead of individual floats for faster handling by the GPU.
    Vector4f * f_a_p;
    float * tsc;


    Mesh waterMesh;
    Material * waterMat;
};






//A DataNode that outputs an offset for UV coordinates that creates an interesting water surface distortion effect.
class WaterSurfaceDistortNode : public DataNode
{
public:

    //Gets a value for the "seedIn" argument for this node's constructor that works well with the Water object.
    static DataLine GetWaterSeedIn(void)
    {
        return DataLine(DataNodePtr(new VectorComponentsNode(DataLine(DataNodePtr(new ObjectColorNode()), 0))), 2);
    }


    std::string GetShiftAmplitudeUniform(void) const { return "u_shiftAmplitude" + std::to_string(GetUniqueID()); }
    std::string GetShiftPeriodUniform(void) const { return "u_shiftPeriod" + std::to_string(GetUniqueID()); }

    unsigned int GetUVOffsetOutputIndex(void) const { return 0; }

    virtual std::string GetName(void) const override { return "waterSurfaceDistortNode"; }
    virtual std::string GetOutputName(unsigned int index) const override { assert(index == 0); return GetName() + std::to_string(GetUniqueID()) + "_uvOffset"; }

    //Takes in a seed value. By default, uses an input that will work well for a Water object.
    //Also takes in the amplitude and period of the random shifting around of the surface.
    WaterSurfaceDistortNode(DataLine shiftAmplitude = DataLine(VectorF(0.01f)),
                            DataLine shiftPeriod = DataLine(VectorF(0.5f)),
                            DataLine seedIn = GetWaterSeedIn(),
                            DataLine timeValue = DataLine(DataNodePtr(new AddNode(DataLine(DataNodePtr(new TimeNode()), 0),
                                                                                  DataLine(DataNodePtr(new VectorComponentsNode(DataLine(DataNodePtr(new ObjectColorNode()), 0))), 3))), 0))
        : DataNode(MakeVector(seedIn, shiftAmplitude, shiftPeriod, timeValue), MakeVector(2))
    { 
        assert(seedIn.GetDataLineSize() == 1 && shiftAmplitude.GetDataLineSize() == 1 && shiftPeriod.GetDataLineSize() == 1);
    }


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        strOut += "\tvec2 " + GetOutputName(0) + " = vec2(" + GetSeedInput().GetValue() + " * " +
                                                              GetAmplitudeInput().GetValue() +
                                                          " * sin(" + GetPeriodInput().GetValue() + " * " +
                                                                      GetTimeInput().GetValue() + "));\n";
    }

private:
    const DataLine & GetSeedInput(void) const { return GetInputs()[0]; }
    const DataLine & GetAmplitudeInput(void) const { return GetInputs()[1]; }
    const DataLine & GetPeriodInput(void) const { return GetInputs()[2]; }
    const DataLine & GetTimeInput(void) const { return GetInputs()[3]; }
};