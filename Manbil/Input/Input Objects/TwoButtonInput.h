#pragma once

#include <memory>
#include "../FloatInput.h"
#include "../BoolInput.h"


//Takes two buttons and puts them on an axis.
//The neutral value is 0.5f, the left side value is 0.0f, and the right side value is 1.0f.
class TwoButtonInput : public FloatInput
{
public:

    std::shared_ptr<BoolInput> LeftButton, RightButton;

    TwoButtonInput(std::shared_ptr<BoolInput> leftButton, std::shared_ptr<BoolInput> rightButton) : FloatInput(0.5f), LeftButton(leftButton), RightButton(rightButton) { }


protected:

    virtual float GetRawInput(float el, float tot) override
    {
        LeftButton->Update(el, tot);
        RightButton->Update(el, tot);

        float value = 0.5f;
        if (LeftButton->GetValue())
            value -= 0.5f;
        if (RightButton->GetValue())
            value += 0.5f;

        return value;
    }
};