#pragma once

#include "../Rendering/GUI/GUIMaterials.h"
#include "../Rendering/GUI/GUI Elements/GUIFormattedPanel.h"
#include "../Rendering/GUI/GUI Elements/GUITextBox.h"
#include "../Rendering/GUI/GUI Elements/GUISlider.h"
#include "../Rendering/GUI/GUI Elements/GUISelectionBox.h"
#include "../Rendering/GUI/GUI Elements/GUICheckbox.h"

#include "EditorPanel.h"
#include "IEditable.h"



#pragma region Definitions for parsed text boxes

//The three template arguments are:
//1) The type of (simple) data being parsed from the text box,
//2) The std function to parse it (like std::stof), and
//3) The function to convert the data to a string.
template<typename DataType, typename std_StringToDataType, typename DataTypeToString>
//A value entered into a text box and parsed as a data type.
struct _TextBoxValue : public EditorObject
{
public:

    //The starting value of this text box.
    DataType StartingValue;

    float BoxWidth;

    void(*OnValueChanged)(GUITextBox * textBox, DataType newVal, void* pData) = 0;
    void* OnValueChanged_Data = 0;


    _TextBoxValue(DataType startingValue, float boxWidth, Vector2f offset = Vector2f(0.0f, 0.0f),
                  EditorObject::DescriptionData description = EditorObject::DescriptionData(),
                  void(*onValueChanged)(GUITextBox * textBox, DataType newVal, void* pData) = 0,
                  void* onValueChanged_pData = 0)
        : StartingValue(startingValue), OnValueChanged(onValueChanged),
          BoxWidth(boxWidth), OnValueChanged_Data(onValueChanged_pData),
          EditorObject(description, offset) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override
    {
        activeGUIElement = GUIElementPtr(0);

        if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID,
                                                          (unsigned int)(BoxWidth / materialSet.TextScale.x),
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
        boxBackground.SetScale(Vector2f(BoxWidth / (float)materialSet.TextBoxBackgroundTex.GetWidth(),
                                        materialSet.TextRenderSpaceHeight * materialSet.TextScale.y /
                                            (float)boxBackground.GetTex()->GetHeight()));
        
        GUITexture boxCursor(boxBackground);
        boxCursor.SetScale(Vector2f(materialSet.TextBoxCursorWidth,
                                    boxBackground.GetBounds().GetYSize() /
                                        (float)boxCursor.GetTex()->GetHeight()));
        GUILabel boxContents(materialSet.StaticMatTextParams, &materialSet.TextRender,
                             slot, materialSet.StaticMatText, materialSet.AnimateSpeed,
                             GUILabel::HO_LEFT, GUILabel::VO_CENTER);
        boxContents.SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
        boxContents.ScaleBy(materialSet.TextScale);
        GUITextBox * box = new GUITextBox(boxBackground, boxCursor, boxContents,
                                          true, materialSet.AnimateSpeed);
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
        
        if (DescriptionLabel.Text.empty())
        {
            activeGUIElement = GUIElementPtr(box);
        }
        else
        {
            activeGUIElement = AddDescription(materialSet, GUIElementPtr(box));
            if (activeGUIElement.get() == 0)
                return false;
        }

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
class StrToStr
{
public: std::string operator()(const std::string & str) { return str; }
};

#pragma endregion

//A text box for entering floats.
typedef _TextBoxValue<float, std_StrToFl, PrimitiveToStr<float>> TextBoxFloat;
//A text box for entering signed ints.
typedef _TextBoxValue<int, std_StrToInt, PrimitiveToStr<int>> TextBoxInt;
//A text box for entering unsigned ints.
typedef _TextBoxValue<unsigned int, std_StrToUInt, PrimitiveToStr<unsigned int>> TextBoxUInt;
typedef _TextBoxValue<std::string, StrToStr, StrToStr> TextBoxString;



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


    _SlidingBarValue(DataType min, DataType max, Vector2f offset = Vector2f(0.0f, 0.0f),
                     EditorObject::DescriptionData description = EditorObject::DescriptionData(),
                     void(*onValChanged)(GUISlider * slider, DataType newVal, void* pData) = 0,
                     float defaultValue = 0.5f, float lerpPow = 1.0f, void* onValChanged_Data = 0)
        : MinValue(min), MaxValue(max), DefaultLerpValue(defaultValue), LerpPow(lerpPow),
          OnValueChanged(onValChanged), OnValueChanged_Data(onValChanged_Data),
          EditorObject(description, offset) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override
    {
        MTexture2D *barTex = &materialSet.SliderBarTex,
                   *nubTex = &materialSet.SliderNubTex;
        GUITexture guiBar(materialSet.GetStaticMatParams(barTex), barTex,
                          materialSet.GetStaticMaterial(barTex), false),
                   guiNub(materialSet.GetAnimatedMatParams(nubTex), nubTex,
                          materialSet.GetAnimatedMaterial(nubTex), true);
        guiBar.ScaleBy(materialSet.SliderBarScale);
        guiNub.ScaleBy(materialSet.SliderNubScale);
        GUISlider * slider = new GUISlider(materialSet.StaticMatGreyParams, guiBar, guiNub,
                                           true, false, materialSet.AnimateSpeed);
        slider->Value = DefaultLerpValue;
        slider->OnValueChanged = [](GUISlider * slid, Vector2f mouse, void* pData)
        {
            _SlidingBarValue<DataType, InterpretValue> * thisP =
                (_SlidingBarValue<DataType, InterpretValue>*)pData;

            if (thisP->OnValueChanged != 0)
            {
                thisP->OnValueChanged(slid,
                                      InterpretValue()(thisP->MinValue, thisP->MaxValue,
                                                       powf(slid->Value, thisP->LerpPow)),
                                      thisP->OnValueChanged_Data);
            }
        };
        slider->OnValueChanged_pData = this;

        if (DescriptionLabel.Text.empty())
        {
            activeGUIElement = GUIElementPtr(slider);
            return true;
        }
        else
        {
            activeGUIElement = AddDescription(materialSet, GUIElementPtr(slider));
            if (activeGUIElement.get() == 0)
                return false;
            return true;
        }
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

    CheckboxValue(EditorObject::DescriptionData description = EditorObject::DescriptionData(),
                  Vector2f offset = Vector2f(), bool defaultVal = false)
        : DefaultValue(defaultVal), EditorObject(description, offset) { }

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


    DropdownValues(const std::vector<std::string> items, Vector2f offset = Vector2f(0.0f, 0.0f),
                   EditorObject::DescriptionData description = EditorObject::DescriptionData(),
                   void(*onSelected)(GUISelectionBox* dropdownBox, const std::string & item,
                                     unsigned int index, void* pData) = 0,
                   void* onSelected_Data = 0,
                   void(*onUpdate)(GUISelectionBox* dropdownBox, void* pData) = 0,
                   void* onUpdate_Data = 0)
        : Items(items), OnUpdate(onUpdate), OnUpdate_Data(onUpdate_Data),
          OnSelected(onSelected), OnSelected_Data(onSelected_Data),
          EditorObject(description, offset) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
};



//Represents data for a single button in an editor.
//If no text is used, no text label object will even be generated.
//If the texture is set to 0, the default button texture in the EditorMaterialSet will be used.
struct EditorButtonData
{
public:

    std::string Text = "";
    MTexture2D* Tex = 0;
    Vector2f ButtonScale = Vector2f(1.0f, 1.0f);

    void(*OnClick)(GUITexture* clicked, Vector2f localMouse, void* pData) = 0;
    void* OnClick_pData = 0;

    EditorButtonData(std::string text = "", MTexture2D* tex = 0, Vector2f buttonScale = Vector2f(1.0f, 1.0f),
                     void(*onClick)(GUITexture* clicked, Vector2f localMouse, void* pData) = 0,
                     void* onClick_pData = 0)
        : Text(text), Tex(tex), ButtonScale(buttonScale), OnClick(onClick), OnClick_pData(onClick_pData) { }
};

//A clickable button with optional text.
//If the button texture is set to 0,
//   the default button texture in the EditorMaterialSet will be used.
struct EditorButton : public EditorObject
{
public:

    std::string Text;
    MTexture2D * TexToUse;

    Vector2f ButtonSize;

    //Raised when this button is clicked. "pData" is equal to this instance's "OnClick_Data" field.
    void(*OnClick)(GUITexture * clicked, Vector2f localMouse, void* pData) = 0;
    void* OnClick_Data = 0;


    EditorButton(std::string text, Vector2f size, MTexture2D* buttonTex = 0, Vector2f offset = Vector2f(),
                 EditorObject::DescriptionData description = EditorObject::DescriptionData(),
                 void(*onClick)(GUITexture* clicked, Vector2f localMouse, void* pData) = 0,
                 void* onClick_Data = 0)
        : Text(text), ButtonSize(size), TexToUse(buttonTex), OnClick(onClick), OnClick_Data(onClick_Data),
          EditorObject(description, offset) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
};


//A horizontal row of clickable buttons with optional text.
//If a button's texture is set to 0,
//   the default button texture in the EditorMaterialSet will be used.
struct EditorButtonList : public EditorObject
{
public:


    EditorButtonList(const std::vector<EditorButtonData> & buttons,
                     DescriptionData description = DescriptionData(),
                     float spaceBetweenButtons = 0.0f, Vector2f spaceAfter = Vector2f())
        : buttonData(buttons), buttonSpacing(spaceBetweenButtons),
          EditorObject(description, spaceAfter) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;


private:

    std::vector<EditorButtonData> buttonData;
    float buttonSpacing;
};



//A label in an editor panel.
//NOTE: This object ignores the description label.
struct EditorLabel : public EditorObject
{
public:

    std::string Text;
    unsigned int TextRenderSpaceWidth;

    EditorLabel(const std::string & text, unsigned int textRenderSpaceWidth,
                EditorObject::DescriptionData description = EditorObject::DescriptionData(),
                Vector2f offset = Vector2f())
        : Text(text), TextRenderSpaceWidth(textRenderSpaceWidth), EditorObject(description, offset) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
};



//Displays a texture.
struct EditorImage : public EditorObject
{
public:

    MTexture2D* Tex;
    Vector2f Scale;
    
    EditorImage(MTexture2D* tex, EditorObject::DescriptionData description = EditorObject::DescriptionData(),
                Vector2f scale = Vector2f(1.0f, 1.0f), Vector2f offset = Vector2f())
        : Tex(tex), Scale(scale), EditorObject(description, offset) { }

    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
};



//A "collapsible" panel.
struct EditorCollapsibleBranch : public EditorObject
{
public:

    EditorCollapsibleBranch(GUIElementPtr _innerElement = GUIElementPtr(0),
                            float _panelIndent = 20.0f, std::string titleBarName = "",
                            Vector2f spaceAfter = Vector2f())
        : innerElement(_innerElement), barOnly(0), fullPanel(0), panelIndent(_panelIndent),
          EditorObject(DescriptionData(titleBarName), spaceAfter) { }


    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override;
    virtual bool Update(float elapsed, Vector2f panelRelativeMousePos) override;

    GUIElementPtr GetInnerElement(void) const { return innerElement; }
    GUIElementPtr GetTitleBar(void) const { return barOnly; }
    GUIElementPtr GetFullPanel(void) const { return fullPanel; }

    void Toggle(void);


private:

    GUIElementPtr innerElement;
    GUIElementPtr barOnly, fullPanel;
    float panelIndent;
    bool didActiveElementChange = false;
};



//The type of collection element that needs a factory.
//Must have a default constructor.
template<typename ElementType>
//Default factory for elements of an EditorCollection.
//Calls the element type's default constructor.
class DefaultElementFactory
{
public:
    std::shared_ptr<ElementType> operator()(void) { return std::shared_ptr<ElementType>(new ElementType()); }
};


//"ElementType" is the type of element being edited in this collection;
//    it must inherit from IEditable.
//"ElementFactory" is a function of type "std::shared_ptr<ElementType> MakeElement(void)".
template<typename ElementType, typename ElementFactory = DefaultElementFactory<ElementType>>
//A collection of identical editor panels that can be added to/removed from.
struct EditorCollection : public EditorObject
{
public:


    EditorCollection(std::string _name = "", float _panelIndent = 20.0f)
        : EditorObject(DescriptionData(_name)), panelIndent(_panelIndent) { }


    const std::vector<std::shared_ptr<ElementType>> & GetElements(void) const { return elements; }

    //Gets the index of the given element in this collection.
    //Returns the size of this collection if it isn't found.
    unsigned int GetIndex(ElementType* toFind) const
    {
        unsigned int i;
        for (i = 0; i < elements.size(); ++i)
            if (elements[i].get() == toFind)
                break;
        return i;
    }

    //Adds an element from the end of this collection.
    //Returns an error message if this failed, or the empty string if everything went fine.
    std::string AddElement(void)
    {
        //Add the element to the collection.
        std::shared_ptr<ElementType> newElement = ElementFactory()();
        elements.insert(elements.end(), newElement);


        //Build the element's editor panel.

        std::vector<EditorObjectPtr> elementEditorObjs;
        newElement->BuildEditorElements(elementEditorObjs, *pMaterialSet);
        EditorPanel* elementPanel = new EditorPanel(*pMaterialSet, 0.0f, 0.0f);
        std::string err = elementPanel->AddObjects(elementEditorObjs);

        if (!err.empty()) return "Error creating editor panel for the new element: " + err;


        //Add this new editor, wrapped in a collapsible title bar, to this object's collection editor panel.
        //Add it to the end of the panel's object list but just behind the add/remove buttons.

        EditorObjectPtr elementTitleBar(new EditorCollapsibleBranch(GUIElementPtr(elementPanel),
                                                                    panelIndent, "", Vector2f()));
        err = collectionPanel->AddObject(elementTitleBar, collectionPanel->GetObjects().size() - 1);

        if (!err.empty()) return "Error creating collapsible editor panel for the new element: " + err;


        didActiveElementChange = true;
        return "";
    }
    //Removes the given element from this collection.
    void RemoveElement(void)
    {
        if (elements.size() == 0) return;

        //Remove the element from the collection.
        elements.erase(elements.end() - 2);

        //Remove the element from the editor panel.
        //There is one editor object in the panel after the last element in the collection.
        collectionPanel->RemoveObject(*(collectionPanel->GetObjects().end() - 2));

        didActiveElementChange = true;
    }


    virtual bool InitGUIElement(EditorMaterialSet & materialSet) override
    {
        pMaterialSet = &materialSet;


        //Build an editor panel for this collection.

        collectionPanel = new EditorPanel(materialSet, 0.0f, 0.0f);

        //Create two buttons: one that adds an element to the end of the collection,
        //    and one that removes an element from the end of the collection.
        std::vector<EditorButtonData> buttonDats;
        buttonDats.insert(buttonDats.end(),
                          EditorButtonData("", &materialSet.AddToCollectionTex, Vector2f(1.0f, 1.0f),
                                           [](GUITexture* clicked, Vector2f localMouse, void* pData)
                                              {
                                                  ((EditorCollection*)pData)->AddElement();
                                              },
                                           this));
        buttonDats.insert(buttonDats.end(),
                          EditorButtonData("", &materialSet.DeleteFromCollectionTex, Vector2f(1.0f, 1.0f),
                                           [](GUITexture* clicked, Vector2f localMouse, void* pData)
                                              {
                                                  ((EditorCollection*)pData)->RemoveElement();
                                              },
                                           this));
        collectionPanel->AddObject(EditorObjectPtr(new EditorButtonList(buttonDats)));


        //Build a small outer editor panel that just has a collapsible title bar wrapping the collection editor panel.
        EditorPanel* outerPanel = new EditorPanel(materialSet, 0.0f, 0.0f);
        outerPanel->AddObject(EditorObjectPtr(new EditorCollapsibleBranch(GUIElementPtr(collectionPanel),
                                                                          panelIndent,
                                                                          DescriptionLabel.Text)));
        activeGUIElement = GUIElementPtr(outerPanel);

        return true;
    }
    virtual bool Update(float elapsed, Vector2f panelRelativeMousePos) override
    {
        bool b = didActiveElementChange;
        didActiveElementChange = false;
        return b;
    }


private:

    std::vector<std::shared_ptr<ElementType>> elements;

    bool didActiveElementChange = false;
    float panelIndent;


    //After initialization, hold on to the material set so that the editor panels for new elements can be created.
    EditorMaterialSet* pMaterialSet;
    //Also hold on to the editor panel holding the collection so that it can be added to/removed from.
    EditorPanel* collectionPanel;
};