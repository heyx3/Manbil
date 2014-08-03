#pragma once

#include "HGPOutputComponent.h"



//Manages several HGPOutputComponents that together define a particle system's behavior.
class HGPComponentManager
{
public:

    UniformDictionary & Params;


    HGPComponentManager(UniformDictionary & params, std::string _name = "");


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
    void SetDuration(HGPComponentPtr(1) newDuration);


    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(1) oldC, HGPComponentPtr(1) newC);
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(2) oldC, HGPComponentPtr(2) newC);
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(3) oldC, HGPComponentPtr(3) newC);
    //Searches down this component's directed graph recursively and replaces all instances of the given old component with the given new component.
    void SwapOutComponent(HGPComponentPtr(4) oldC, HGPComponentPtr(4) newC);


    void Initialize(void);
    void Update(float elapsedTime);

    //Puts DataLines into the given output map to create particles that behave according to this manager's components.
    void SetGPUPOutputs(std::unordered_map<GPUPOutputs, DataLine> & outputs);


private:

    HGPComponentPtr(3) worldPosition;
    HGPComponentPtr(1) rotation;
    HGPComponentPtr(2) size;
    HGPComponentPtr(4) color;
    HGPComponentPtr(1) duration;

    std::string name;
    static unsigned int nameCounter;

    DataNodePtr timeLerpComponentNode;
    DataLine timeLerp;
};