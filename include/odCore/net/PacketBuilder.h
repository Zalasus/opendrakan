
#ifndef INCLUDE_ODCORE_NET_PACKETBUILDER_H_
#define INCLUDE_ODCORE_NET_PACKETBUILDER_H_

#include <vector>
#include <memory>
#include <functional>

#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/Protocol.h>

namespace odNet
{

    class DownlinkPacketBuilder : public DownlinkConnector
    {
    public:

        DownlinkPacketBuilder(const std::function<void(const char *, size_t)> &packetCallback);

        virtual void loadLevel(const std::string &path) override final;
        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) override final;
        virtual void spawnObject(od::LevelObjectId id) override final;
        virtual void despawnObject(od::LevelObjectId id) override final;
        virtual void destroyObject(od::LevelObjectId id) override final;
        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount) override final;


    private:

        void _beginPacket(PacketType type, uint16_t payloadSize);
        void _endPacket();

        std::function<void(const char *, size_t)> mPacketCallback;
        std::vector<char> mPacketBuffer;
        od::MemoryOutputBuffer mStreamBuffer;
        std::ostream mOutputStream;
        od::DataWriter mWriter;

    };

}

#endif
