#pragma once

#include "../IO/DataSerialization.h"
#include "../Editor/IEditable.h"


//Handles the demo data that the user can toy with.
class OldOneEditableData : public ISerializable, IEditable
{
public:

    //"Shape" has a dramaic effect on the fractal's shape.
    //A shape of 1.0 will generally be a very simple, spherical shape.
    //IMPL NOTE: Shape is Power.
    float Shape = 8.0f;

    //The roundness of the fractal's base.
    //IMPL NOTE: z += pow(pos, vec3(roundness))
    float Roundness = 1.0f;

    //The spikiness of each side of the fractal.
    //IMPL NOTE: z = zr * vec3(..., clamp(sin(phi) * sin(theta), SpikynessLeft, SpikynessRight), ...)
    float SpikynessLeftSide = -0.9f,
          SpikynessRightSide = 0.9f;

    //The quality of the fractal, from 0 to 1.
    //IMPL NOTE: vary # raymarch iterations and epsilon.
    float Quality = 0.5f;

    //A measure of how "angry" and "scary" the fractal is supposed to be, from 0 to 1.
    //IMPL NOTE: vary vibration, spikyness, and color.
    float Angriness = 0.0f;


    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;

    virtual std::string BuildEditorElements(std::vector<EditorObjectPtr>& outElements,
                                            EditorMaterialSet& materialSet) override;
};