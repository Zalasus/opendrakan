
#ifndef INCLUDE_ODCORE_STATE_EVENTQUEUE_H_
#define INCLUDE_ODCORE_STATE_EVENTQUEUE_H_

#include <deque>
#include <utility>

#include <odCore/state/Event.h>

namespace od
{
    class Level;
}

namespace odDb
{
    class DbManager;
}

namespace odNet
{
    class DownlinkConnector;
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

        EventQueue(odDb::DbManager &dbManager, od::Level &level);

        inline void setCurrentTime(double t) { mCurrentTime = t; }

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

        void sendEventsToClient(odNet::DownlinkConnector &connector, double realtime);

        /**
         * @brief Marks all events from realtime or ealier as sent.
         *
         * This will prevent them from being sent the at the next call of sendEventsToClient(). If they are also marked as dispatched,
         * the next call to flush() will remove them from the queue.
         */
        void markAsSent(double realtime);

        /**
         * @brief Removes all dispatched and sent events from the start of the queue.
         */
        void flush();


    private:

        struct EventData
        {
            EventVariant event;
            double realtime;
            bool dispatched;
            bool sent;
        };

        using EventVector = std::deque<EventData>;
        using EventIterator = EventVector::iterator;

        EventIterator _getEventInsertPoint(double realtime);

        odDb::DbManager &mDbManager;
        od::Level &mLevel;

        double mCurrentTime;
        EventVector mEvents;

    };

}

#endif
