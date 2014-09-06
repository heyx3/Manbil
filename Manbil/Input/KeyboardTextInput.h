#pragma once

#include <string>
#include "../Math/BasicMath.h"



//Allows the user to enter text via the keyboard.
class KeyboardTextInput
{
public:

    //Raised when the text changes because of user input.
    //The value of "pData" is the value of this instance's "OnTextChanged_Data" field.
    void(*OnTextChanged)(KeyboardTextInput * thisText, void* pData) = 0;
    void* OnTextChanged_Data = 0;

    //Raised when the user presses the Enter key.
    //The value of "pData" is the value of this instance's "OnEnterKey_Data" field.
    void(*OnEnterKey)(KeyboardTextInput * thisText, void* pData) = 0;
    void* OnEnterKey_Data = 0;

    //Raised when the user moves the cursor with a keyboard key (e.x. the arrow keys).
    //Does not include events that modified the text (e.x. adding text behind the cursor).
    //The value of "pData" is the value of this instance's "OnCursorMoved_Data" field.
    void(*OnCursorMoved)(KeyboardTextInput * thisText, int moveAmount, void* pData) = 0;
    void* OnCursorMoved_Data = 0;


    //The current position of the cursor. Must stay within text bounds.
    unsigned int CursorPos = 0;
    //The length of time a key must be held down before it starts repeating.
    float KeyRepeatWait = 0.5f;
    //If true the Enter key, along with raising the "OnEnterKey" event, will add '\n' to the text.
    bool EnterKeyAddsText = false;


    KeyboardTextInput(std::string startingText = "");
    //This class takes up a nontrivial amount of space, so it shouldn't be wantonly copied around.
    KeyboardTextInput(const KeyboardTextInput & cpy) = delete;


    void Update(float elapsedTime);
    
    //Inserts the given char at the given position in the text.
    void InsertChar(unsigned int pos, char value);
    //Removes the char at the given position in the text and returns its value.
    //Returns 0 if the given pos is beyond the size of the text.
    char RemoveChar(unsigned int pos);
    //Inserts the given text at the given position in the text.
    void InsertText(unsigned int pos, const std::string & value);
    //Removes the given substring and returns its value.
    //If the given pos is beyond the size of the text, returns an empty string.
    //If the end of the given substring is beyond the size of the text, it will be trimmed.
    std::string RemoveText(unsigned int pos, unsigned int length);
    //Clears the text from this input. Optionally raises the "OnTextChanged" event.
    void ClearText(bool raiseEvent) { text.clear(); if (raiseEvent) RaiseOnTextChanged(); }

    const std::string & GetText(void) const { return text; }


private:
    
    static const int NUMB_LETTERS = 26, //A-Z
                     NUMB_DIGITS = 10, //0-9
                     NUMB_SPECIALS = 17, // [ ] ; , . ' \ / ~ = - + - * / \t and the spacebar
                     NUMB_CONTROLS = 11; //Return, backspace, delete, page up, page down, end, home, left, right, up, down

    //The length of time since each kind of key was pressed down.
    //Any values set to -1.0 indicate that that character isn't currently being pressed.
    float timeHeldDownLetters[NUMB_LETTERS],
          timeHeldDownDigits[NUMB_DIGITS],
          timeHeldDownSpecials[NUMB_SPECIALS],
          timeHeldDownControls[NUMB_CONTROLS];


    std::string text;


    void RaiseOnTextChanged(void) { if (OnTextChanged != 0) OnTextChanged(this, OnTextChanged_Data); }
    void RaiseOnEnterKey(void) { if (OnEnterKey != 0) OnEnterKey(this, OnEnterKey_Data); }
    void RaiseOnCursorMoved(int moveAmount) { if (OnCursorMoved != 0) OnCursorMoved(this, moveAmount, OnCursorMoved_Data); }

    unsigned int GetClampedCursor(void) const { return BasicMath::Min(text.size(), CursorPos); }
};