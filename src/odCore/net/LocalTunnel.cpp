
#include <odCore/net/LocalTunnel.h>

#include <algorithm>

namespace odNet
{

    LocalTunnel::LocalTunnel(std::shared_ptr<DownlinkConnector> downlinkOutput, std::shared_ptr<UplinkConnector> uplinkOutput)
    : mUplinkOutput(uplinkOutput)
    , mDownlinkPacketParser(downlinkOutput)
    , mLatency(0.0)
    , mDropRate(0.0)
    , mDropDistribution(0.0, 1.0)
    {
        auto downlinkPacketCallback = [this](const char *data, size_t size)
        {
            if(size > MAX_PAYLOAD_SIZE)
            {
                Logger::warn() << "Downlink packet payload size " << size << " exceeds payload size limit of " << MAX_PAYLOAD_SIZE << ". Dropping packet";
                return;
            }

            if(_shouldDrop())
            {
                return;
            }

            if(mLatency > 0.0)
            {
                mDownlinkPacketBuffer.emplace_back();
                auto &packet = mDownlinkPacketBuffer.back();

                std::copy_n(data, size, packet.data.begin());
                packet.size = size;
                packet.sendTimer = mLatency;

            }else
            {
                mDownlinkPacketParser.parse(data, size);
            }
        };

        mDownlinkPacketBuilder = std::make_shared<DownlinkPacketBuilder>(downlinkPacketCallback);
    }

    bool LocalTunnel::_shouldDrop()
    {
        return mDropDistribution(mRandomEngine) < mDropRate;
    }

}