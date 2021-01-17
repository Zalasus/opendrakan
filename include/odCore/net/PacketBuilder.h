
#ifndef INCLUDE_ODCORE_NET_PACKETBUILDER_H_
#define INCLUDE_ODCORE_NET_PACKETBUILDER_H_

#include <vector>
#include <memory>
#include <functional>

#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/Protocol.h>
#include <odCore/net/UplinkConnector.h>

namespace odNet
{

    class PacketBuilder : public DownlinkConnector, public UplinkConnector
    {
    public:

        enum class LinkType
        {
            UNRELIABLE,
            RELIABLE
        };

        /**
         * packetCallback will be called for every packet built. The third parameter indicated whether the
         * packet needs to be sent over a reliable connection (i.e. TCP).
         */
        PacketBuilder(const std::function<void(const char *, size_t, LinkType)> &packetCallback);

        virtual void globalDatabaseTableEntry(odDb::GlobalDatabaseIndex dbIndex, const std::string &path) override final;
        virtual void loadLevel(const std::string &path, size_t loadedDatabaseCount) override final;
        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) override final;
        virtual void objectExtraStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const char *data, size_t size) override final;
        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick) override final;
        virtual void globalMessage(MessageChannelCode code, const char *data, size_t size) override final;
        virtual void event(const odState::EventVariant &e, double realtime) override final;

        virtual void actionTriggered(odInput::ActionCode code, odInput::ActionState state) override final;
        virtual void analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes) override final;
        virtual void acknowledgeSnapshot(odState::TickNumber tick) override final;


    private:

        void _beginPacket(PacketType type);
        void _endPacket(LinkType linkType);

        std::function<void(const char *, size_t, LinkType)> mPacketCallback;
        std::vector<char> mPacketBuffer;
        od::VectorOutputBuffer mStreamBuffer;
        std::ostream mOutputStream;
        od::DataWriter mWriter;

    };

}

#endif
