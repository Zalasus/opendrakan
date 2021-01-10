
#include <odCore/state/EventQueue.h>

#include <odCore/Level.h>
#include <odCore/LevelObject.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>

#include <odCore/net/DownlinkConnector.h>

namespace odState
{

    class DispatchVisitor
    {
    public:

        DispatchVisitor(od::Level &l, double dt)
        : mLevel(l)
        , mTimeDelta(dt)
        {
        }

        bool operator()(const Event &event)
        {
            return true;
        }

        bool operator()(const ObjectMessageEvent &event)
        {
            return _objectEvent(event, event.receiverObjectId);
        }

        bool operator()(const ObjectAnimEvent &event)
        {
            return _objectEvent(event, event.objectId);
        }


    private:

        bool _objectEvent(EventVariant event, od::LevelObjectId receiverId)
        {
            auto obj = mLevel.getLevelObjectById(receiverId);
            if(obj != nullptr)
            {
                return obj->handleEvent(event, mTimeDelta);
            }

            return true;
        }

        od::Level &mLevel;
        double mTimeDelta;
    };


    struct PrefetchVisitor
    {
        odDb::DbManager &dbManager;

        PrefetchVisitor(odDb::DbManager &d)
        : dbManager(d)
        {
        }

        void operator()(Event &event)
        {
        }

        void operator()(ObjectAnimEvent &event)
        {
            event.anim = dbManager.loadAsset<odDb::Animation>(event.animRef);
        }
    };


    EventQueue::EventQueue(odDb::DbManager &dbManager, od::Level &level)
    : mDbManager(dbManager)
    , mLevel(level)
    {
    }

    void EventQueue::addIncomingEvent(double realtime, const EventVariant &event)
    {
        auto it = _getEventInsertPoint(realtime);
        auto inserted = mEvents.emplace(it, EventData{event, realtime, false, true});

        // prefetch assets. this should later happen asynchronously
        PrefetchVisitor pv(mDbManager);
        std::visit(pv, inserted->event);
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
        for(auto &eventData : mEvents)
        {
            if(eventData.realtime <= realtime)
            {
                if(!eventData.dispatched)
                {
                    // the dispatch functor returns false if dispatch should be retried later.
                    //  this might happen if an object is stopped and thus can not process events
                    DispatchVisitor visitor(mLevel, realtime - eventData.realtime);
                    eventData.dispatched = std::visit(visitor, eventData.event);
                }

            }else
            {
                break; // event queue is sorted, so the next events won't be relevant either. we are done
            }
        }
    }

    void EventQueue::sendEventsToClient(odNet::DownlinkConnector &connector, double realtime)
    {
        for(auto &eventData : mEvents)
        {
            if(eventData.realtime <= realtime)
            {
                if(!eventData.sent)
                {
                    connector.event(eventData.event, eventData.realtime);
                }

            }else
            {
                break;
            }
        }
    }

    void EventQueue::markAsSent(double realtime)
    {
        for(auto &eventData : mEvents)
        {
            if(eventData.realtime <= realtime)
            {
                eventData.sent = true;

            }else
            {
                break;
            }
        }
    }

    void EventQueue::cleanup()
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
            auto pred = [](double time, auto &eventData) { return time < eventData.realtime; };
            return std::upper_bound(mEvents.begin(), mEvents.end(), realtime, pred);
        }
    }
}
