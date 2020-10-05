
#ifndef INCLUDE_ODCORE_NET_PACKETPARSER_H_
#define INCLUDE_ODCORE_NET_PACKETPARSER_H_

#include <memory>

namespace od
{
    class DataReader;
}

namespace odNet
{
    class DownlinkConnector;

    class DownlinkPacketParser
    {
    public:

        DownlinkPacketParser(std::shared_ptr<DownlinkConnector> output);
        ~DownlinkPacketParser();

        /**
         * @return The number of bytes of input that were used.
         */
        size_t parse(const char *data, size_t size);


    private:

        void _parsePacket(uint8_t type, uint16_t length, od::DataReader &dr, const char *rawPayload);
        void _badPacket(const char *reason);

        std::shared_ptr<DownlinkConnector> mOutput;

        size_t mBadPacketCount;

    };

}

#endif
