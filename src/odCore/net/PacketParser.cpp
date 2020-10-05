
#include <odCore/net/PacketParser.h>

#include <odCore/net/Protocol.h>
#include <odCore/net/DownlinkConnector.h>

namespace odNet
{

    DownlinkPacketParser::DownlinkPacketParser(std::shared_ptr<DownlinkConnector> output)
    : mOutput(output)
    , mBadPacketCount(0)
    {
    }

    DownlinkPacketParser::~DownlinkPacketParser()
    {
    }

    size_t DownlinkPacketParser::parse(const char *data, size_t size)
    {
        if(size < PacketConstants::HEADER_SIZE)
        {
            // no need to parse anything when we don't even have a full header yet
            return 0;
        }

        od::MemoryInputBuffer buf(data, size);
        std::istream dataStr(&buf);
        od::DataReader dr(dataStr);

        uint8_t type;
        uint16_t length;
        dr >> type >> length;

        // is the packet available in full?
        if(size >= length + PacketConstants::HEADER_SIZE)
        {
            const char *rawPayload = data + PacketConstants::HEADER_SIZE;

            _parsePacket(type, length, dr, rawPayload);
            return length + PacketConstants::HEADER_SIZE; // TODO: consume more than one packet

        }else
        {
            return 0;
        }
    }

    void DownlinkPacketParser::_parsePacket(uint8_t type, uint16_t length, od::DataReader &dr, const char *rawPayload)
    {
        switch(static_cast<PacketType>(type))
        {
        case PacketType::LOAD_LEVEL:
            {
                std::string level(rawPayload, static_cast<size_t>(length));
                mOutput->loadLevel(level);
            }
            break;

        case PacketType::OBJECT_STATE_CHANGED:
            {
                if(length < PacketConstants::STATE_HEADER_SIZE)
                {
                    _badPacket("objstate unexpected size");
                    return;
                }

                odState::TickNumber tick;
                od::LevelObjectId id;
                uint32_t stateFlags;
                dr >> tick >> id >> stateFlags;

                if(length != getObjectStateChangePacketSize(stateFlags))
                {
                    _badPacket("objstate unexpected size");
                    return;
                }

                od::ObjectStates states;

                if(stateFlags & PacketConstants::STATE_MASK_POSITION)
                {
                    glm::vec3 pos;
                    dr >> pos;
                    states.position = pos;
                }

                if(stateFlags & PacketConstants::STATE_MASK_ROTATION)
                {
                    glm::quat rot;
                    dr >> rot;
                    states.rotation = rot;
                }

                if(stateFlags & PacketConstants::STATE_MASK_SCALE)
                {
                    glm::vec3 scale;
                    dr >> scale;
                    states.scale = scale;
                }

                if(stateFlags & PacketConstants::STATE_MASK_VISIBILITY)
                {
                    uint8_t vis;
                    dr >> vis;
                    states.visibility = vis;
                }

                mOutput->objectStatesChanged(tick, id, states);
            }
            break;

        case PacketType::CONFIRM_SNAPSHOT:
            {
                if(length != PacketConstants::CONFIRM_PAYLOAD_SIZE)
                {
                    _badPacket("snapconf unexpected size");
                    return;
                }

                odState::TickNumber tick;
                double realtime;
                uint32_t changeCount;
                dr >> tick >> realtime >> changeCount;

                mOutput->confirmSnapshot(tick, realtime, changeCount);
            }
            break;

        case PacketType::GLOBAL_MESSAGE:
            {
                MessageChannelCode code;
                dr >> code;

                mOutput->globalMessage(code, rawPayload + PacketConstants::GLOBAL_MESSAGE_HEADER_SIZE, length - PacketConstants::GLOBAL_MESSAGE_HEADER_SIZE);
            }
            break;

        default:
            // unknown packet type
            _badPacket("unknown type");
            break;
        }
    }

    void DownlinkPacketParser::_badPacket(const char *reason)
    {
        Logger::error() << "Bad packet: " << reason;
        mBadPacketCount++;
    }

}
