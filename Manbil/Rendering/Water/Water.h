#pragma once

#include "WaterRendering.h"

#include "../Basic Rendering/Mesh.h"
#include "../Basic Rendering/Material.h"
#include "../../Math/HigherMath.hpp"
#include "../../OptionalValue.h"
#include "../Textures/MTexture2D.h"


//The Vertex type used for this water system. Inputs:
//0: Pos (size 3)
//1: UV (size 2)
struct WaterVertex
{
    Vector3f Pos;
    Vector2f TexCoord;

    WaterVertex(Vector3f pos = Vector3f(), Vector2f texCoord = Vector2f())
        : Pos(pos), TexCoord(texCoord) { }

    static RenderIOAttributes GetVertexAttributes(void);
};


//TODO: Add move semantics.

//Represents a flowing body of water.
//Contains two kinds of water flows:
// -Directional -- water that oscillates along a direction on the plane of the surface
// -Ripple -- water that oscillates outwards from some point on the surface
class Water
{
public:

    //Data about a directional water flow.
    struct DirectionalWaterArgs
    {
    public:
        //The direction and magnitude of the water flow in object space.
        //The magnitude is in object units per second.
        Vector2f Flow;
        float Amplitude, Period;
        //The amount of time since this wave was initially created.
        //This is used to create a smooth fade-in for the wave.
        float TimeSinceCreated;
        DirectionalWaterArgs(Vector2f flow, float amplitude, float period)
            : Flow(flow), Amplitude(amplitude), Period(period), TimeSinceCreated(0.0f) { }
    };

    //Data about a ripple water flow.
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
            : Source(source), Period(period), Speed(speed), DropoffPoint(dropoffPoint),
              Amplitude(height), TimeSinceCreated(0.0f) { }
    };


    Mesh MyMesh;
    UniformDictionary Params;


    TransformObject& GetTransform(void) { return MyMesh.Transform; }
    const TransformObject& GetTransform(void) const { return MyMesh.Transform; }

    
    Water(unsigned int size, Vector3f pos, Vector3f scale,
          unsigned int maxRipples = 0,
          unsigned int maxFlows = 0);
    ~Water(void);


    //Sets this instance to render with the given material.
    //The given new material is assumed to use the WaterNode uniforms from the WaterNode class.
    void SetMaterial(Material* newMat);


    //Adds another ripple to the water.
    //Returns the id for the created ripple so that it can be modified in the future.
    unsigned int AddRipple(const RippleWaterArgs& args);
    //Changes the water ripple with the given ID.
    //Returns false if the given id isn't found; returns true otherwise.
    bool ChangeRipple(unsigned int element, const RippleWaterArgs& args);

    //Adds a new flow to the water.
    //Returns the id for the created flow so that it can be modified in the future.
    unsigned int AddFlow(const DirectionalWaterArgs& args);
    //Changes the water flow with the given ID.
    //Returns false if the given id isn't found; returns true otherwise.
    bool ChangeFlow(unsigned int element, const DirectionalWaterArgs& args);

    //TODO: Allow ripples to be stopped, and track in the shader how long ago they were stopped using negative "timeSinceCreated" values.

    //Updates this water's flows.
    void Update(float elapsedTime);
    //Renders this water. Assumes its material has already been set through "SetMaterial()".
    void Render(const RenderInfo& info);


private:

    void UpdateMeshUniforms(void);


    //Ripple stuff.
    unsigned int currentRippleIndex;
    unsigned int maxRipples;
    unsigned int nextRippleID;
    unsigned int totalRipples;
    unsigned int* rippleIDs;
    //Uniform data is compressed into vectors instead of individual floats for faster handling by the GPU.
    //This data represents the DropoffPoint, TimeSinceCreated, Height, and Period of each ripple.
    Vector4f* dp_tsc_h_p;
    //Uniform data is compressed into vectors instead of individual floats for faster handling by the GPU.
    //This data represents the source of each ripple along the water's surface as well as its speed.
    Vector3f* sXY_sp;

    //Directional flow stuff.
    unsigned int currentFlowIndex;
    unsigned int maxFlows;
    unsigned int nextFlowID;
    unsigned int totalFlows;
    unsigned int* flowIDs;
    //Uniform data is compressed into vectors instead of individual floats for faster handling by the GPU.
    //This data represents the direction/magnitude, amplitude, and period of each directional flow.
    Vector4f* f_a_p;
    //This data represents the time since each directional flow was created.
    float* tsc;

    Material* waterMat;
};