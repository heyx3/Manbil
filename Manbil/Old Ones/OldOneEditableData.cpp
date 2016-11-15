#include "OldOneEditableData.h"

#include "../Editor/EditorObjects.h"


void OldOneEditableData::WriteData(DataWriter* writer) const
{
    writer->WriteFloat(Shape, "Shape");
    writer->WriteFloat(Roundness, "Roundness");
    writer->WriteFloat(SpikynessLeftSide, "Spikyness Left Side");
    writer->WriteFloat(SpikynessRightSide, "Spikyness Right Side");
    writer->WriteFloat(Quality, "Quality");
    writer->WriteFloat(Angriness, "Angriness");
}
void OldOneEditableData::ReadData(DataReader* reader)
{
    reader->ReadFloat(Shape);
    reader->ReadFloat(Roundness);
    reader->ReadFloat(SpikynessLeftSide);
    reader->ReadFloat(SpikynessRightSide);
    reader->ReadFloat(Quality);
    reader->ReadFloat(Angriness);
}

std::string OldOneEditableData::BuildEditorElements(std::vector<EditorObjectPtr>& outElements,
                                                    EditorMaterialSet& materialSet)
{
    EditorObjectPtr temp;

    //Create sliding bars for each float that can be edited.
    //The callback for the sliding bar as it changes just sets this instance's value.
    #define MAKE_FLOAT_BAR(itemName, minName, maxName, defaultVal, lerpPow, textName) \
        temp.reset(); \
        temp = EditorObjectPtr(new SlidingBarFloat<void*>( \
                                    minName, maxName, Vector2f(0.0f, 20.0f), \
                                    EditorObject::DescriptionData(#itemName, false, \
                                                                  20.0f, 1500U), \
                                    [](GUISlider* slider, float newVal, void* p) \
                                    { \
                                        *((float*)p) = newVal; \
                                    }, \
                                    defaultVal, lerpPow, &itemName)); \
        \
        outElements.push_back(temp);

    MAKE_FLOAT_BAR(Shape,              1.0f, 10.0f,    1.0f,   1.0f,    "Shape");
    //MAKE_FLOAT_BAR(Roundness,        1.0f, 9.0f,     0.0f,   1.0f,    "Roundness");
    MAKE_FLOAT_BAR(SpikynessLeftSide, -1.0f, -0.014f,  0.0f,   1.0f,    "Spikyness Left Side");
    MAKE_FLOAT_BAR(SpikynessRightSide, 1.0f, 0.014f,   0.0f,   1.0f,    "Spikyness Right Side");
    //MAKE_FLOAT_BAR(Quality,          0.0f, 1.0f,     0.5f,   1.0f,    "Quality");
    MAKE_FLOAT_BAR(Angriness,          0.0f, 1.0f,     0.0f,   6.0f,    "Angriness");

    return "";
}