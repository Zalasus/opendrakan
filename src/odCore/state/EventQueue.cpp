
#include <odCore/state/EventQueue.h>

namespace odState
{

    EventQueue::EventQueue(odDb::DbManager &dbManager)
    : mDbManager(dbManager)
    {
    }

    void EventQueue::addAnimationEvent(double realtime, od::LevelObject &object, odDb::GlobalAssetRef animRef, int32_t channelIndex, float speedModifier)
    {
        auto it = _getEventInsertPoint(realtime);
        //mEvents.insert(it, ObjectAnimEvent(realtime, object, animRef, channelIndex, speedModifier));

        // load animation TODO: do this asynchronously
        //auto animation = mDbManager
        //  TODO: uhhh... huge problem here: asset references are context-dependent. where to we get the dependency table from?
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
