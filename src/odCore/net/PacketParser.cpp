
#include <odCore/net/PacketParser.h>

#include <odCore/net/DownlinkConnector.h>

namespace odNet
{

    DownlinkPacketParser::DownlinkPacketParser(std::shared_ptr<DownlinkConnector> output)
    : mOutput(output)
    {
    }

    DownlinkPacketParser::~DownlinkPacketParser()
    {
    }

    void DownlinkPacketParser::parse(const char *data, size_t size)
    {

    }

}
