#include "NoiseFilterer.h"

#include <assert.h>


#pragma warning(disable: 4100)


#pragma region TwoD Noise

typedef NoiseFilterer2D NF2;

const float sqrt2 = sqrt(2.0f),
	        sqrt2Inv = 1.0f / sqrt2;

void NF2::ReflectValues(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

	struct RefValuesStruct { Interval i; };
	RefValuesStruct rvs;
	rvs.i = Interval::GetZeroToOne();

	SetAtEveryPoint((void*)&rvs, [](void *pData, Vector2u loc, Noise2D* nse) { return (*(RefValuesStruct*)pData).i.Reflect((*nse)[loc]); });
}

void NF2::RemapValues(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

	Interval newVals = RemapValues_NewVals;
	Interval oldVals = RemapValues_OldVals;

	struct RemapValuesStruct { Interval oldVs, newVs; Noise2D* nse; };
	RemapValuesStruct rvs;
	rvs.oldVs = oldVals;
	rvs.newVs = newVals;
	rvs.nse = noise;

    FillRegion->DoToEveryPoint((void*)&rvs, [](void* pDat, Vector2u loc, float strength)
	{
		RemapValuesStruct* rvS = (RemapValuesStruct*)pDat;
		float* fOut = &(*(rvS->nse))[loc];
		*fOut = rvS->oldVs.MapValue(rvS->newVs, *fOut);
	},
	*noise, Vector2u(noise->GetWidth(), noise->GetHeight()));
}

void NF2::UpContrast(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

    //Struct to pass into the lambda.
    struct UpContrastArgs
    {
    public:
        float(*SmoothStepper)(float inF);
        unsigned int Iterations;
    };

    UpContrastArgs args;
    args.Iterations = UpContrast_Passes;
	switch (UpContrast_Power)
	{
		case UpContrastPowers::CUBIC:
			args.SmoothStepper = &Mathf::Smooth;
			break;
		case UpContrastPowers::QUINTIC:
			args.SmoothStepper = &Mathf::Supersmooth;
			break;

		default: assert(false);
	}
    

    SetAtEveryPoint((void*)(&args), [](void* pDat, Vector2u loc, Noise2D* _noise)
	{
        UpContrastArgs rgs = *(UpContrastArgs*)pDat;
        float val = (*_noise)[loc];

        for (unsigned int pass = 0; pass < rgs.Iterations; ++pass)
            val = rgs.SmoothStepper(val);

        return val;
	});
}

void NF2::Average(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

	struct AverageStruct { float average; int count; Noise2D* nse; AverageStruct(void) : average(0.0f), count(0) { } };
	AverageStruct avs;
	avs.nse = noise;

    FillRegion->DoToEveryPoint((void*)&avs, [](void* pData, Vector2u loc, float str)
	{
		AverageStruct* avS = (AverageStruct*)pData;

		avS->average += (*(avS->nse))[loc];
		avS->count += 1;
    }, *noise, noise->GetDimensions(), false);

	if (avs.count > 0)
	{
		avs.average /= (float)avs.count;
	}

	SetAtEveryPoint((void*)&avs.average, [](void* pData, Vector2u loc, Noise2D* _noise) { return *(float*)pData; });
}

void NF2::Flatten(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

	SetAtEveryPoint((void*)&Flatten_FlatValue, [](void *pData, Vector2u loc, Noise2D* _noise) { return *(float*)pData; });
}

void NF2::Smooth(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

	SetAtEveryPoint((void*)0, [](void *pData, Vector2u loc, Noise2D* _noise)
	{
		float average = 0.0f;
		unsigned int count = 0;

#define ADD_NOISE(noiseX, noiseY) \
	{ average += (*_noise)[Vector2u(noiseX, noiseY)]; \
	  count += 1; }

		ADD_NOISE(loc.x, loc.y);
		if (loc.y > 0)
			ADD_NOISE(loc.x, loc.y - 1);
		if (loc.y < _noise->GetHeight() - 1)
			ADD_NOISE(loc.x, loc.y + 1);

		if (loc.x > 0)
		{
			ADD_NOISE(loc.x - 1, loc.y);
			if (loc.y > 0)
				ADD_NOISE(loc.x - 1, loc.y - 1);
			if (loc.y < _noise->GetHeight() - 1)
				ADD_NOISE(loc.x - 1, loc.y + 1);
		}

		if (loc.x < _noise->GetWidth() - 1)
		{
			ADD_NOISE(loc.x + 1, loc.y);
			if (loc.y > 0)
				ADD_NOISE(loc.x + 1, loc.y - 1);
			if (loc.y < _noise->GetHeight() - 1)
				ADD_NOISE(loc.x + 1, loc.y + 1);
		}

#undef ADD_NOISE

		if (count > 0)
		{
			average /= (float)count;
		}

		return average;
	});
}

void NF2::Increase(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

	SetAtEveryPoint((void*)&Increase_Amount, [](void *pData, Vector2u loc, Noise2D* _noise)
	{
		return (*_noise)[loc] + *(float*)pData;
	});
}

void NF2::Min(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

    SetAtEveryPoint((void*)&Min_Value, [](void *pData, Vector2u loc, Noise2D* _noise)
    {
        return Mathf::Min((*_noise)[loc], *(float*)pData);
    });
}
void NF2::Max(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

    SetAtEveryPoint((void*)&Max_Value, [](void *pData, Vector2u loc, Noise2D* _noise)
    {
        return Mathf::Max((*_noise)[loc], *(float*)pData);
    });
}
void NF2::Clamp(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

    float fs[2] = { Min_Value, Max_Value };
    SetAtEveryPoint((void*)fs, [](void *pData, Vector2u loc, Noise2D* _noise)
    {
        return Mathf::Clamp((*_noise)[loc], ((float*)pData)[0], ((float*)pData)[1]);
    });
}

void NF2::Noise(Noise2D* _nse) const
{
    if (_nse != 0)
		noise = _nse;

	struct NoiseStruct { float amount; int seed; FastRand fr; };
	NoiseStruct ns;
	ns.amount = Noise_Amount;
	ns.seed = Noise_Seed;
	ns.fr = FastRand(Noise_Seed);

	SetAtEveryPoint((void*)&ns, [](void* pData, Vector2u loc, Noise2D* _noise)
	{
		NoiseStruct* nS = (NoiseStruct*)pData;

		nS->fr.Seed = Vector3i((int)loc.x, (int)loc.y, nS->seed).GetHashCode();
		return (*_noise)[loc] + (nS->amount * nS->fr.GetZeroToOne());
	});
}

void NF2::CustomFunc(Noise2D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    struct CustomFuncStruct { float(*F)(Vector2u pos, float f, void* p); void* P; };
    CustomFuncStruct cfs;
    cfs.F = CustomFunc_Function;
    cfs.P = CustomFunc_CustomData;

    SetAtEveryPoint((void*)&cfs, [](void* pData, Vector2u loc, Noise2D* _noise)
    {
        CustomFuncStruct* cfS = (CustomFuncStruct*)pData;

        return cfS->F(loc, (*_noise)[loc], cfS->P);
    });
}

void NF2::Generate(Noise2D& dat) const
{
	assert(NoiseToFilter != 0 && FillRegion != 0 && FilterFunc != 0);

	NoiseToFilter->Generate(dat);

	noise = &dat;
	((*this).*FilterFunc)(0);
}

void NF2::SetAtEveryPoint(void* pData, float (*GetValue)(void* pData, Vector2u loc, Noise2D* _noise)) const
{
	//Package necessary information into a struct.
	struct SAEPData { void* PData; bool Invert; Noise2D* Noise; float(*PGetValue)(void* pData, Vector2u loc, Noise2D* _noise); };
	SAEPData d;
	d.PData = pData;
	d.PGetValue = GetValue;
	d.Noise = noise;
	d.Invert = InvertFunc;

	//Fill the noise.
	FillRegion->DoToEveryPoint((void*)(&d), [](void *pData, Vector2u loc, float strength)
	{
		//Unpack the information.
		SAEPData* dt = (SAEPData*)pData;
		void *PD = dt->PData;
		float (*pGV)(void* pData, Vector2u loc, Noise2D* pNoise) = dt->PGetValue;

		//Get the original and new value.
		float preVal = pGV(PD, loc, dt->Noise),
			  orgVal = dt->Noise->operator[](loc);

		//If necessary, invert the effect of the filter function.
		if (dt->Invert)
		{
			preVal = orgVal - (preVal - orgVal);
		}

		//Lerp between the old and new value using the strength value.
		dt->Noise->operator[](loc) = Mathf::Clamp(Mathf::Lerp(orgVal, preVal, strength), 0.0f, 1.0f);
	}, *noise, noise->GetDimensions());
}

#pragma endregion



#pragma region ThreeD Noise

typedef NoiseFilterer3D NF3;


void NF3::ReflectValues(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    struct RefValuesStruct { Interval i; };
    RefValuesStruct rvs;
    rvs.i = Interval::GetZeroToOne();

    SetAtEveryPoint((void*)&rvs, [](void *pData, Vector3u loc, Noise3D* nse) { return (*(RefValuesStruct*)pData).i.Reflect((*nse)[loc]); });
}

void NF3::RemapValues(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    Interval newVals = RemapValues_NewVals;
    Interval oldVals = RemapValues_OldVals;

    struct RemapValuesStruct { Interval oldVs, newVs; Noise3D* nse; };
    RemapValuesStruct rvs;
    rvs.oldVs = oldVals;
    rvs.newVs = newVals;
    rvs.nse = noise;

    FillVolume->DoToEveryPoint((void*)&rvs, [](void* pDat, Vector3u loc, float strength)
    {
        RemapValuesStruct* rvS = (RemapValuesStruct*)pDat;
        float* fOut = &(*(rvS->nse))[loc];
        *fOut = rvS->oldVs.MapValue(rvS->newVs, *fOut);
    }, *noise, noise->GetDimensions());
}

void NF3::UpContrast(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    //Struct to pass into the lambda.
    struct UpContrastArgs
    {
    public:
        float(*SmoothStepper)(float inF);
        unsigned int Iterations;
    };

    UpContrastArgs args;
    args.Iterations = UpContrast_Passes;
    switch (UpContrast_Power)
    {
    case UpContrastPowers::CUBIC:
        args.SmoothStepper = &Mathf::Smooth;
        break;
    case UpContrastPowers::QUINTIC:
        args.SmoothStepper = &Mathf::Supersmooth;
        break;

    default: assert(false);
    }


    SetAtEveryPoint((void*)(&args), [](void* pDat, Vector3u loc, Noise3D* _noise)
    {
        UpContrastArgs rgs = *(UpContrastArgs*)pDat;
        float val = (*_noise)[loc];

        for (unsigned int pass = 0; pass < rgs.Iterations; ++pass)
            val = rgs.SmoothStepper(val);

        return val;
    });
}

void NF3::Average(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    struct AverageStruct { float average; int count; Noise3D* nse; AverageStruct(void) : average(0.0f), count(0) { } };
    AverageStruct avs;
    avs.nse = noise;

    FillVolume->DoToEveryPoint((void*)&avs, [](void* pData, Vector3u loc, float str)
    {
        AverageStruct* avS = (AverageStruct*)pData;

        avS->average += (*(avS->nse))[loc];
        avS->count += 1;
    }, *noise, noise->GetDimensions(), false);

    if (avs.count > 0)
    {
        avs.average /= (float)avs.count;
    }

    SetAtEveryPoint((void*)&avs.average, [](void* pData, Vector3u loc, Noise3D* _noise) { return *(float*)pData; });
}

void NF3::Set(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    SetAtEveryPoint((void*)&Set_Value, [](void *pData, Vector3u loc, Noise3D* _noise) { return *(float*)pData; });
}

void NF3::Smooth(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    SetAtEveryPoint((void*)0, [](void *pData, Vector3u lc, Noise3D* _noise)
    {
        //Average the surrounding values.
        Vector3i noiseLoc;
        float avg = 0.0f;
        unsigned int count = 0;
        for (Vector3i loc(0, 0, -1); loc.z <= 1; ++loc.z)
        {
            noiseLoc.z = loc.z + lc.z;

            if (noiseLoc.z < 0 || noiseLoc.z >= (int)_noise->GetDepth())
                continue;

            for (loc.y = -1; loc.y <= 1; ++loc.y)
            {
                noiseLoc.y = loc.y + lc.y;

                if (noiseLoc.y < 0 || noiseLoc.y >= (int)_noise->GetHeight())
                    continue;

                for (loc.x = -1; loc.x <= 1; ++loc.x)
                {
                    noiseLoc.x = loc.x + lc.x;

                    if ((noiseLoc.x >= 0 && noiseLoc.x < (int)_noise->GetWidth()) &&
                        (loc.x != 0 || loc.y != 0 || loc.z != 0))
                    {
                        avg += (*_noise)[Vector3u((unsigned int)noiseLoc.x, (unsigned int)noiseLoc.y, (unsigned int)noiseLoc.z)];
                        count += 1;
                    }
                }
            }
        }

        if (count > 0)
            avg /= (float)count;
        else avg = 0.5f;

        return avg;
    });
}

void NF3::Increase(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    SetAtEveryPoint((void*)&Increase_Amount, [](void *pData, Vector3u loc, Noise3D* _noise)
    {
        return (*_noise)[loc] + *(float*)pData;
    });
}

void NF3::Min(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    SetAtEveryPoint((void*)&Min_Value, [](void *pData, Vector3u loc, Noise3D* _noise)
    {
        return Mathf::Min((*_noise)[loc], *(float*)pData);
    });
}
void NF3::Max(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    SetAtEveryPoint((void*)&Max_Value, [](void *pData, Vector3u loc, Noise3D* _noise)
    {
        return Mathf::Max((*_noise)[loc], *(float*)pData);
    });
}
void NF3::Clamp(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    float fs[2] = { Min_Value, Max_Value };
    SetAtEveryPoint((void*)fs, [](void *pData, Vector3u loc, Noise3D* _noise)
    {
        return Mathf::Clamp((*_noise)[loc], ((float*)pData)[0], ((float*)pData)[1]);
    });
}

void NF3::Noise(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    struct NoiseStruct { float amount; int seed; FastRand fr; };
    NoiseStruct ns;
    ns.amount = Noise_Amount;
    ns.seed = Noise_Seed;
    ns.fr = FastRand(Noise_Seed);

    SetAtEveryPoint((void*)&ns, [](void *pData, Vector3u loc, Noise3D* _noise)
    {
        NoiseStruct* nS = (NoiseStruct*)pData;

        nS->fr.Seed = Vector4i(loc.x, loc.y, loc.z, nS->seed).GetHashCode();

        return (*_noise)[loc] + (nS->amount * (-1.0f + (2.0f * nS->fr.GetZeroToOne())));
    });
}

void NF3::CustomFunc(Noise3D* _nse) const
{
    if (_nse != 0)
        noise = _nse;

    struct CustomFuncStruct { float(*F)(Vector3u pos, float f, void* p); void* P; };
    CustomFuncStruct cfs;
    cfs.F = CustomFunc_Function;
    cfs.P = CustomFunc_CustomData;

    SetAtEveryPoint((void*)&cfs, [](void* pData, Vector3u loc, Noise3D* _noise)
    {
        CustomFuncStruct* cfS = (CustomFuncStruct*)pData;

        return cfS->F(loc, (*_noise)[loc], cfS->P);
    });
}

void NF3::Generate(Noise3D& dat) const
{
	assert(NoiseToFilter != 0 && FillVolume != 0 && FilterFunc != 0);

	NoiseToFilter->Generate(dat);

	noise = &dat;
	((*this).*FilterFunc)(0);
}

void NF3::SetAtEveryPoint(void* pData, float(*GetValue)(void* pData, Vector3u loc, Noise3D* _noise)) const
{
    //Package necessary information into a struct.
    struct SAEPData { void* PData; bool Invert; Noise3D* Noise; float(*PGetValue)(void* pData, Vector3u loc, Noise3D* _noise); };
    SAEPData d;
    d.PData = pData;
    d.PGetValue = GetValue;
    d.Noise = noise;
    d.Invert = InvertFunc;

    //Fill the noise.
    FillVolume->DoToEveryPoint((void*)(&d), [](void *pData, Vector3u loc, float strength)
    {
        //Unpack the information.
        SAEPData* dt = (SAEPData*)pData;
        void *PD = dt->PData;
        float(*pGV)(void* pData, Vector3u loc, Noise3D* pNoise) = dt->PGetValue;

        //Get the original and new value.
        float preVal = pGV(PD, loc, dt->Noise),
              orgVal = dt->Noise->operator[](loc);

        //If necessary, invert the effect of the filter function.
        if (dt->Invert)
        {
            preVal = orgVal - (preVal - orgVal);
        }

        //Lerp between the old and new value using the strength value.
        dt->Noise->operator[](loc) = Mathf::Clamp(Mathf::Lerp(orgVal, preVal, strength), 0.0f, 1.0f);
    }, *noise, noise->GetDimensions());
}

#pragma endregion


#pragma warning(default: 4100)