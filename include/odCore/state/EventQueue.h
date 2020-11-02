
#ifndef INCLUDE_ODCORE_STATE_EVENTQUEUE_H_
#define INCLUDE_ODCORE_STATE_EVENTQUEUE_H_

#include <vector>

#include <odCore/state/Event.h>

namespace odDb
{
    class DbManager;
}

namespace odState
{

    /**
     * Handles storing events until they happen, prefetching assets in that time
     * and stuff.
     *
     * TODO: is this really "state"?
     */
    class EventQueue
    {
    public:

        EventQueue(odDb::DbManager &dbManager);

        void addAnimationEvent(double realtime, od::LevelObject &object, odDb::AssetRef animRef, int32_t channelIndex, float speedModifier);
        void sound();

        void dispatch(double realtime);


    private:

        using EventIterator = std::vector<EventVariant>::iterator;

        EventIterator _getEventInsertPoint(double realtime);

        odDb::DbManager &mDbManager;

        std::vector<EventVariant> mEvents;

    };

}

#endif
