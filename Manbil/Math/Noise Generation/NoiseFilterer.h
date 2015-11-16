#pragma once

#include "BasicGenerators.h"
#include "NoiseFilterRegion.h"
#include "NoiseFilterVolume.h"


#pragma region TwoD Noise

//Filters 2D floating-point noise using different algorithms.
//Either a generator can be specified in "NoiseToFilter",
//     or a Noise2D can be supplied directly to the desired filter function.
class NoiseFilterer2D : public Generator2D
{
public:

	NoiseFilterer2D(void) { InitData(); }
	void InitData(void)
    {
        RemapValues_OldVals = Interval::GetZeroToOne();
        RemapValues_NewVals = Interval::GetZeroToOne();
		InvertFunc = false;
		FillRegion = 0;
		NoiseToFilter = 0;
		FilterFunc = 0;

		Flatten_FlatValue = 0.0f;

		Noise_Amount = 1.0f;
		Noise_Seed = 12345;

		Increase_Amount = 1.0f;

		UpContrast_Power = UpContrastPowers::CUBIC;
        UpContrast_Passes = 1;
        
        CustomFunc_Function = 0;
        CustomFunc_CustomData = 0;
	}


	NoiseFilterRegion* FillRegion;
	Generator2D* NoiseToFilter;
	bool InvertFunc;

	typedef void (NoiseFilterer2D::*MemberFunc)(Noise2D* nse) const;
	MemberFunc FilterFunc;


	//Remaps the noise from the given original range to the given new range.
	void RemapValues(Noise2D* nse = 0) const;
	Interval RemapValues_OldVals, RemapValues_NewVals;


	//Reflects the noise's values around the center of the noise range. The strength will always be 1.0 regardless of what is passed in.
    void ReflectValues(Noise2D* nse = 0) const;


	//Increases the contrast in the noise.
    void UpContrast(Noise2D* nse = 0) const;
	//The higher the power, the more expensive the function.
	enum UpContrastPowers
	{
		CUBIC,
		QUINTIC,
	};
	UpContrastPowers UpContrast_Power;
    //The number of times the effect is applied.
    unsigned int UpContrast_Passes;


	//Gets the average of the area and pushes all noise values to that average.
    void Average(Noise2D* nse = 0) const;


	//Flattens the noise.
    void Flatten(Noise2D* nse = 0) const;
	float Flatten_FlatValue;


    //Performs the operation "Mathf::Min(Min_Value, x)" on each noise value.
    void Min(Noise2D* nse = 0) const;
    //Performs the operation "Mathf::Max(Max_Value, x)" on each noise value.
    void Max(Noise2D* nse = 0) const;
    //Performs the operation "Mathf::Clamp(x, Min_Value, Max_Value)" on each noise value.
    void Clamp(Noise2D* nse = 0) const;
    float Min_Value, Max_Value;


	//Smooths the area.
    void Smooth(Noise2D* nse = 0) const;


	//Adds random noise to the area.
    void Noise(Noise2D* nse = 0) const;
	float Noise_Amount;
	int Noise_Seed;


	//Increases the area by the given amount.
    void Increase(Noise2D* nse = 0) const;
	float Increase_Amount;


    //Runs a user-defined function on every noise value.
    void CustomFunc(Noise2D* nse = 0) const;
    float(*CustomFunc_Function)(Vector2u pos, float inNoise, void* customData) = 0;
    void* CustomFunc_CustomData = 0;


	virtual void Generate(Noise2D& noise) const override;


private:

	//Iterates through every spot in the noise and sets it to a value. Note:
	//   1) The given function should return the new noise value at full strength (1.0f) and not inverted;
	//   2) The new value at the given noise point will be set to
	//      lerp([original value], [function value], [function strength]).
	void SetAtEveryPoint(void* pData, float(*GetValue)(void* pData, Vector2u loc, Noise2D* noise)) const;

	
	mutable Noise2D* noise;
};

#pragma endregion


#pragma region ThreeD Noise

//Filters 3D floating-point noise using different algorithms.
class NoiseFilterer3D : public Generator3D
{
public:

    NoiseFilterer3D(void) { InitData(); }
    void InitData(void)
    {
        RemapValues_OldVals = Interval::GetZeroToOne();
        RemapValues_NewVals = Interval::GetZeroToOne();
        InvertFunc = false;
        FillVolume = 0;
        NoiseToFilter = 0;
        FilterFunc = 0;

        Set_Value = 0.5f;

        Noise_Amount = 1.0f;
        Noise_Seed = 12345;

        Increase_Amount = 1.0f;

        UpContrast_Power = UpContrastPowers::CUBIC;
        UpContrast_Passes = 1;
    }


    NoiseFilterVolume* FillVolume;
    bool InvertFunc;
    Generator3D* NoiseToFilter;
    
    typedef void (NoiseFilterer3D::*MemberFunc)(Noise3D* nse) const;
    MemberFunc FilterFunc;


    //Remaps the noise from the given original range to the range 0.0-1.0.
    void RemapValues(Noise3D* nse = 0) const;
    Interval RemapValues_OldVals, RemapValues_NewVals;


    //Reflects the noise's values around the center of the noise range. The strength will always be 1.0 regardless of what is passed in.
    void ReflectValues(Noise3D* nse = 0) const;


    //Increases the contrast in the noise.
    void UpContrast(Noise3D* nse = 0) const;
    //The higher the power, the more expensive the function.
    enum UpContrastPowers
    {
        CUBIC,
        QUINTIC,
    };
    UpContrastPowers UpContrast_Power;
    //The number of times the effect is applied.
    unsigned int UpContrast_Passes;


    //Gets the average of the area and pushes all noise values to that average.
    void Average(Noise3D* nse = 0) const;


    //Sets the noise to a specific value.
    void Set(Noise3D* nse = 0) const;
    float Set_Value;


    //Performs the operation "Mathf::Min(Min_Value, x)" on each noise value.
    void Min(Noise3D* nse = 0) const;
    //Performs the operation "Mathf::Max(Max_Value, x)" on each noise value.
    void Max(Noise3D* nse = 0) const;
    //Performs the operation "Mathf::Clamp(x, Min_Value, Max_Value)" on each noise value.
    void Clamp(Noise3D* nse = 0) const;
    float Min_Value, Max_Value;


    //Smooths an area.
    void Smooth(Noise3D* nse = 0) const;


    //Adds random noise to an area.
    void Noise(Noise3D* nse = 0) const;
    float Noise_Amount;
    int Noise_Seed;


    //Increases the value of noise in an area by "Increase_Amount".
    void Increase(Noise3D* nse = 0) const;
    float Increase_Amount;


    //Runs a user-defined function on every noise value.
    void CustomFunc(Noise3D* nse = 0) const;
    float(*CustomFunc_Function)(Vector3u pos, float inNoise, void* customData) = 0;
    void* CustomFunc_CustomData = 0;
    

	virtual void Generate(Noise3D& noise) const override;
  

private:

    //Iterates through every spot in the noise and sets it to a value. Note:
    //   1) The given function should return the new noise value at full strength (1.0f) and not inverted;
    //   2) The new value at the given noise point will be set to
    //      lerp([original value], [function value], [function strength]).
    void SetAtEveryPoint(void* pData, float(*GetValue)(void* pData, Vector3u loc, Noise3D* noise)) const;

	
	mutable Noise3D* noise;
};

#pragma endregion