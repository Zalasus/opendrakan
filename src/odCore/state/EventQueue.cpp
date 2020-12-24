
#include <odCore/state/EventQueue.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>

#include <odCore/net/DownlinkConnector.h>

namespace odState
{

    struct DispatchVisitor
    {
        bool operator()(const Event &event)
        {
            return true;
        }

        bool operator()(const ObjectAnimEvent &event)
        {
            return true;
        }
    };


    struct SendVisitor
    {
        odNet::DownlinkConnector &connector;
        double realtime;

        SendVisitor(odNet::DownlinkConnector &c, double rt)
        : connector(c)
        , realtime(rt)
        {
        }

        void operator()(const Event &event)
        {
        }

        void operator()(const ObjectAnimEvent &event)
        {
            connector.objectAnimation(event.objectId, event.animRef, event.channelIndex, event.speedModifier, realtime);
        }
    };


    EventQueue::EventQueue(odDb::DbManager &dbManager)
    : mDbManager(dbManager)
    {
    }

    void EventQueue::addIncomingEvent(double realtime, const EventVariant &event)
    {
        auto it = _getEventInsertPoint(realtime);
        mEvents.emplace(it, EventData{event, realtime, false, true});
    }

    void EventQueue::logEvent(double realtime, const EventVariant &event)
    {
        auto it = _getEventInsertPoint(realtime);
        mEvents.emplace(it, EventData{event, realtime, true, false});
    }

    void EventQueue::logEvent(const EventVariant &event)
    {
        logEvent(mCurrentTime, event);
    }

    void EventQueue::dispatch(double realtime)
    {
        DispatchVisitor visitor;

        auto it = mEvents.begin();
        for(; it != mEvents.end(); ++it)
        {
            if(it->realtime <= realtime && !it->dispatched)
            {
                // the dispatch functor returns false if dispatch should be retried later.
                //  this might happen if an object is stopped and thus can not process events
                it->dispatched = std::visit(visitor, it->event);

            }else
            {
                break;
            }
        }

        mEvents.erase(mEvents.begin(), it);
    }

    void EventQueue::sendEventsToClient(odNet::DownlinkConnector &connector, double realtime)
    {
        SendVisitor visitor(connector, realtime);

        for(auto &event : mEvents)
        {
            if(event.realtime <= realtime && !event.sent)
            {
                std::visit(visitor, event.event);

            }else
            {
                break;
            }
        }
    }

    void EventQueue::markAsSent(double realtime)
    {
        for(auto &event : mEvents)
        {
            if(event.realtime <= realtime)
            {
                event.sent = true;

            }else
            {
                break;
            }
        }
    }

    void EventQueue::flush()
    {
        while(!mEvents.empty() && mEvents.front().dispatched && mEvents.front().sent)
        {
            mEvents.pop_front();
        }
    }

    EventQueue::EventIterator EventQueue::_getEventInsertPoint(double realtime)
    {
        if(mEvents.empty() || mEvents.back().realtime <= realtime)
        {
            return mEvents.end();

        }else
        {
            auto pred = [](double time, auto &event) { return time < event.realtime; };
            return std::upper_bound(mEvents.begin(), mEvents.end(), realtime, pred);
        }
    }
}
