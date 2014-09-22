#pragma once

#include "../Rendering/GUI/GUIMaterials.h"
#include "../Rendering/GUI/GUI Elements/GUIFormattedPanel.h"
#include "../Rendering/GUI/GUI Elements/GUITextBox.h"
#include "../Rendering/GUI/GUI Elements/GUISlider.h"
#include "../Rendering/GUI/GUI Elements/GUISelectionBox.h"
#include "../Rendering/GUI/GUI Elements/GUICheckbox.h"

#include "EditorObject.h"



#pragma region Definitions for parsed text boxes

//The type of (simple) data being parsed from the text box,
//    the std function to parse it (like std::stof),
//    and the function to convert the data to a string.
template<typename DataType, typename std_StringToDataType, typename DataTypeToString>
//A value entered into a text box and parsed as a data type.
struct _TextBoxValue : public EditorObject
{
public:

    //The starting value of this text box.
    DataType StartingValue;

    Vector2u BoxDimensions;

    void(*OnValueChanged)(GUITextBox * textBox, DataType newVal, void* pData) = 0;
    void* OnValueChanged_Data = 0;


    _TextBoxValue(DataType startingValue, Vector2u boxDimensions,
                  void(*onValueChanged)(GUITextBox * textBox, DataType newVal, void* pData) = 0,
                  void* onValueChanged_pData = 0)
        : StartingValue(startingValue), OnValueChanged(onValueChanged),
          BoxDimensions(boxDimensions), OnValueChanged_Data(onValueChanged_pData) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override
    {
        activeGUIElement = GUIElementPtr(0);

        if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID,
                                                          (unsigned int)((float)BoxDimensions.x /
                                                                         materialSet.TextScale.x),
                                                          materialSet.TextRenderSpaceHeight, false,
                                                          TextureSampleSettings2D(FT_NEAREST, WT_CLAMP)))
        {
            ErrorMsg = "Error creating text render slot: " + materialSet.TextRender.GetError();
            return false;
        }

        TextRenderer::FontSlot slot(materialSet.FontID,
                                    materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);
        GUITexture boxBackground(materialSet.GetStaticMatParams(&materialSet.TextBoxBackgroundTex),
                                 &materialSet.TextBoxBackgroundTex,
                                 materialSet.GetStaticMaterial(&materialSet.TextBoxBackgroundTex),
                                 false, materialSet.AnimateSpeed);
        GUILabel boxContents(materialSet.StaticMatGreyParams, &materialSet.TextRender,
                             slot, materialSet.StaticMatGrey, materialSet.AnimateSpeed,
                             GUILabel::HO_LEFT, GUILabel::VO_CENTER);
        boxContents.SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
        boxContents.ScaleBy(materialSet.TextScale);
        GUITextBox * box = new GUITextBox(boxBackground, boxBackground, GUITexture(), boxContents,
                                          (float)BoxDimensions.x, (float)BoxDimensions.y, true,
                                          materialSet.StaticMatGreyParams, materialSet.AnimateSpeed);
        box->Cursor.SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
        //Try setting the initial value of the box.
        ErrorMsg.clear();
        ErrorMsg = box->SetText(DataTypeToString()(StartingValue));
        if (!ErrorMsg.empty())
        {
            delete box;
            return false;
        }

        //When new text is entered, try to parse it.
        box->OnTextChanged = [](GUITextBox* tBox, void* pData)
        {
            _TextBoxValue<DataType, std_StringToDataType, DataTypeToString> * tbv =
                (_TextBoxValue<DataType, std_StringToDataType, DataTypeToString> *)pData;

            //Try parsing the value.
            try
            {
                DataType val = std_StringToDataType()(tBox->GetText());

                //Raise the correct event.
                if (tbv->OnValueChanged != 0)
                    tbv->OnValueChanged(tBox, val, tbv->OnValueChanged_Data);
            }
            //If it was out of range, use the default value.
            catch (std::out_of_range rng)
            {
                if (tbv->OnValueChanged != 0)
                    tbv->OnValueChanged(tBox, tbv->StartingValue, tbv->OnValueChanged_Data);
            }
            //If it cannot be parsed, use the default value.
            catch (std::invalid_argument rng)
            {
                if (tbv->OnValueChanged != 0)
                    tbv->OnValueChanged(tBox, tbv->StartingValue, tbv->OnValueChanged_Data);
            }
        };
        box->OnTextChanged_Data = this;
        
        activeGUIElement = GUIElementPtr(box);
        return true;
    }
};

//Classes that are used for defining different types of text boxes.
class std_StrToFl
{
public: float operator()(const std::string & str) { return std::stof(str); }
};
class std_StrToInt
{
public: int operator()(const std::string & str) { return std::stoi(str); }
};
class std_StrToUInt
{
public: unsigned int operator()(const std::string & str) { return (unsigned int)std::stoul(str); }
};
template<typename DataType>
class PrimitiveToStr
{
public: std::string operator()(DataType t) { return std::to_string(t); }
};

#pragma endregion

//A text box for entering floats.
typedef _TextBoxValue<float, std_StrToFl, PrimitiveToStr<float>> TextBoxFloat;
//A text box for entering signed ints.
typedef _TextBoxValue<int, std_StrToInt, PrimitiveToStr<int>> TextBoxInt;
//A text box for entering unsigned ints.
typedef _TextBoxValue<unsigned int, std_StrToUInt, PrimitiveToStr<unsigned int>> TextBoxUInt;



#pragma region Definitions for sliding bars

//The data type being interpolated,
//   and a function of signature "DataType Lerp(DataType min, DataType max, float t)".
template<typename DataType, typename InterpretValue>
//A sliding bar that interpolates between two values.
class _SlidingBarValue : public EditorObject
{
public:

    //The range of the slider.
    DataType MinValue, MaxValue;
    //The default lerp value of the slider.
    float DefaultLerpValue;

    //The lerp value between 0 and 1 is raised to this power.
    float LerpPow;

    void(*OnValueChanged)(GUISlider * slider, DataType newVal, void* pData) = 0;
    void* OnValueChanged_Data = 0;


    _SlidingBarValue(DataType min, DataType max,
                     void(*onValChanged)(GUISlider * slider, DataType newVal, void* pData) = 0,
                     float defaultValue = 0.5f, float lerpPow = 1.0f, void* onValChanged_Data = 0)
        : MinValue(min), MaxValue(max), DefaultLerpValue(defaultValue), LerpPow(lerpPow),
          OnValueChanged(onValChanged), OnValueChanged_Data(onValChanged_Data) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override
    {
        MTexture2D *barTex = &materialSet.SliderBarTex,
                   *nubTex = &materialSet.SliderNubTex;
        GUISlider * slider = new GUISlider(materialSet.StaticMatGreyParams,
                                           materialSet.GetStaticMatParams(barTex),
                                           materialSet.GetAnimatedMatParams(nubTex),
                                           barTex, nubTex, materialSet.GetStaticMaterial(barTex),
                                           materialSet.GetAnimatedMaterial(nubTex),
                                           materialSet.SliderBarScale, materialSet.SliderNubScale, true, false,
                                           materialSet.AnimateSpeed);
        slider->Value = DefaultLerpValue;
        slider->OnValueChanged = [](GUISlider * slid, Vector2f mouse, void* pData)
        {
            _SlidingBarValue<DataType, InterpretValue> * thisP =
                (_SlidingBarValue<DataType, InterpretValue>*)pData;

            if (thisP->OnValueChanged != 0)
            {
                thisP->OnValueChanged(slid,
                                      InterpretValue(thisP->MinValue, thisP->MaxValue,
                                                     powf(slid->Value, thisP->LerpPow)),
                                      slid->OnValueChanged_pData);
            }
        };
        slider->OnValueChanged_pData = this;

        activeGUIElement = GUIElement(slider);
        return true;
    }
};

class InterpretValue_Float
{
public: float operator()(float min, float max, float t) { return BasicMath::Lerp(min, max, t); }
};
class InterpretValue_Int
{
public:
    int operator()(int min, int max, float t)
    {
        //Use higher precision to make sure the whole range is correctly covered.

        double range = (double)max - (double)min;

        double val = min + (range * (double)t);
        return BasicMath::RoundToInt(val);
    }
};
class InterpretValue_UInt
{
public:
    unsigned int operator()(unsigned int min, unsigned int max, float t)
    {
        //Use higher precision to make sure the whole range is correctly covered.

        double range = (double)max - (double)min;

        double val = min + (range * (double)t);
        assert(val > -0.5f);
        return BasicMath::RoundToUInt(val);
    }
};

#pragma endregion

//A sliding bar for a floating-point range.
typedef _SlidingBarValue<float, InterpretValue_Float> SlidingBarFloat;
//A sliding bar for a signed integer range.
typedef _SlidingBarValue<int, InterpretValue_Int> SlidingBarInt;
//A sliding bar for an unsigned integer range.
typedef _SlidingBarValue<unsigned int, InterpretValue_UInt> SlidingBarUInt;



//A value that can be toggled on or off.
struct CheckboxValue : public EditorObject
{
public:

    //The default value of the checkbox.
    bool DefaultValue;

    //Raised when the checkbox generated by this object is clicked.
    //The value of "pData" is equal to the value of this instance's "OnBoxClicked_Data" field.
    void(*OnBoxClicked)(GUICheckbox * checkbox, void* pData) = 0;
    void* OnBoxClicked_Data = 0;

    CheckboxValue(bool defaultVal = false)
        : DefaultValue(defaultVal) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
};


//A value that is one of several possible string values.
struct DropdownValues : public EditorObject
{
public:

    //The initial items in the dropdown menu.
    std::vector<std::string> Items;

    //Raised every Update() step for the dropdown box generated by this instance.
    //The value of "pData" is equal to the value of this instance's "OnUpdate_Data" field.
    void(*OnUpdate)(GUISelectionBox * dropdownBox, void* pData) = 0;
    void* OnUpdate_Data = 0;

    //Raised every time the user selects an option from this dropdown box.
    //The value of "pData" is equal to the value of this instance's "OnSelected_Data" field.
    void(*OnSelected)(GUISelectionBox* dropdownBox, const std::string & item, unsigned int index, void* pData) = 0;
    void* OnSelected_Data = 0;


    DropdownValues(const std::vector<std::string> items,
                   void(*onSelected)(GUISelectionBox* dropdownBox, const std::string & item,
                                     unsigned int index, void* pData) = 0,
                   void* onSelected_Data = 0,
                   void(*onUpdate)(GUISelectionBox* dropdownBox, void* pData) = 0,
                   void* onUpdate_Data = 0)
        : Items(items), OnUpdate(onUpdate), OnUpdate_Data(onUpdate_Data),
          OnSelected(onSelected), OnSelected_Data(onSelected_Data) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
};



//A string value entered into a text box by the user.
struct TextBoxString : public EditorObject
{
public:

    //The starting value of this text box.
    std::string StartingValue;

    Vector2u BoxDimensions;

    void(*OnValueChanged)(GUITextBox * textBox, void* pData) = 0;
    void* OnValueChanged_Data = 0;


    TextBoxString(std::string startingValue, Vector2u boxDimensions,
                  void(*onValueChanged)(GUITextBox * textBox, void* pData) = 0,
                  void* onValueChanged_pData = 0)
        : StartingValue(startingValue), OnValueChanged(onValueChanged),
          BoxDimensions(boxDimensions), OnValueChanged_Data(onValueChanged_pData) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
};



//A clickable button with text.
struct EditorButton : public EditorObject
{
public:

    std::string Text;

    Vector2f ButtonSize;

    //Raised when this button is clicked. "pData" is equal to this instance's "OnClick_Data" field.
    void(*OnClick)(GUITexture * clicked, Vector2f localMouse, void* pData) = 0;
    void* OnClick_Data = 0;


    EditorButton(std::string text, Vector2f size,
                 void(*onClick)(GUITexture* clicked, Vector2f localMouse, void* pData) = 0,
                 void* onClick_Data = 0)
        : Text(text), ButtonSize(size), OnClick(onClick), OnClick_Data(onClick_Data),
          buttonTex(0), buttonLabel(0) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;


private:

    GUIElementPtr buttonTex, buttonLabel;
};