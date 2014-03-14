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

    //Adds a timer to this manager. "destroyAfterEvent" determines
    //   whether the timer is 1) removed or 2) reset when it goes off.
    void AddTimer(const Timer & t, bool destroyAfterEvent)
    {
        timers.insert(timers.end(), TimerElement(t, destroyAfterEvent));
    }
    //Removes all timers with the given event type.
    //Returns the number of timers that were removed.
    unsigned int RemoveTimers(int eventType);
    //Removes all timers that pass the given predicate function.
    //The predicate function may optionally be passed some extra information to use.
    //Returns the number of timers that were removed.
    unsigned int RemoveTimers(bool(*predicate)(const Timer & t, bool willBeDestroyedAfterEvent, void* args), void* args = 0);
    //Updates all timers and returns the number of them that went off.
    unsigned int UpdateTimers(float elapsedTime);


private:

    struct TimerElement
    {
    public: Timer T; bool DestroyAfterEvent;
            TimerElement(Timer t, bool destroy) : T(t), DestroyAfterEvent(destroy) { }
    };

    std::vector<TimerElement> timers;
    //TODO: Add "toRemove" and "toAdd" lists and use them during "UpdateTimers", so that timers that go off can remove/add other timers without causing a problem.
};