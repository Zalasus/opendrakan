
#include <odCore/net/PacketBuilder.h>

#include <algorithm>
#include <type_traits>

namespace odNet
{

    PacketBuilder::PacketBuilder(const std::function<void(const char *, size_t)> &packetCallback)
    : mPacketCallback(packetCallback)
    , mStreamBuffer(mPacketBuffer)
    , mOutputStream(&mStreamBuffer)
    , mWriter(mOutputStream)
    {
    }

    void PacketBuilder::loadLevel(const std::string &path)
    {
        uint16_t pathLength = path.size() > 0xffff ? 0xffff : path.size(); // TODO: warn on truncation?

        _beginPacket(PacketType::LOAD_LEVEL);
        mWriter.write(path.data(), pathLength);
        _endPacket();
    }

    void PacketBuilder::objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states)
    {
        uint32_t flags = 0;
        if(states.position.hasValue()) flags |= PacketConstants::STATE_MASK_POSITION;
        if(states.rotation.hasValue()) flags |= PacketConstants::STATE_MASK_ROTATION;
        if(states.scale.hasValue()) flags |= PacketConstants::STATE_MASK_SCALE;
        if(states.visibility.hasValue()) flags |= PacketConstants::STATE_MASK_VISIBILITY;

        _beginPacket(PacketType::OBJECT_STATE_CHANGED);
        mWriter << tick
                << id
                << flags;
        if(states.position.hasValue())   mWriter << states.position.get();
        if(states.rotation.hasValue())   mWriter << states.rotation.get();
        if(states.scale.hasValue())      mWriter << states.scale.get();
        if(states.visibility.hasValue()) mWriter << (states.visibility.get() ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0));
        _endPacket();
    }

    void PacketBuilder::objectLifecycleStateChanged(odState::TickNumber tick, od::LevelObjectId id, od::ObjectLifecycleState state)
    {
        auto stateCode = static_cast<std::underlying_type_t<od::ObjectLifecycleState>>(state);
        if(stateCode > std::numeric_limits<uint8_t>::max())
        {
            throw od::Exception("State code exceed limits of state field");
        }

        _beginPacket(PacketType::OBJECT_LIFECYCLE_STATE_CHANGED);
        mWriter << tick
                << id
                << static_cast<uint8_t>(stateCode);
        _endPacket();
    }

    void PacketBuilder::confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick)
    {
        _beginPacket(PacketType::CONFIRM_SNAPSHOT);
        mWriter << tick
                << realtime
                << static_cast<uint32_t>(discreteChangeCount)
                << referenceTick;
        _endPacket();
    }

    void PacketBuilder::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        _beginPacket(PacketType::GLOBAL_MESSAGE);
        mWriter << code;
        mWriter.write(data, size);
        _endPacket();
    }

    void PacketBuilder::actionTriggered(odInput::ActionCode code, odInput::ActionState state)
    {
        _beginPacket(PacketType::ACTION_TRIGGERED);
        mWriter << code << static_cast<uint8_t>(state);
        _endPacket();
    }

    void PacketBuilder::analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes)
    {
        _beginPacket(PacketType::ANALOG_ACTION_TRIGGERED);
        mWriter << code << axes.x << axes.y;
        _endPacket();
    }

    void PacketBuilder::acknowledgeSnapshot(odState::TickNumber tick)
    {
        _beginPacket(PacketType::ACKNOWLEDGE_SNAPSHOT);
        mWriter << tick;
        _endPacket();
    }

    void PacketBuilder::_beginPacket(PacketType type)
    {
        mPacketBuffer.clear();
        //mOutputStream.seekp(0);

        uint16_t dummyPayloadSize = 0;
        mWriter << static_cast<uint8_t>(type) << dummyPayloadSize;
    }

    void PacketBuilder::_endPacket()
    {
        mOutputStream.flush();

        size_t payloadSize = mPacketBuffer.size() - PacketConstants::HEADER_SIZE;
        if(payloadSize > 0xffff)
        {
            throw od::Exception("Packet payload size exceeds size fields limits");
        }

        //mOutputStream.seekp(1);
        //mWriter << static_cast<uint16_t>(payloadSize);

        // FIXME: bad hack! while seeking is unimplemented, we have to overwrite the payload size field manually
        mPacketBuffer[1] = (payloadSize >> 0) & 0xff;
        mPacketBuffer[2] = (payloadSize >> 8) & 0xff;

        if(mPacketCallback != nullptr)
        {
            mPacketCallback(mPacketBuffer.data(), mPacketBuffer.size());
        }
    }


}
