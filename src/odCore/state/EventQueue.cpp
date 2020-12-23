
#include <odCore/state/EventQueue.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>

namespace odState
{

    EventQueue::EventQueue(odDb::DbManager &dbManager)
    : mDbManager(dbManager)
    {
    }

    void EventQueue::addIncomingEvent(double realtime, const EventVariant &event)
    {
    }

    void EventQueue::logEvent(double realtime, const EventVariant &event)
    {
        auto it = _getEventInsertPoint(realtime);
        mEvents.emplace(it, std::make_pair(realtime, event));
    }

    void EventQueue::logEvent(const EventVariant &event)
    {
        double currentTime = 0.0; // TODO: fetch somewhere
        logEvent(currentTime, event);
    }

    void EventQueue::dispatch(double realtime)
    {
        /*DispatchVisitor visitor;

        for(auto &event : mEvents)
        {
            if(realtimeForEventVariant(event) <= realtime)
            {
                std::visit(visitor, event);
            }
        }*/
    }

    EventQueue::EventIterator EventQueue::_getEventInsertPoint(double realtime)
    {
        if(mEvents.empty() || mEvents.back().first <= realtime)
        {
            return mEvents.end();

        }else
        {
            auto pred = [](double time, auto &event) { return time < event.first; };
            return std::upper_bound(mEvents.begin(), mEvents.end(), realtime, pred);
        }
    }
}
