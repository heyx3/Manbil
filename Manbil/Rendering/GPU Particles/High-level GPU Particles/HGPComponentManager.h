#pragma once

#include "HGPOutputComponent.h"


#pragma warning(disable: 4512)

//Manages several HGPOutputComponents that together define a particle system's behavior.
class HGPComponentManager
{
public:

    TextureManager & Manager;
    UniformDictionary & Params;


    HGPComponentPtr(3) GetWorldPosition(void) const { return worldPosition; }
    HGPComponentPtr(1) GetRotation(void) const { return rotation; }
    HGPComponentPtr(2) GetSize(void) const { return size; }
    HGPComponentPtr(4) GetColor(void) const { return color; }
    HGPComponentPtr(1) GetDuration(void) const { return duration; }

    const DataLine & GetTimeInterpolant(void) const { return timeLerp; }


    void SetWorldPosition(HGPComponentPtr(3) newWorldPos) { SwapOutComponent(worldPosition, newWorldPos); }
    void SetRotation(HGPComponentPtr(1) newRotation) { SwapOutComponent(rotation, newRotation); }
    void SetSize(HGPComponentPtr(2) newSize) { SwapOutComponent(size, newSize); }
    void SetColor(HGPComponentPtr(4) newColor) { SwapOutComponent(color, newColor); }
    void SetDuration(HGPComponentPtr(1) newDuration)
    {
        SwapOutComponent(duration, newDuration);
        timeLerp = DataLine(DataNodePtr(new GetLerpComponentNode(DataLine(0.0f), duration->GetComponentOutput(), HGPGlobalData::ParticleElapsedTime)), 0);
    }


    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(1) oldC, HGPComponentPtr(1) newC)
    {
        if (rotation.get() == oldC.get())
            rotation = newC;
        else rotation->SwapOutSubComponent(oldC, newC);

        if (duration.get() == oldC.get())
            duration = newC;
        else duration->SwapOutSubComponent(oldC, newC);

        worldPosition->SwapOutSubComponent(oldC, newC);
        size->SwapOutSubComponent(oldC, newC);
        color->SwapOutSubComponent(oldC, newC);
    }
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(2) oldC, HGPComponentPtr(2) newC)
    {
        if (size.get() == oldC.get())
            size = newC;
        else size->SwapOutSubComponent(oldC, newC);

        worldPosition->SwapOutSubComponent(oldC, newC);
        rotation->SwapOutSubComponent(oldC, newC);
        color->SwapOutSubComponent(oldC, newC);
        duration->SwapOutSubComponent(oldC, newC);
    }
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(3) oldC, HGPComponentPtr(3) newC)
    {
        if (worldPosition.get() == oldC.get())
            worldPosition = newC;
        else worldPosition->SwapOutSubComponent(oldC, newC);

        size->SwapOutSubComponent(oldC, newC);
        rotation->SwapOutSubComponent(oldC, newC);
        color->SwapOutSubComponent(oldC, newC);
        duration->SwapOutSubComponent(oldC, newC);
    }
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(4) oldC, HGPComponentPtr(4) newC)
    {
        if (color.get() == oldC.get())
            color = newC;
        else color->SwapOutSubComponent(oldC, newC);

        size->SwapOutSubComponent(oldC, newC);
        rotation->SwapOutSubComponent(oldC, newC);
        worldPosition->SwapOutSubComponent(oldC, newC);
        duration->SwapOutSubComponent(oldC, newC);
    }


    HGPComponentManager(TextureManager & texManager, UniformDictionary & params)
        : Manager(texManager), Params(params)
    {
        HGPComponentManager & thisM = *this;
        worldPosition = HGPComponentPtr(3)(new ConstantHGPComponent<3>(VectorF((unsigned int)3), thisM));
        rotation = HGPComponentPtr(1)(new ConstantHGPComponent<1>(VectorF((unsigned int)1), thisM));
        size = HGPComponentPtr(2)(new ConstantHGPComponent<1>(VectorF((unsigned int)2, 1.0f), thisM));
        color = HGPComponentPtr(4)(new ConstantHGPComponent<1>(VectorF((unsigned int)4, 1.0f), thisM));
        duration = HGPComponentPtr(1)(new ConstantHGPComponent<1>(VectorF((unsigned int)1, 5.0f), thisM));
    }

    void Initialize(void) { worldPosition->InitializeComponent(); rotation->InitializeComponent(); size->InitializeComponent(); color->InitializeComponent(); duration->InitializeComponent(); }
    void Update(void) { worldPosition->UpdateComponent(); rotation->UpdateComponent(); size->UpdateComponent(); color->UpdateComponent(); duration->UpdateComponent(); }

    //Puts DataLines into the given output map to create particles that behave according to this manager's components.
    void SetGPUPOutputs(std::unordered_map<GPUPOutputs, DataLine> & outputs)
    {
        outputs[GPUPOutputs::GPUP_WORLDPOSITION] = worldPosition->GetComponentOutput();
        outputs[GPUPOutputs::GPUP_QUADROTATION] = rotation->GetComponentOutput();
        outputs[GPUPOutputs::GPUP_SIZE] = size->GetComponentOutput();
        outputs[GPUPOutputs::GPUP_COLOR] = color->GetComponentOutput();
    }

private:

    HGPComponentPtr(3) worldPosition;
    HGPComponentPtr(1) rotation;
    HGPComponentPtr(2) size;
    HGPComponentPtr(4) color;
    HGPComponentPtr(1) duration;

    DataLine timeLerp;
};

#pragma warning(default: 4512)