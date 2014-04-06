#pragma once
#include <assert.h>


template<typename Value>
//Represents a value that may or may not exist.
struct OptionalValue
{
public:

    bool HasValue(void) const { return hasValue; }
    bool SetValue(const Value & val) const { value = val; }
    Value GetValue(void) const { assert(hasValue); return value; }

    OptionalValue(void) : hasValue(false) { }
    OptionalValue(const Value & valueCopy) : hasValue(true), value(valueCopy) { }

private:

    bool hasValue;
    Value value;
};