
#ifndef INCLUDE_ODCORE_STATE_EVENTQUEUE_H_
#define INCLUDE_ODCORE_STATE_EVENTQUEUE_H_

#include <vector>
#include <utility>

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

        /**
         * @brief Adds an event coming from the server to the queue.
         */
        void addIncomingEvent(double realtime, const EventVariant &event);

        /**
         * @brief Adds a local event that won't get dispatched at the current time.
         *
         * Only for the purpose of logging events that need sending to clients.
         * It is assumed that the event has already been processed.
         */
        void logEvent(double realtime, const EventVariant &event);

        /**
         * @brief Same as logEvent(double, const EventVariant &), but logs event at current simulation time.
         */
        void logEvent(const EventVariant &event);

        void dispatch(double realtime);


    private:

        using EventVector = std::vector<std::pair<double, EventVariant>>;
        using EventIterator = EventVector::iterator;

        EventIterator _getEventInsertPoint(double realtime);

        odDb::DbManager &mDbManager;

        EventVector mEvents;

    };

}

#endif
