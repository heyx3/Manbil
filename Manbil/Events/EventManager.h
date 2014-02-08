#pragma once

//An abstract class that can respond to an event happening.
class EventResponder
{
public:
    //Called when an event happens.
    virtual void OnTimerEvent(int eventType, void* pArgs) = 0;
};

//TODO: Add an event dispatcher class.