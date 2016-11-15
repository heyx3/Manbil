#pragma once

#include "BasicGenerators.h"


//TODO: Make wrapping optional. If not wrapping, just expand the cell grid by 2 and offset the cell position by -1.


//Creates 2D noise using the "Worley Noise" algorithm.
//Basically splits the grid up into a voroni diagram and generates noise
//    based on how far each spot is from the center of its cell.
class Worley2D : public Generator2D
{
public:
    
    //A way to calculate the distance between a point and the center of its cell.
	typedef float (*DistanceCalculatorFunc)(Vector2f o, Vector2f p);

    static inline float StraightLineDistance       (Vector2f o, Vector2f p) { return o.Distance(p); }
    static inline float StraightLineDistanceSquared(Vector2f o, Vector2f p) { return o.DistanceSquared(p); }
    static inline float ManhattanDistance          (Vector2f o, Vector2f p) { return o.ManhattanDistance(p); }
    static inline float LargestManhattanDistance   (Vector2f o, Vector2f p) { return Mathf::Max(Mathf::Abs(o.x - p.x), Mathf::Abs(o.y - p.y)); }
    static inline float SmallestManhattanDistance  (Vector2f o, Vector2f p) { return Mathf::Min(Mathf::Abs(o.x - p.x), Mathf::Abs(o.y - p.y)); }


	static const unsigned int NUMB_DISTANCE_VALUES = 3;
    //The distance to the closest N cells near a grid spot.
	struct DistanceValues { float Values[NUMB_DISTANCE_VALUES]; DistanceValues(void) { } };

    //Calculates the noise value based on how far a spot is from nearby grid cells.
	typedef float (*GetValueFunc)(DistanceValues distVals);


    //How to calculate the distance between two points.
	DistanceCalculatorFunc DistFunc;
    //The value of each pixel of worley noise given the distances to the closest cells around it.
    GetValueFunc ValueGenerator;
	
    //The approximate spacing between each cell.
    unsigned int CellSize;
    //Offsets each cell's seed used to generate its center position.
    Vector2i CellOffset;
    
    //The variability (between 0 and 1) of worley cells along each axis.
    Vector2f Variability;

	int Seed;


    Worley2D(DistanceCalculatorFunc distFunc = &StraightLineDistance,
             GetValueFunc noiseOutput = [](DistanceValues distVals) { return distVals.Values[0]; },
             unsigned int cellSize = 64,
             Vector2f variability = Vector2f(0.5f, 0.5f),
             int seed = 12345,
             Vector2i cellOffset = Vector2i());


	virtual void Generate(Noise2D& noise) const override;
};



//Creates 3D noise using the "Worley Noise" algorithm.
//Basically splits the grid up into a voroni diagram and generates noise
//    based on how far each spot is from the center of its cell.
class Worley3D : public Generator3D
{
public:
    
    //A way to calculate the distance between a point and the center of its cell.
	typedef float (*DistanceCalculatorFunc)(Vector3f o, Vector3f p);

    static inline float StraightLineDistance       (Vector3f o, Vector3f p) { return o.Distance(p); }
    static inline float StraightLineDistanceSquared(Vector3f o, Vector3f p) { return o.DistanceSquared(p); }
    static inline float ManhattanDistance          (Vector3f o, Vector3f p) { return o.ManhattanDistance(p); }
    static inline float LargestManhattanDistance   (Vector3f o, Vector3f p) { return Mathf::Max(Mathf::Abs(o.x - p.x), Mathf::Abs(o.y - p.y)); }
    static inline float SmallestManhattanDistance  (Vector3f o, Vector3f p) { return Mathf::Min(Mathf::Abs(o.x - p.x), Mathf::Abs(o.y - p.y)); }


	static const unsigned int NUMB_DISTANCE_VALUES = 3;
    //The distance to the closest N cells near a grid spot.
	struct DistanceValues { float Values[NUMB_DISTANCE_VALUES]; DistanceValues(void) { } };

    //Calculates the noise value based on how far a spot is from nearby grid cells.
	typedef float (*GetValueFunc)(DistanceValues distVals);


    //How to calculate the distance between two points.
	DistanceCalculatorFunc DistFunc;
    //The value of each pixel of worley noise given the distances to the closest cells around it.
    GetValueFunc ValueGenerator;
	
    //The approximate spacing between each cell.
    unsigned int CellSize;
    //Offsets each cell's seed used to generate its center position.
    Vector3i CellOffset;
    
    //The variability (between 0 and 1) of worley cells along each axis.
    Vector3f Variability;

	int Seed;


    Worley3D(DistanceCalculatorFunc distFunc = &StraightLineDistance,
             GetValueFunc noiseOutput = [](DistanceValues distVals) { return distVals.Values[0]; },
             unsigned int cellSize = 64, Vector3f variability = Vector3f(0.5f, 0.5f, 0.5f),
             int seed = 12345,
             Vector3i cellOffset = Vector3i());


	virtual void Generate(Noise3D& noise) const override;
};