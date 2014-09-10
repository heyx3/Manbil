#include "KeyboardTextInput.h"

#include <assert.h>
#include <SFML/Window/Keyboard.hpp>



KeyboardTextInput::KeyboardTextInput(std::string _text)
    : text(_text)
{
    for (unsigned int i = 0; i < NUMB_LETTERS; ++i)
        timeHeldDownLetters[i] = -1.0f;
    for (unsigned int i = 0; i < NUMB_DIGITS; ++i)
        timeHeldDownDigits[i] = -1.0f;
    for (unsigned int i = 0; i < NUMB_SPECIALS; ++i)
        timeHeldDownSpecials[i] = -1.0f;
    for (unsigned int i = 0; i < NUMB_CONTROLS; ++i)
        timeHeldDownControls[i] = -1.0f;
}

void KeyboardTextInput::InsertChar(unsigned int pos, char value)
{
    if (CursorPos >= pos) CursorPos += 1;

    text.insert(text.begin() + pos, value);
    RaiseOnTextChanged();
}
char KeyboardTextInput::RemoveChar(unsigned int pos)
{
    if (pos >= text.size()) return 0;

    if (CursorPos > pos) CursorPos -= 1;

    char c = text[pos];
    text.erase(text.begin() + pos);
    RaiseOnTextChanged();
    return c;
}
void KeyboardTextInput::InsertText(unsigned int pos, const std::string & value)
{
    if (CursorPos >= pos) CursorPos += value.size();

    text.insert(text.begin() + pos, value.begin(), value.end());
    RaiseOnTextChanged();
}
std::string KeyboardTextInput::RemoveText(unsigned int pos, unsigned int length)
{
    if (pos >= text.size()) return "";

    //Trim the length to not extend beyond the length of the string.
    if (pos + length > text.size())
        length = text.size() - pos;

    if (CursorPos > pos) CursorPos -= length;

    std::string s = text.substr(pos, length);
    text.erase(pos, length);
    RaiseOnTextChanged();
    return s;
}


void KeyboardTextInput::CopyTo(KeyboardTextInput & outCpy) const
{
    outCpy.OnTextChanged = OnTextChanged;
    outCpy.OnTextChanged_Data = OnTextChanged_Data;
    outCpy.OnEnterKey = OnEnterKey;
    outCpy.OnEnterKey_Data = OnEnterKey_Data;
    outCpy.OnCursorMoved = OnCursorMoved;
    outCpy.OnCursorMoved_Data = OnCursorMoved_Data;

    outCpy.CursorPos = CursorPos;
    outCpy.KeyRepeatWait = KeyRepeatWait;
    outCpy.EnterKeyAddsText = false;

    memcpy(outCpy.timeHeldDownLetters, timeHeldDownLetters, sizeof(float) * NUMB_LETTERS);
    memcpy(outCpy.timeHeldDownDigits, timeHeldDownDigits, sizeof(float) * NUMB_DIGITS);
    memcpy(outCpy.timeHeldDownSpecials, timeHeldDownSpecials, sizeof(float) * NUMB_SPECIALS);
    memcpy(outCpy.timeHeldDownControls, timeHeldDownControls, sizeof(float) * NUMB_CONTROLS);

    outCpy.text = text;
}

void KeyboardTextInput::Update(float elapsedTime)
{
    if (CursorPos > text.size()) CursorPos = text.size();


    //Update all key counters.

    #define GetOffsetKey(key, i) sf::Keyboard::isKeyPressed((sf::Keyboard::Key)((int)key + i))
    #define GetKeyUsable(timeArray) (timeArray[i] <= 0.0f || timeArray[i] > KeyRepeatWait)

    #define UpdateKeyTime(keyTime) \
        if (keyTime < -0.0001f) \
            keyTime = 0.0f; \
        else keyTime += elapsedTime;


    bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::RShift),
         ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    for (unsigned int i = 0; i < NUMB_LETTERS; ++i)
    {
        if (GetOffsetKey(sf::Keyboard::A, i))
        {
            UpdateKeyTime(timeHeldDownLetters[i]);
            if (GetKeyUsable(timeHeldDownLetters))
            {
                if (shift)
                    InsertChar(GetClampedCursor(), 'a' + i + ('A' - 'a'));
                else InsertChar(GetClampedCursor(), 'a' + i);
            }
        }
        else timeHeldDownLetters[i] = -1.0f;
    }
    for (unsigned int i = 0; i < NUMB_DIGITS; ++i)
    {
        if (GetOffsetKey(sf::Keyboard::Num0, i) ||
            GetOffsetKey(sf::Keyboard::Numpad0, i))
        {
            UpdateKeyTime(timeHeldDownDigits[i]);

            if (GetKeyUsable(timeHeldDownDigits))
            {
                char c = '0' + i;
                if (shift)
                {
                    switch (i)
                    {
                        case 0: c = ')'; break;
                        case 1: c = '!'; break;
                        case 2: c = '@'; break;
                        case 3: c = '#'; break;
                        case 4: c = '$'; break;
                        case 5: c = '%'; break;
                        case 6: c = '^'; break;
                        case 7: c = '&'; break;
                        case 8: c = '*'; break;
                        case 9: c = '('; break;
                        default: assert(false);
                    }
                }
                InsertChar(GetClampedCursor(), c);
            }
        }
        else timeHeldDownDigits[i] = -1.0f;
    }
    for (unsigned int i = 0; i < NUMB_SPECIALS; ++i)
    {
        sf::Keyboard::Key specialKey = sf::Keyboard::Unknown;
        char c = '\0';
        switch (i)
        {
            case 0:
                specialKey = sf::Keyboard::LBracket;
                c = (shift ? '{' : '[');
                break;
            case 1:
                specialKey = sf::Keyboard::RBracket;
                c = (shift ? '}' : ']');
                break;
            case 2:
                specialKey = sf::Keyboard::SemiColon;
                c = (shift ? ':' : ';');
                break;
            case 3:
                specialKey = sf::Keyboard::Comma;
                c = (shift ? '<' : ',');
                break;
            case 4:
                specialKey = sf::Keyboard::Period;
                c = (shift ? '>' : '.');
                break;
            case 5:
                specialKey = sf::Keyboard::Quote;
                c = (shift ? '"' : '\'');
                break;
            case 6:
                specialKey = sf::Keyboard::BackSlash;
                c = (shift ? '|' : '\\');
                break;
            case 7:
                specialKey = sf::Keyboard::Slash;
                c = (shift ? '?' : '/');
                break;
            case 8:
                specialKey = sf::Keyboard::Tilde;
                c = (shift ? '~' : '`');
                break;
            case 9:
                specialKey = sf::Keyboard::Equal;
                c = (shift ? '+' : '=');
                break;
            case 10:
                specialKey = sf::Keyboard::Dash;
                c = (shift ? '_' : '-');
                break;
            case 11:
                specialKey = sf::Keyboard::Add;
                c = '+';
                break;
            case 12:
                specialKey = sf::Keyboard::Subtract;
                c = '-';
                break;
            case 13:
                specialKey = sf::Keyboard::Multiply;
                c = '*';
                break;
            case 14:
                specialKey = sf::Keyboard::Divide;
                c = '/';
                break;
            case 15:
                specialKey = sf::Keyboard::Tab;
                c = '\t';
                break;
            case 16:
                specialKey = sf::Keyboard::Space;
                c = ' ';
                break;

            default: assert(false);
        }

        if (sf::Keyboard::isKeyPressed(specialKey))
        {
            UpdateKeyTime(timeHeldDownSpecials[i]);
            if (GetKeyUsable(timeHeldDownSpecials))
                InsertChar(GetClampedCursor(), c);
        }
        else timeHeldDownSpecials[i] = -1.0f;
    }
    for (unsigned int i = 0; i < NUMB_CONTROLS; ++i)
    {
        switch (i)
        {
            case 0:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    if (GetKeyUsable(timeHeldDownControls))
                    {
                        if (EnterKeyAddsText)
                            InsertChar(CursorPos, '\n');
                        RaiseOnEnterKey();
                    }
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 1:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    if (GetKeyUsable(timeHeldDownControls) && CursorPos > 0)
                        if (ctrl) RemoveText(0, CursorPos);
                        else RemoveChar(CursorPos - 1);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 2:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    if (GetKeyUsable(timeHeldDownControls) && CursorPos < text.size())
                        if (ctrl) RemoveText(CursorPos, text.size());
                        else RemoveChar(CursorPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 3:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls))
                        CursorPos = 0;
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 4:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls))
                        CursorPos = text.size();
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 5:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::End))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls))
                        CursorPos = text.size();
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 6:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Home))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls))
                        CursorPos = 0;
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 7:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls) && CursorPos > 0)
                        CursorPos -= 1;
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 8:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls) && CursorPos < text.size())
                        CursorPos += 1;
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 9:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls))
                        CursorPos = 0;
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;
            case 10:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    UpdateKeyTime(timeHeldDownControls[i]);
                    int oldPos = (int)CursorPos;
                    if (GetKeyUsable(timeHeldDownControls))
                        CursorPos = text.size();
                    RaiseOnCursorMoved((int)CursorPos - oldPos);
                }
                else timeHeldDownControls[i] = -1.0f;
                break;

            default: assert(false);
        }
    }
}