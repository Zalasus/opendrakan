
#include <odCore/net/PacketParser.h>

#include <odCore/net/Protocol.h>
#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/UplinkConnector.h>

namespace odNet
{

    PacketParser::PacketParser(std::shared_ptr<DownlinkConnector> downlinkOutput, std::shared_ptr<UplinkConnector> uplinkOutput)
    : mDownlinkOutput(downlinkOutput)
    , mUplinkOutput(uplinkOutput)
    , mBadPacketCount(0)
    {
    }

    PacketParser::~PacketParser()
    {
    }

    size_t PacketParser::parse(const char *data, size_t size)
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

            try
            {
                _parsePacket(type, length, dr, rawPayload);

            }catch(od::IoException &e) // kinda ugly to use this to detect an underrun condition. this could cause us to miss actually bad states in the data reader
            {
                _badPacket("unexpected size");
            }

            return length + PacketConstants::HEADER_SIZE; // TODO: consume more than one packet

        }else
        {
            return 0;
        }
    }

    void PacketParser::_parsePacket(uint8_t type, uint16_t length, od::DataReader &dr, const char *rawPayload)
    {
        switch(static_cast<PacketType>(type))
        {
        case PacketType::OBJECT_STATES_CHANGED:
            {
                odState::TickNumber tick;
                od::LevelObjectId id;
                dr >> tick >> id;

                od::ObjectStates states;
                states.deserialize(dr, odState::StateSerializationPurpose::NETWORK);

                if(mDownlinkOutput != nullptr)
                {
                    mDownlinkOutput->objectStatesChanged(tick, id, states);
                }
            }
            break;

        case PacketType::OBJECT_EXTRA_STATES_CHANGED:
            {
                _badPacket("extra state parsing unsupported ATM");
            }
            break;

        case PacketType::CONFIRM_SNAPSHOT:
            {
                odState::TickNumber tick;
                double realtime;
                uint32_t changeCount;
                odState::TickNumber referenceTick;
                dr >> tick >> realtime >> changeCount >> referenceTick;

                if(mDownlinkOutput != nullptr)
                {
                    mDownlinkOutput->confirmSnapshot(tick, realtime, changeCount, referenceTick);
                }
            }
            break;

        case PacketType::LOAD_LEVEL:
            {
                std::string level(rawPayload, static_cast<size_t>(length));

                if(mDownlinkOutput != nullptr)
                {
                    mDownlinkOutput->loadLevel(level);
                }
            }
            break;

        case PacketType::GLOBAL_MESSAGE:
            {
                MessageChannelCode code;
                dr >> code;

                if(mDownlinkOutput != nullptr)
                {
                    mDownlinkOutput->globalMessage(code, rawPayload + PacketConstants::GLOBAL_MESSAGE_HEADER_SIZE, length - PacketConstants::GLOBAL_MESSAGE_HEADER_SIZE);
                }
            }
            break;

        case PacketType::ACKNOWLEDGE_SNAPSHOT:
            {
                odState::TickNumber tick;
                dr >> tick;

                if(mUplinkOutput != nullptr)
                {
                    mUplinkOutput->acknowledgeSnapshot(tick);
                }
            }
            break;

        case PacketType::ACTION_TRIGGERED:
            {
                odInput::ActionCode code;
                uint8_t state;
                dr >> code >> state;

                if(mUplinkOutput != nullptr)
                {
                    mUplinkOutput->actionTriggered(code, static_cast<odInput::ActionState>(state)); // TODO: validate state value
                }
            }
            break;

        case PacketType::ANALOG_ACTION_TRIGGERED:
            {
                odInput::ActionCode code;
                float x;
                float y;
                dr >> code >> x >> y;

                if(mUplinkOutput != nullptr)
                {
                    mUplinkOutput->analogActionTriggered(code, {x, y});
                }
            }
            break;

        default:
            // unknown packet type
            _badPacket("unknown type");
            break;
        }
    }

    void PacketParser::_badPacket(const char *reason)
    {
        Logger::error() << "Bad packet: " << reason;
        mBadPacketCount++;
    }

}
