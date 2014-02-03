#include "Timing.h"

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

unsigned int TimerManager::RemoveTimers(int eventType)
{
    unsigned int count = 0;
    for (int i = 0; i < timers.size(); ++i)
    {
        if (timers[i].T.EventID == eventType)
        {
            timers.erase(timers.begin() + i);
            count += 1;
            i -= 1;
        }
    }
    return count;
}
unsigned int TimerManager::RemoveTimers(bool(*predicate)(const Timer & t, bool wbd, void* args), void* args)
{
    unsigned int count = 0;
    for (int i = 0; i < timers.size(); ++i)
    {
        if (predicate(timers[i].T, timers[i].DestroyAfterEvent, args))
        {
            timers.erase(timers.begin() + 1);
            count += 1;
            i -= 1;
        }
    }
    return count;
}

unsigned int TimerManager::UpdateTimers(float elapsedTime)
{
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
    return count;
}