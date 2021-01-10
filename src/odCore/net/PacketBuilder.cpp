
#include <odCore/net/PacketBuilder.h>

#include <algorithm>
#include <type_traits>

#include <odCore/anim/AnimModes.h>

namespace odNet
{

    PacketBuilder::PacketBuilder(const std::function<void(const char *, size_t, LinkType)> &packetCallback)
    : mPacketCallback(packetCallback)
    , mStreamBuffer(mPacketBuffer)
    , mOutputStream(&mStreamBuffer)
    , mWriter(mOutputStream)
    {
    }

    void PacketBuilder::globalDatabaseTableEntry(odDb::GlobalDatabaseIndex dbIndex, const std::string &path)
    {
        _beginPacket(PacketType::GLOBAL_DB_TABLE_ENTRY);
        mWriter << dbIndex;
        mWriter.write(path.data(), path.size());
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::loadLevel(const std::string &path, size_t loadedDatabaseCount)
    {
        if(loadedDatabaseCount > std::numeric_limits<uint32_t>::max())
        {
            throw od::Exception("Entry count out of bounds");
        }

        _beginPacket(PacketType::LOAD_LEVEL);
        mWriter << static_cast<uint32_t>(loadedDatabaseCount);
        mWriter.write(path.data(), path.size());
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states)
    {
        _beginPacket(PacketType::OBJECT_STATES_CHANGED);
        mWriter << tick
                << id;
        states.serialize(mWriter, odState::StateSerializationPurpose::NETWORK);
        _endPacket(LinkType::UNRELIABLE);
    }

    void PacketBuilder::objectExtraStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const char *data, size_t size)
    {
        _beginPacket(PacketType::OBJECT_EXTRA_STATES_CHANGED);
        mWriter << tick
                << id;
        mWriter.write(data, size);
        _endPacket(LinkType::UNRELIABLE);
    }

    void PacketBuilder::confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick)
    {
        _beginPacket(PacketType::CONFIRM_SNAPSHOT);
        mWriter << tick
                << realtime
                << static_cast<uint32_t>(discreteChangeCount)
                << referenceTick;
        _endPacket(LinkType::UNRELIABLE);
    }

    void PacketBuilder::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        _beginPacket(PacketType::GLOBAL_MESSAGE);
        mWriter << code;
        mWriter.write(data, size);
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::event(const odState::EventVariant &e, double realtime)
    {
        /*
        // FIXME: this only works as long as there are no more than four bone modes/playback types
        uint8_t flags =
              (static_cast<uint8_t>(modes.boneModes[0]) << 0)
            | (static_cast<uint8_t>(modes.boneModes[1]) << 2)
            | (static_cast<uint8_t>(modes.boneModes[2]) << 4)
            | (static_cast<uint8_t>(modes.playbackType) << 6);*/

        _beginPacket(PacketType::EVENT);
        mWriter << realtime;
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::actionTriggered(odInput::ActionCode code, odInput::ActionState state)
    {
        _beginPacket(PacketType::ACTION_TRIGGERED);
        mWriter << code << static_cast<uint8_t>(state);
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes)
    {
        _beginPacket(PacketType::ANALOG_ACTION_TRIGGERED);
        mWriter << code << axes.x << axes.y;
        _endPacket(LinkType::RELIABLE);
    }

    void PacketBuilder::acknowledgeSnapshot(odState::TickNumber tick)
    {
        _beginPacket(PacketType::ACKNOWLEDGE_SNAPSHOT);
        mWriter << tick;
        _endPacket(LinkType::UNRELIABLE);
    }

    void PacketBuilder::_beginPacket(PacketType type)
    {
        mPacketBuffer.clear();

        uint16_t dummyPayloadSize = 0;
        mWriter << static_cast<uint8_t>(type) << dummyPayloadSize;
    }

    void PacketBuilder::_endPacket(LinkType linkType)
    {
        mOutputStream.flush();

        size_t payloadSize = mPacketBuffer.size() - PacketConstants::HEADER_SIZE;
        if(payloadSize > 0xffff)
        {
            throw od::Exception("Packet payload size exceeds size fields limits");
        }

        auto p = mWriter.tell();
        mWriter.seek(1);
        mWriter << static_cast<uint16_t>(payloadSize);
        mWriter.seek(p);

        if(mPacketCallback != nullptr)
        {
            mPacketCallback(mPacketBuffer.data(), mPacketBuffer.size(), linkType);
        }
    }


}
