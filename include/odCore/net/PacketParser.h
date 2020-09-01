
#ifndef INCLUDE_ODCORE_NET_PACKETPARSER_H_
#define INCLUDE_ODCORE_NET_PACKETPARSER_H_

#include <memory>

namespace odNet
{
    class DownlinkConnector;

    class DownlinkPacketParser
    {
    public:

        DownlinkPacketParser(std::shared_ptr<DownlinkConnector> output);
        ~DownlinkPacketParser();

        void parse(const char *data, size_t size);


    private:

        std::shared_ptr<DownlinkConnector> mOutput;

    };

}

#endif
