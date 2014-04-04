#pragma once

#include <assert.h>
#include <vector>
#include "EventManager.h"


//Counts down a clock, then notifies "subscribers" of the clock going off.
class Timer
{
public:

    float TimeLeft, TimeLength;
    int EventID;
    void* pArgs;

    std::vector<EventResponder*> ToNotify;

    Timer(float timeLeft, EventResponder * toNotify, int eventID = 0, void* pArguments = 0)
        : EventID(eventID), pArgs(pArguments), TimeLeft(timeLeft), TimeLength(timeLeft)
    {
        assert(toNotify != 0);
        ToNotify.insert(ToNotify.begin(), toNotify);
    }
    Timer(float timeLeft, const std::vector<EventResponder*> & toNotify, int eventID = 0, void* pArguments = 0)
        : EventID(eventID), pArgs(pArguments), TimeLeft(timeLeft), TimeLength(timeLeft)
    {
        ToNotify.insert(ToNotify.begin(), toNotify.begin(), toNotify.end());
    }

    //Returns whether or not this Timer finally went off.
    //Returns "true" if it already went off earlier.
    bool Update(float timeElapsed);
};


//Manages a bunch of Timers.
class TimerManager
{
public:

    //Evaluates a timer.
    typedef bool(*TimerPredicate)(const Timer & t, bool willBeDestroyedAfterEvent, void* pArgs);


    //Adds a timer to this manager. "destroyAfterEvent" determines
    //   whether the timer is 1) removed or 2) reset when it goes off.
    void AddTimer(const Timer & t, bool destroyAfterEvent)
    {
        toAdd.insert(timers.end(), TimerElement(t, destroyAfterEvent));
    }
    //Removes all timers with the given event type.
    void RemoveTimers(int eventType) { toRemoveByEventType.insert(toRemoveByEventType.end(), eventType); }
    //Removes all timers that pass the given predicate function.
    //The predicate function may optionally be passed some extra information to use.
    void RemoveTimers(bool(*predicate)(const Timer & t, bool willBeDestroyedAfterEvent, void* args), void* args = 0)
    {
        PredicateData pd;
        pd.Predicate = predicate;
        pd.pArgs = args;
        toRemoveByPredicate.insert(toRemoveByPredicate.end(), pd);
    }
    //Updates all timers and returns the number of them that went off.
    unsigned int UpdateTimers(float elapsedTime);


private:

    struct TimerElement
    {
    public: Timer T; bool DestroyAfterEvent;
            TimerElement(Timer t, bool destroy) : T(t), DestroyAfterEvent(destroy) { }
    };

    std::vector<TimerElement> timers, toAdd;

    std::vector<int> toRemoveByEventType;
    struct PredicateData { public: TimerPredicate Predicate; void* pArgs; };
    std::vector<PredicateData> toRemoveByPredicate;
};