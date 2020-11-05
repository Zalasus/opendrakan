
#include <odCore/state/EventQueue.h>

#include <odCore/db/DbManager.h>
#include <odCore/db/Database.h>

namespace odState
{

    EventQueue::EventQueue(odDb::DbManager &dbManager)
    : mDbManager(dbManager)
    {
    }

    void EventQueue::addAnimationEvent(double realtime, od::LevelObject &object, odDb::GlobalAssetRef animRef, int32_t channelIndex, float speedModifier)
    {
        ObjectAnimEvent event(realtime, object, animRef, channelIndex, speedModifier);

        // load animation TODO: do this asynchronously
        auto db = mDbManager.getDatabaseByGlobalIndex(animRef.globalDbIndex);
        if(db != nullptr)
        {
            event.animation = db->getAnimation(animRef.assetId);
        }

        // no idea why this does not work:
        //auto it = _getEventInsertPoint(realtime);
        //mEvents.emplace(it, event);
        mEvents.emplace_back(event);
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
        if(mEvents.empty() || realtimeForEventVariant(mEvents.back()) <= realtime)
        {
            return mEvents.end();

        }else
        {
            auto pred = [](double time, EventVariant &event) { return time < realtimeForEventVariant(event); };
            return std::upper_bound(mEvents.begin(), mEvents.end(), realtime, pred);
        }
    }
}
