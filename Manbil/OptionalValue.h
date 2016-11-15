#pragma once

#include <assert.h>


template<typename Value>
//Represents a value that may or may not exist.
struct OptionalValue
{
public:

    OptionalValue(void) : hasValue(false) { }
    OptionalValue(const Value & valueCopy) : hasValue(true), value(valueCopy) { }

    
    bool HasValue(void) const { return hasValue; }
    Value GetValue(void) const { assert(hasValue); return value; }

    void SetValue(const Value & val) { value = val; }


private:

    bool hasValue;
    Value value;
};