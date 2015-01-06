#include "Timing.h"

#include "../Math/Lower Math/Mathf.h"


bool Timer::Update(float timeElapsed)
{
    if (TimeLeft <= 0.0f) return true;

    TimeLeft -= timeElapsed;

    if (TimeLeft <= 0.0f)
    {
        for (int i = 0; i < ToNotify.size(); ++i)
            ToNotify[i]->OnTimerEvent(EventID, pArgs);
        return true;
    }
    else return false;
}

unsigned int TimerManager::UpdateTimers(float elapsedTime)
{
    //Update timers.
    unsigned int count = 0;
    for (int i = 0; i < timers.size(); ++i)
    {
        if (timers[i].T.Update(elapsedTime))
        {
            count += 1;
            if (timers[i].DestroyAfterEvent)
            {
                timers.erase(timers.begin() + i);
                i -= 1;
            }
            else
            {
                timers[i].T.TimeLeft += timers[i].T.TimeLength;
            }
        }
    }

    //Add new timers.
    for (unsigned int i = 0; i < toAdd.size(); ++i)
        timers.insert(timers.end(), toAdd[i]);
    toAdd.clear();

    //Remove timers based on event id and predicates.
    for (unsigned int i = 0; i < Mathf::Max(toRemoveByEventType.size(), toRemoveByPredicate.size()); ++i)
    {
        for (unsigned int j = 0; j < timers.size(); ++j)
        {
            if ((i < toRemoveByEventType.size() && timers[j].T.EventID == toRemoveByEventType[i]) ||
                (i < toRemoveByPredicate.size() && toRemoveByPredicate[i].Predicate(timers[j].T,
                                                                                    timers[j].DestroyAfterEvent,
                                                                                    toRemoveByPredicate[i].pArgs)))
            {
                timers.erase(timers.begin() + j);
            }

        }
    }
    toRemoveByEventType.clear();
    toRemoveByPredicate.clear();


    return count;
}