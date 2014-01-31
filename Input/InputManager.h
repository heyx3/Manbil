#pragma once

#include <memory>
#include <unordered_map>

#include "BoolInput.h"
#include "FloatInput.h"
#include "Vector2Input.h"


typename std::shared_ptr<BoolInput> BoolInputPtr;
typename std::shared_ptr<FloatInput> FloatInputPtr;
typename std::shared_ptr<Vector2Input> Vector2InputPtr;


template<typename Key>
//Handles multiple inputs. Inputs are indexed by some Key.
class InputManager
{
public:

    bool GetBoolInputValue(const Key & key) { assert(boolInputs.find(key) != boolInputs.end()); return boolInputs[key]->GetValue(); }
    float GetFloatInputValue(const Key & key) { assert(floatInputs.find(key) != floatInputs.end()); return floatInputs[key]->GetValue(); }
    Vector2f GetV2InputValue(const Key & key) { assert(v2Inputs.find(key) != v2Inputs.end()); return v2Inputs[key]->GetValue(); }

    //Returns whether or not this function succeeded.
    //It will fail if the given key already exists for another BoolInput.
    bool AddBoolInput(const Key & key, BoolInputPtr input)
    {
        if (boolInputs.find(key) != boolInputs.end()) return false;
        boolInputs[key] = input;
    }
    //Returns whether or not this function succeeded.
    //It will fail if the given key already exists for another FloatInput.
    bool AddFloatInput(const Key & key, FloatInputPtr input)
    {
        if (floatInputs.find(key) != floatInputs.end()) return false;
        floatInputs[key] = input;
    }
    //Returns whether or not this function succeeded.
    //It will fail if the given key already exists for another Vector2Input.
    bool AddV2Input(const Key & key, Vector2InputPtr input)
    {
        if (v2Inputs.find(key) != v2Inputs.end()) return false;
        v2Inputs[key] = input;
    }

    //Removes the BoolInput for the given key and returns it.
    //Returns a pointer to 0 if the given key doesn't point to a BoolInput.
    BoolInputPtr RemoveBoolInput(const Key & key)
    {
        if (boolInputs.find(key) == boolInputs.end()) return BoolInputPtr(0);

        BoolInputPtr ptr = boolInputs[key];
        boolInputs.erase(boolInputs.find(key));
        return ptr;
    }
    //Removes the FloatInput for the given key and returns it.
    //Returns a pointer to 0 if the given key doesn't point to a FloatInput.
    BoolInputPtr RemoveFloatInput(const Key & key)
    {
        if (floatInputs.find(key) == floatInputs.end()) return FloatInputPtr(0);

        FloatInputPtr ptr = floatInputs[key];
        floatInputs.erase(floatInputs.find(key));
        return ptr;
    }
    //Removes the Vector2Input for the given key and returns it.
    //Returns a pointer to 0 if the given key doesn't point to a Vector2Input.
    BoolInputPtr RemoveV2Input(const Key & key)
    {
        if (v2Inputs.find(key) == v2Inputs.end()) return Vector2InputPtr(0);

        Vector2InputPtr ptr = v2Inputs[key];
        v2Inputs.erase(v2Inputs.find(key));
        return ptr;
    }


    //Updates all inputs already added to this manager.
    void Update(float elapsedFrameTime, float elapsedTotalTime)
    {
        for (std::unordered_map<Key, BoolInputPtr>::const_iterator it = boolInputs.begin();
             it != boolInputs.end(); it += 1)
        {
            it->Update(elapsedFrameTime, elapsedTotalTime);
        }
        for (std::unordered_map<Key, FloatInputPtr>::const_iterator it = floatInputs.begin();
             it != floatInputs.end(); it += 1)
        {
            it->Update(elapsedFrameTime, elapsedTotalTime);
        }
        for (std::unordered_map<Key, Vector2InputPtr>::const_iterator it = v2Inputs.begin();
             it != v2Inputs.end(); it += 1)
        {
            it->Update(elapsedFrameTime, elapsedTotalTime);
        }
    }


private:

    std::unordered_map<Key, BoolInputPtr> boolInputs;
    std::unordered_map<Key, FloatInputPtr> floatInputs;
    std::unordered_map<Key, Vector2InputPtr> v2Inputs;
};