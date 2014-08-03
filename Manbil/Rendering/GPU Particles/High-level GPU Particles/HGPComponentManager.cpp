#include "HGPComponentManager.h"



unsigned int HGPComponentManager::nameCounter = 0;


HGPComponentManager::HGPComponentManager(UniformDictionary & params, std::string _name = "")
    : Params(params), name(_name)
{
    if (name.empty())
    {
        name = "HGP" + std::to_string(nameCounter);
        nameCounter += 1;
    }

    HGPComponentManager & thisM = *this;
    worldPosition = HGPComponentPtr(3)(new ConstantHGPComponent<3>(VectorF((unsigned int)3), thisM));
    rotation = HGPComponentPtr(1)(new ConstantHGPComponent<1>(VectorF((unsigned int)1), thisM));
    size = HGPComponentPtr(2)(new ConstantHGPComponent<2>(VectorF((unsigned int)2, 1.0f), thisM));
    color = HGPComponentPtr(4)(new ConstantHGPComponent<4>(VectorF((unsigned int)4, 1.0f), thisM));
    duration = HGPComponentPtr(1)(new ConstantHGPComponent<1>(VectorF((unsigned int)1, 5.0f), thisM));
}

void HGPComponentManager::SetWorldPosition(HGPComponentPtr(3) newWorldPos) { SwapOutComponent(worldPosition, newWorldPos); }
void HGPComponentManager::SetRotation(HGPComponentPtr(1) newRotation) { SwapOutComponent(rotation, newRotation); }
void HGPComponentManager::SetSize(HGPComponentPtr(2) newSize) { SwapOutComponent(size, newSize); }
void HGPComponentManager::SetColor(HGPComponentPtr(4) newColor) { SwapOutComponent(color, newColor); }
void HGPComponentManager::SetDuration(HGPComponentPtr(1) newDuration)
{
    SwapOutComponent(duration, newDuration);
    timeLerpComponentNode = DataNodePtr(new GetLerpComponentNode(DataLine(0.0f), duration->GetComponentOutput(),
                                                                 HGPGlobalData::ParticleElapsedTime,
                                                                 name + "_timeLerpCalc"));
    timeLerp = DataLine(name + "_timeLerpCalc");
}

void HGPComponentManager::SwapOutComponent(HGPComponentPtr(1) oldC, HGPComponentPtr(1) newC)
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
void HGPComponentManager::SwapOutComponent(HGPComponentPtr(2) oldC, HGPComponentPtr(2) newC)
{
    if (size.get() == oldC.get())
        size = newC;
    else size->SwapOutSubComponent(oldC, newC);

    worldPosition->SwapOutSubComponent(oldC, newC);
    rotation->SwapOutSubComponent(oldC, newC);
    color->SwapOutSubComponent(oldC, newC);
    duration->SwapOutSubComponent(oldC, newC);
}
void HGPComponentManager::SwapOutComponent(HGPComponentPtr(3) oldC, HGPComponentPtr(3) newC)
{
    if (worldPosition.get() == oldC.get())
        worldPosition = newC;
    else worldPosition->SwapOutSubComponent(oldC, newC);

    size->SwapOutSubComponent(oldC, newC);
    rotation->SwapOutSubComponent(oldC, newC);
    color->SwapOutSubComponent(oldC, newC);
    duration->SwapOutSubComponent(oldC, newC);
}
void HGPComponentManager::SwapOutComponent(HGPComponentPtr(4) oldC, HGPComponentPtr(4) newC)
{
    if (color.get() == oldC.get())
        color = newC;
    else color->SwapOutSubComponent(oldC, newC);

    size->SwapOutSubComponent(oldC, newC);
    rotation->SwapOutSubComponent(oldC, newC);
    worldPosition->SwapOutSubComponent(oldC, newC);
    duration->SwapOutSubComponent(oldC, newC);
}


void HGPComponentManager::Initialize(void)
{
    worldPosition->InitializeComponent();
    rotation->InitializeComponent();
    size->InitializeComponent();
    color->InitializeComponent(); 
    duration->InitializeComponent();
}
void HGPComponentManager::Update(float elapsedTime)
{
    worldPosition->UpdateComponent();
    rotation->UpdateComponent();
    size->UpdateComponent();
    color->UpdateComponent();
    duration->UpdateComponent();

    Params.FloatUniforms[HGPGlobalData::ParticleElapsedTimeUniformName].Value[0] += elapsedTime;
}

void HGPComponentManager::SetGPUPOutputs(std::unordered_map<GPUPOutputs, DataLine> & outputs)
{
    outputs[GPUPOutputs::GPUP_WORLDPOSITION] = worldPosition->GetComponentOutput();
    outputs[GPUPOutputs::GPUP_QUADROTATION] = rotation->GetComponentOutput();
    outputs[GPUPOutputs::GPUP_SIZE] = size->GetComponentOutput();
    outputs[GPUPOutputs::GPUP_COLOR] = color->GetComponentOutput();
}