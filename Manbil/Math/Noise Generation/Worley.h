#pragma once

#include "BasicGenerators.h"

#define CALL_MEMBER_FUNCTION(object, ptrToMember) ((object).*(ptrToMember))


//Creates 2D noise using the "Worley Noise" algorithm.
class Worley2D : public Generator2D
{
public:

	//Distance functions.
	typedef float (*DistanceCalculatorFunc)(Vector2f o, Vector2f p);
	static inline float StraightLineDistance(Vector2f o, Vector2f p) { return o.Distance(p); }
	static inline float StraightLineDistanceSquared(Vector2f o, Vector2f p) { return o.DistanceSquared(p); }
	static inline float StraightLineDistanceFast(Vector2f o, Vector2f p) { return 1.0f / o.FastInvDistance(p); }
	static inline float ManhattanDistance(Vector2f o, Vector2f p) { return o.ManhattanDistance(p); }
	static inline float LargestManhattanDistance(Vector2f o, Vector2f p) { return BasicMath::Max(BasicMath::Abs(o.x - p.x), BasicMath::Abs(o.y - p.y)); }
	static inline float SmallestManhattanDistance(Vector2f o, Vector2f p) { return BasicMath::Min(BasicMath::Abs(o.x - p.x), BasicMath::Abs(o.y - p.y)); }
	static inline float QuadraticDistance(Vector2f o, Vector2f p) { float f1 = BasicMath::Abs(o.x - p.x), f2 = BasicMath::Abs(o.y - p.y); return (f1 * f1) + (f1 * f2) + (f2 * f2); }

	//Value functions.
	static const int NUMB_DISTANCE_VALUES = 3;
	struct DistanceValues { float Values[NUMB_DISTANCE_VALUES]; DistanceValues(void) { } };
	typedef float (*GetValueFunc)(DistanceValues distVals);

	//Function members.
	DistanceCalculatorFunc DistFunc;
	GetValueFunc ValueGenerator;
	
	//Other members.
	int Seed, CellSize;
	Interval PointsPerCell;


	Worley2D(int _Seed = 12345, int _CellSize = 30, Interval _PointsPerCell = Interval(5.0f, 8.0f))
		: Seed(_Seed), CellSize(_CellSize), PointsPerCell(_PointsPerCell), DistFunc(&StraightLineDistanceFast), ValueGenerator([](DistanceValues distVals) { return distVals.Values[0]; }) { }
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
    static inline float StraightLineDistanceFast(Vector3f o, Vector3f p) { return 1.0f / o.FastInvDistance(p); }
    static inline float ManhattanDistance(Vector3f o, Vector3f p) { return o.ManhattanDistance(p); }
    static inline float LargestManhattanDistance(Vector3f o, Vector3f p) { return BasicMath::Max(BasicMath::Abs(o.x - p.x), BasicMath::Abs(o.y - p.y), BasicMath::Abs(o.z - p.z)); }
    static inline float SmallestManhattanDistance(Vector3f o, Vector3f p) { return BasicMath::Min(BasicMath::Abs(o.x - p.x), BasicMath::Abs(o.y - p.y), BasicMath::Abs(o.z - p.z)); }

    //Value functions.
    static const int NUMB_DISTANCE_VALUES = 3;
    struct DistanceValues { float Values[NUMB_DISTANCE_VALUES]; DistanceValues(void) { } };
    typedef float(*GetValueFunc)(DistanceValues distVals);

    //Function members.
    DistanceCalculatorFunc DistFunc;
    GetValueFunc ValueGenerator;

    //Other members.
    int Seed, CellSize;
    Interval PointsPerCell;


    Worley3D(int _Seed = 12345, int _CellSize = 30, Interval _PointsPerCell = Interval(5.0f, 8.0f))
        : Seed(_Seed), CellSize(_CellSize), PointsPerCell(_PointsPerCell), DistFunc(&StraightLineDistanceFast), ValueGenerator([](DistanceValues distVals) { return distVals.Values[0]; })
    { }
    ~Worley3D(void) { }

    virtual void Generate(Noise3D & noise) const override;
};