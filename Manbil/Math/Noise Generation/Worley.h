#pragma once

#include "BasicGenerators.h"

#define CALL_MEMBER_FUNCTION(object, ptrToMember) ((object).*(ptrToMember))


//TODO: Specify X/Y/Z ranges (0 - 1) that constrain each point's placement in a cell.
//TODO: Change Worley to always generate just one point per cell.
//TODO: More descriptive comment documentation.


//Creates 2D noise using the "Worley Noise" algorithm.
class Worley2D : public Generator2D
{
public:

    //How to calculate the distance between two points (Euclidean, Manhattan, etc.).
	typedef float (*DistanceCalculatorFunc)(Vector2f o, Vector2f p);


    //The number of closest distances that can be used in creating the final noise value.
	static const unsigned int NUMB_DISTANCE_VALUES = 3;
	struct DistanceValues { float Values[NUMB_DISTANCE_VALUES]; DistanceValues(void) { } };
	typedef float (*GetValueFunc)(DistanceValues distVals);

    //How to calculate the distance between two points.
	DistanceCalculatorFunc DistFunc;
    //The value of each pixel of worley noise given the distances to the closest points.
	GetValueFunc ValueGenerator;
	
	//Other members.
	int Seed;
    unsigned int MaxPointsPerCell, MinPointsPerCell, CellSize;


	Worley2D(int _Seed = 12345, unsigned int _CellSize = 30, unsigned int minPointsPerCell = 5, unsigned int maxPointsPerCell = 8)
		: Seed(_Seed), CellSize(_CellSize), MinPointsPerCell(minPointsPerCell), MaxPointsPerCell(maxPointsPerCell),
          DistFunc([](Vector2f o, Vector2f p) { return o.Distance(p); }),
          ValueGenerator([](DistanceValues distVals) { return distVals.Values[0]; }) { }
	~Worley2D(void) { }

	virtual void Generate(Noise2D & noise) const override;
};


//Creates 3D noise using the "Worley Noise" algorithm.
class Worley3D : public Generator3D
{
public:

    //Distance functions.
    typedef float(*DistanceCalculatorFunc)(Vector3f o, Vector3f p);
    static inline float StraightLineDistance(Vector3f o, Vector3f p) { return o.Distance(p); }
    static inline float StraightLineDistanceSquared(Vector3f o, Vector3f p) { return o.DistanceSquared(p); }
    static inline float ManhattanDistance(Vector3f o, Vector3f p) { return o.ManhattanDistance(p); }
    static inline float LargestManhattanDistance(Vector3f o, Vector3f p) { return Mathf::Max(Mathf::Abs(o.x - p.x), Mathf::Abs(o.y - p.y), Mathf::Abs(o.z - p.z)); }
    static inline float SmallestManhattanDistance(Vector3f o, Vector3f p) { return Mathf::Min(Mathf::Abs(o.x - p.x), Mathf::Abs(o.y - p.y), Mathf::Abs(o.z - p.z)); }

    //Value functions.
    static const int NUMB_DISTANCE_VALUES = 3;
    struct DistanceValues { float Values[NUMB_DISTANCE_VALUES]; DistanceValues(void) { } };
    typedef float(*GetValueFunc)(DistanceValues distVals);

    //Function members.
    DistanceCalculatorFunc DistFunc;
    GetValueFunc ValueGenerator;

    //Other members.
    int Seed;
    unsigned int MaxPointsPerCell, MinPointsPerCell, CellSize;


    Worley3D(int _Seed = 12345, unsigned int _CellSize = 30, unsigned int minPointsPerCell = 5, unsigned int maxPointsPerCell = 8)
        : Seed(_Seed), CellSize(_CellSize), MinPointsPerCell(minPointsPerCell), MaxPointsPerCell(maxPointsPerCell),
          DistFunc(&StraightLineDistance), ValueGenerator([](DistanceValues distVals) { return distVals.Values[0]; }) { }
    ~Worley3D(void) { }

    virtual void Generate(Noise3D & noise) const override;
};