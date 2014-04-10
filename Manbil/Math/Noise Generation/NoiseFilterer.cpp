#include "NoiseFilterer.h"

#include <math.h>
#include <assert.h>



typedef NoiseFilterer NF;

const float sqrt2 = sqrt(2.0f),
	sqrt2Inv = 1.0f / sqrt2;

void NF::ReflectValues(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	struct RefValuesStruct { Interval i; };
	RefValuesStruct rvs;
	rvs.i = Interval::GetZeroToOneInterval();

	SetAtEveryPoint((void*)&rvs, [](void *pData, Vector2i loc, Noise2D * nse) { return (*(RefValuesStruct*)pData).i.Reflect((*nse)[loc]); });
}

void NF::RemapValues(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	Interval newVals = Interval::GetZeroToOneInterval();
	Interval oldVals = RemapValues_OldVals;

	struct RemapValuesStruct { Interval oldVs, newVs; Noise2D * nse; };
	RemapValuesStruct rvs;
	rvs.oldVs = oldVals;
	rvs.newVs = newVals;
	rvs.nse = noise;

	FillRegion->DoToEveryPoint((void*)&rvs, [](void * pDat, Vector2i loc, float strength)
	{
		RemapValuesStruct * rvS = (RemapValuesStruct*)pDat;
		float * fOut = &(*(rvS->nse))[loc];
		*fOut = rvS->newVs.ClampValueToInterval(rvS->oldVs.MapValue(rvS->newVs, *fOut));
	},
	*noise, Vector2i(noise->GetWidth(), noise->GetHeight()));
}

void NF::UpContrast(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	//Get the smoothing function to use.
	float (*smoothStepper)(float inF);
	switch (UpContrast_Power)
	{
		case UpContrastPowers::CUBIC:
			smoothStepper = &BasicMath::Smooth;
			break;
		case UpContrastPowers::QUINTIC:
			smoothStepper = &BasicMath::Supersmooth;
			break;

		default: assert(false);
	}


	SetAtEveryPoint((void*)smoothStepper, [](void * pDat, Vector2i loc, Noise2D * noise)
	{
		return ((float (*)(float inF))pDat)((*noise)[loc]);
	});
}

void NF::Average(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	struct AverageStruct { float average; int count; Noise2D * nse; AverageStruct(void) : average(0.0f), count(0) { } };
	AverageStruct avs;
	avs.nse = noise;

	FillRegion->DoToEveryPoint((void*)&avs, [](void* pData, Vector2i loc, float str)
	{
		AverageStruct * avS = (AverageStruct*)pData;

		avS->average += (*(avS->nse))[loc];
		avS->count += 1;
	}, *noise, Vector2i(noise->GetWidth(), noise->GetHeight()), false);

	if (avs.count > 0)
	{
		avs.average /= (float)avs.count;
	}

	SetAtEveryPoint((void*)&avs.average, [](void * pData, Vector2i loc, Noise2D * noise) { return *(float*)pData; });
}

void NF::Flatten(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	SetAtEveryPoint((void*)&Flatten_FlatValue, [](void *pData, Vector2i loc, Noise2D * noise) { return *(float*)pData; });
}

void NF::Smooth(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	SetAtEveryPoint((void*)0, [](void *pData, Vector2i loc, Noise2D * noise)
	{
		int x, y, lX, lY;

		float average = 0.0f;
		int count = 0;
		Vector2i locTemp;

		//Average the surrounding values.
		for (x = -1; x <= 1; ++x)
		{
			lX = x + loc.x;
			locTemp.x = lX;

			if (lX >= 0 && lX < noise->GetWidth())
			{
				for (y = -1; y <= 1; ++y)
				{
					lY = y + loc.y;
					locTemp.y = lY;

					if ((x != 0 || y != 0) &&
						lY >= 0 && lY < noise->GetHeight())
					{
						average += (*noise)[locTemp];
						count += 1;
					}
				}
			}
		}

		if (count > 0)
		{
			average /= (float)count;
		}

		return average;
	});
}

void NF::Increase(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	SetAtEveryPoint((void*)&Increase_Amount, [](void *pData, Vector2i loc, Noise2D * noise)
	{
		return (*noise)[loc] + *(float*)pData;
	});
}

void NF::Min(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

    SetAtEveryPoint((void*)&Min_Value, [](void *pData, Vector2i loc, Noise2D * noise)
    {
        return BasicMath::Min((*noise)[loc], *(float*)pData);
    });
}
void NF::Max(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

    SetAtEveryPoint((void*)&Max_Value, [](void *pData, Vector2i loc, Noise2D * noise)
    {
        return BasicMath::Max((*noise)[loc], *(float*)pData);
    });
}
void NF::Clamp(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

    float fs[2] = { Min_Value, Max_Value };
    SetAtEveryPoint((void*)fs, [](void *pData, Vector2i loc, Noise2D * noise)
    {
        return BasicMath::Clamp((*noise)[loc], ((float*)pData)[0], ((float*)pData)[1]);
    });
}

void NF::Noise(Noise2D * nse) const
{
    if (nse != 0) noise = nse;

	struct NoiseStruct { float amount; int seed; FastRand fr; };
	NoiseStruct ns;
	ns.amount = Noise_Amount;
	ns.seed = Noise_Seed;
	ns.fr = FastRand(Noise_Seed);

	SetAtEveryPoint((void*)&ns, [](void *pData, Vector2i loc, Noise2D * noise)
	{
		NoiseStruct * nS = (NoiseStruct*)pData;

		nS->fr.Seed = Vector3i(loc.x, loc.y, nS->seed).GetHashCode();
		return (*noise)[loc] + (nS->amount * nS->fr.GetZeroToOne());
	});
}

void NF::Generate(Noise2D & dat) const
{
	if (NoiseToFilter == 0 || FillRegion == 0 || FilterFunc == 0) return;

	NoiseToFilter->Generate(dat);

	noise = &dat;
	((*this).*FilterFunc)(0);
}

void NF::SetAtEveryPoint(void * pData, float (*GetValue)(void * pData, Vector2i loc, Noise2D * noise)) const
{
	//Package necessary information into a struct.
	struct SAEPData { void * PData; bool Invert; Noise2D * Noise; float(*PGetValue)(void * pData, Vector2i loc, Noise2D * noise); };
	SAEPData d;
	d.PData = pData;
	d.PGetValue = GetValue;
	d.Noise = noise;
	d.Invert = InvertFunc;

	//Fill the noise.
	FillRegion->DoToEveryPoint((void*)(&d), [](void *pData, Vector2i loc, float strength)
	{
		//Unpack the information.
		SAEPData * dt = (SAEPData*)pData;
		void *PD = dt->PData;
		float (*pGV)(void * pData, Vector2i loc, Noise2D * pNoise) = dt->PGetValue;

		//Get the original and new value.
		float preVal = pGV(PD, loc, dt->Noise),
			  orgVal = dt->Noise->operator[](loc);

		//If necessary, invert the effect of the filter function.
		if (dt->Invert)
		{
			preVal = orgVal - (preVal - orgVal);
		}

		//Lerp between the old and new value using the strength value.
		dt->Noise->operator[](loc) = BasicMath::Clamp(BasicMath::Lerp(orgVal, preVal, strength), 0.0f, 1.0f);
	},
	*noise, Vector2i(noise->GetWidth(), noise->GetHeight()));
}