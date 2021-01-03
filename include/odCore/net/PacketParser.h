
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
    class UplinkConnector;

    /**
     * Parses packets and turns them into calls of the appropriate connector
     * methods.
     *
     * This only does protocol validation, not data validation. You might get
     * bad object IDs, tick numbers etc. via the output connector if the server
     * misbehaves.
     */
    class PacketParser
    {
    public:

        PacketParser(std::shared_ptr<DownlinkConnector> downlinkOutput, std::shared_ptr<UplinkConnector> uplinkOutput);
        ~PacketParser();

        /**
         * @return The number of bytes of input that were used.
         */
        size_t parse(const char *data, size_t size);


    private:

        void _parsePacket(uint8_t type, uint16_t length, od::DataReader &dr, const char *rawPayload);
        void _badPacket(const char *reason);

        std::shared_ptr<DownlinkConnector> mDownlinkOutput;
        std::shared_ptr<UplinkConnector> mUplinkOutput;

        size_t mBadPacketCount;

    };

}

#endif
