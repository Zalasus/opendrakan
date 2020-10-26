
#ifndef INCLUDE_ODCORE_NET_QUEUEDDOWNLINKCONNECTOR_H_
#define INCLUDE_ODCORE_NET_QUEUEDDOWNLINKCONNECTOR_H_

#include <mutex>
#include <vector>
#include <variant>

#include <odCore/CTypes.h>

#include <odCore/net/DownlinkConnector.h>

namespace odNet
{

    /**
     * @brief A DownlinkConnector implementation that stores it's calls and arguments in a queue, to be dispatched to another connector later.
     *
     * This is useful for synchronizing the connection. Network events and even the local client-server-link
     * generally happen in multiple threads and therefore need to be synchronized.
     */
    class QueuedDownlinkConnector final : public DownlinkConnector
    {
    public:

        void flushQueue(DownlinkConnector &c);

        virtual void loadLevel(const std::string &path) override;
        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) override;
        virtual void objectExtraStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const char *data, size_t size) override;
        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick) override;
        virtual void globalMessage(MessageChannelCode code, const char *data, size_t size) override;


    private:

        struct LoadLevel
        {
            std::string path;
        };

        struct ObjectStatesChanged
        {
            odState::TickNumber tick;
            od::LevelObjectId id;
            od::ObjectStates states;
        };

        struct ObjectExtraStatesChanged
        {
            odState::TickNumber tick;
            od::LevelObjectId id;
            size_t dataOffset;
            size_t size;
        };

        struct ConfirmSnapshot
        {
            odState::TickNumber tick;
            double realtime;
            size_t discreteChangeCount;
            odState::TickNumber referenceTick;
        };

        struct GlobalMessage
        {
            MessageChannelCode code;
            // it would be really inefficient to allocate storage for every message,
            //  so instead we store them concatenated in a single vector and store them as slices here
            size_t dataOffset; // index in mMessages where message data starts
            size_t size;
        };

        using QueuedCall = std::variant<LoadLevel, ObjectStatesChanged, ObjectExtraStatesChanged, ConfirmSnapshot, GlobalMessage>;

        std::vector<QueuedCall> mCalls;
        std::vector<char> mMessages;
        std::mutex mMutex;

    };

}

#endif
