
#include <odCore/net/LocalTunnel.h>

#include <algorithm>
#include <random>

namespace odNet
{
    constexpr size_t LocalTunnel::MAX_PAYLOAD_SIZE; // pre-C++17 bullshit (no inline vars)

    static thread_local std::random_device seedSource;
    static thread_local std::minstd_rand randomEngine(seedSource());
    static thread_local std::uniform_real_distribution<double> dropDistribution(0.0, 1.0);

    LocalTunnel::LocalTunnel(std::shared_ptr<DownlinkConnector> downlinkOutput, std::shared_ptr<UplinkConnector> uplinkOutput)
    : mPacketParser(downlinkOutput, uplinkOutput)
    , mLatency(0.0)
    , mDropRate(0.0)
    {
        auto downlinkPacketCallback = [this](const char *data, size_t size)
        {
            _addPacket(data, size, mDownlinkPacketBuffer);
        };
        mDownlinkPacketBuilder = std::make_shared<PacketBuilder>(downlinkPacketCallback);

        auto uplinkPacketCallback = [this](const char *data, size_t size)
        {
            _addPacket(data, size, mUplinkPacketBuffer);
        };
        mUplinkPacketBuilder = std::make_shared<PacketBuilder>(uplinkPacketCallback);
    }

    bool LocalTunnel::_shouldDrop()
    {
        return dropDistribution(randomEngine) < mDropRate;
    }

    void LocalTunnel::_addPacket(const char *data, size_t size, std::deque<Packet> &buffer)
    {
        if(size > MAX_PAYLOAD_SIZE)
        {
            Logger::warn() << "Packet payload size " << size << " exceeds payload size limit of " << MAX_PAYLOAD_SIZE << ". Dropping packet";
            return;
        }

        if(_shouldDrop())
        {
            return;
        }

        if(mLatency > 0.0)
        {
            buffer.emplace_back();
            auto &packet = buffer.back();

            std::copy_n(data, size, packet.data.begin());
            packet.size = size;
            packet.sendTimer = mLatency;

        }else
        {
            size_t usedBytes = mPacketParser.parse(data, size);
            if(usedBytes != size)
            {
                throw od::Exception("Parser did not use all packet bytes, but it should have");
            }
        }
    }

}
