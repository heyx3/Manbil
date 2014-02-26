#pragma once
#include <assert.h>


template<typename Value>
//Represents a value that may or may not exist.
struct OptionalValue
{
public:

    bool HasValue(void) const { return hasValue; }
    Value GetValue(void) const { assert(hasValue); return value; }

    OptionalValue(void) : hasValue(false) { }
    OptionalValue(const Value & valueCopy) : value(valueCopy) { }

private:

    bool hasValue;
    Value value;
};