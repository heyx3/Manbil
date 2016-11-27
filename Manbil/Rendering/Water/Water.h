#pragma once

#include "WaterRendering.h"

#include "../Basic Rendering/Mesh.h"
#include "../Basic Rendering/Material.h"
#include "../../Math/HigherMath.hpp"
#include "../../OptionalValue.h"
#include "../Textures/MTexture2D.h"


//The type of vertex used in this water system:
//0: Pos (size 3)
//1: UV (size 2)
struct WaterVertex
{
	//TODO: Change to a 2D position.

    Vector3f Pos;
    Vector2f TexCoord;

    WaterVertex(Vector3f pos = Vector3f(), Vector2f texCoord = Vector2f())
        : Pos(pos), TexCoord(texCoord) { }

    static RenderIOAttributes GetVertexAttributes(void);
};


//TODO: Add move semantics.
//TODO: Add an integer uniform saying how many ripples are actually in use, to improve performance.
//TODO: Use Inverse FFT for faster ripple calculations.
//TODO: Allow ripples to be stopped, and track in the shader how long ago they were stopped using negative "timeSinceCreated" values.


//Represents a flowing body of water.
//Contains two kinds of water flows:
// -Directional -- water that oscillates along a direction on the plane of the surface
// -Circular -- water that oscillates outwards from some point on the surface
class Water
{
public:

	using RippleID = unsigned int;

	static const RippleID RIPPLEID_INVALID = UINT_MAX;


    //Data about a directional ripple.
    struct DirectionalRipple
    {
    public:
        //The direction and magnitude of the water flow in object space.
        //The magnitude is in object units per second.
        Vector2f Flow;
        float Amplitude, Period;
        //The amount of time since this wave was initially created.
        //This is used to create a smooth fade-in for the wave.
        float TimeSinceCreated;

		DirectionalRipple(Vector2f flow, float amplitude, float period)
            : Flow(flow), Amplitude(amplitude), Period(period), TimeSinceCreated(0.0f) { }
		DirectionalRipple() : DirectionalRipple(Vector2f(0.000001f, 0.0f), 0.0f, 999999.0f) { }
    };

    //Data about a circular ripple.
    struct CircularRipple
    {
    public:
        //The position of the ripple's source in object space (i.e. from {0,0} to {1,1}.
        Vector2f Source;
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

		CircularRipple(Vector2f source, float dropoffPoint, float height, float period, float speed)
            : Source(source), Period(period), Speed(speed), DropoffPoint(dropoffPoint),
              Amplitude(height), TimeSinceCreated(0.0f) { }
		CircularRipple() : CircularRipple(Vector2f(), 0.000001f, 0.0f, 9999999.0f, 0.000001f) { }
    };


    Mesh MyMesh;
    UniformDictionary Params;

    
	//"size" is the number of vertices in the water mesh along each axis.
    Water(unsigned int size, unsigned int maxCircularRipples = 0, unsigned int maxDirectionalRipples = 0);


	//Returns the id for the created ripple so that it can be modified in the future.
	//If the maximum number of ripples already exists, one of two things happens:
	//    * If the "removeIfFull" parameter is used, the ripple with that ID will get removed
	//    * Otherwise, the new ripple won't be added, and RIPPLEID_INVALID will be returned.
	RippleID AddRipple(DirectionalRipple newRipple, RippleID removeIfFull = RIPPLEID_INVALID);
	//Returns the id for the created ripple so that it can be modified in the future.
	//If the maximum number of ripples already exists, one of two things happens:
	//    * If the "removeIfFull" parameter is used, the ripple with that ID will get removed
	//    * Otherwise, the new ripple won't be added, and RIPPLEID_INVALID will be returned.
	RippleID AddRipple(CircularRipple newRipple, RippleID removeIfFull = RIPPLEID_INVALID);

	void ChangeRipple(RippleID toChange, const DirectionalRipple& changedRipple);
	void ChangeRipple(RippleID toChange, const CircularRipple& changedRipple);

	bool RippleExists_Circular(RippleID ripple) const;
	bool RippleExists_Directional(RippleID ripple) const;

	void RemoveRipple_Directional(RippleID toRemove);
	void RemoveRipple_Circular(RippleID toRemove);

	//Returns nullptr if a ripple with the given ID doesn't exist.
	const DirectionalRipple* GetRipple_Directional(RippleID id) const;
	//Returns nullptr if a ripple with the given ID doesn't exist.
	const CircularRipple* GetRipple_Circular(RippleID id) const;


    //Sets this instance to render with the given material.
    //The material is assumed to use a WaterNode in its DataNode graph.
    void SetMaterial(Material* newMat);

    //Updates this water's ripples.
    void Update(float elapsedTime);

    //Renders this water. Assumes its material has already been set through "SetMaterial()".
	//Note that without the transform, this water spans the horizontal range {0, 0} to {1, 1}.
    void Render(const Transform& transform, const RenderInfo& info);


private:

    void UpdateMeshUniforms(void);

    unsigned int maxRipples_Circular,
				 maxRipples_Directional;
	std::unordered_map<RippleID, CircularRipple> ripples_Circular;
	std::unordered_map<RippleID, DirectionalRipple> ripples_Directional;

    //Uniform data is packed into vectors instead of individual floats for efficiency.
    //This data represents the DropoffPoint, TimeSinceCreated,
	//    Height, and Period of each circular ripple.
    std::vector<Vector4f> ripplesUniformCircular_dp_tsc_h_p;
    //Uniform data is packed into vectors instead of individual floats for efficiency.
    //This data represents the source of each circular ripple along the water's surface,
	//    as well as its speed.
    std::vector<Vector3f> ripplesUniformCircular_sXY_sp;
	//Uniform data is packed into vectors instead of individual floats for efficiency.
	//This data represents the Direction/magnitude, Amplitude, and Period of each directional ripple.
	std::vector<Vector4f> ripplesUniformDirectional_d_a_p;
	//This data represents the Time Since Created of each directional flow.
	std::vector<float> ripplesUniformDirectional_tsc;

    Material* waterMat;
};