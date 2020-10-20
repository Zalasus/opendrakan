
#include <odCore/net/LocalTunnel.h>

#include <algorithm>
#include <random>

#include <odCore/ThreadUtils.h>

namespace odNet
{
    constexpr size_t LocalTunnel::MAX_PAYLOAD_SIZE; // pre-C++17 bullshit (no inline vars)

    static thread_local std::random_device seedSource;
    static thread_local std::minstd_rand randomEngine(seedSource());
    static thread_local std::uniform_real_distribution<double> dropDistribution(0.0, 1.0);

    LocalTunnel::LocalTunnel(std::shared_ptr<DownlinkConnector> downlinkOutput, std::shared_ptr<UplinkConnector> uplinkOutput)
    : mDownlinkPacketParser(downlinkOutput, nullptr)
    , mUplinkPacketParser(nullptr, uplinkOutput)
    , mLatencyMin(0.0)
    , mLatencyMax(0.0)
    , mDropRate(0.0)
    , mDispatchThreadStarted(false)
    , mTerminateDispatchThread(false)
    {
        auto downlinkPacketCallback = [this](const char *data, size_t size, PacketBuilder::LinkType linkType)
        {
            _addPacket(data, size, linkType, false);
        };
        mDownlinkPacketBuilder = std::make_shared<PacketBuilder>(downlinkPacketCallback);

        auto uplinkPacketCallback = [this](const char *data, size_t size, PacketBuilder::LinkType linkType)
        {
            _addPacket(data, size, linkType, true);
        };
        mUplinkPacketBuilder = std::make_shared<PacketBuilder>(uplinkPacketCallback);
    }

    LocalTunnel::~LocalTunnel()
    {
        mTerminateDispatchThread.store(true, std::memory_order_release);
        mDispatchCondition.notify_all();
        if(mDispatchThread.joinable())
        {
            mDispatchThread.join();
        }
    }

    void LocalTunnel::setLatency(double min, double max)
    {
        if(min < 0 || min > max)
        {
            throw od::Exception("Invalid latency value");
        }

        mLatencyMin = min;
        mLatencyMax = max;

        // we only start the dispatch thread on demand (it is never stopped, though)
        if(!mDispatchThreadStarted && (mLatencyMin > 0 || mLatencyMax > 0))
        {
            mTerminateDispatchThread = false;
            mDispatchThread = std::thread([this](){ this->_dispatchThreadWorkerFunc(); });
            od::ThreadUtils::setThreadName(mDispatchThread, "localtunnel");
            mDispatchThreadStarted = true;
        }
    }

    bool LocalTunnel::_shouldDrop()
    {
        return dropDistribution(randomEngine) < mDropRate;
    }

    double LocalTunnel::_getRandomLatency()
    {
        double r = dropDistribution(randomEngine);
        return r*(mLatencyMax - mLatencyMin) + mLatencyMin;
    }

    void LocalTunnel::_addPacket(const char *data, size_t size, PacketBuilder::LinkType linkType, bool isUplink)
    {
        if(size > MAX_PAYLOAD_SIZE)
        {
            Logger::warn() << "Packet payload size " << size << " exceeds payload size limit of " << MAX_PAYLOAD_SIZE << ". Dropping packet";
            return;
        }

        if(linkType == PacketBuilder::LinkType::UNRELIABLE && _shouldDrop())
        {
            return;
        }

        double latency = _getRandomLatency();
        if(latency > 0.0)
        {
            std::lock_guard<std::mutex> lock(mPacketBufferMutex);

            auto arriveBy = std::chrono::high_resolution_clock::now() + std::chrono::nanoseconds(static_cast<long int>(latency*1e9));

            // maintain ascending order in packet buffer!
            if(!mPacketBuffer.empty() && mPacketBuffer.back().arriveBy > arriveBy)
            {
                arriveBy = mPacketBuffer.back().arriveBy;
            }

            mPacketBuffer.emplace_back();
            auto &packet = mPacketBuffer.back();

            std::copy_n(data, size, packet.data.begin());
            packet.size = size;
            packet.arriveBy = arriveBy;
            packet.isUplink = isUplink;

            if(mPacketBuffer.size() == 1)
            {
                // if this is the only packet in the buffer, that means the
                //  buffer was empty before, which means the dispatch thread is
                //  waiting indefinitely until notified.
                mDispatchCondition.notify_all();
            }

        }else
        {
            // no latency. bypass the buffer and parse packet directly.
            _parsePacket(data, size, isUplink);
        }
    }

    void LocalTunnel::_parsePacket(const char *data, size_t size, bool isUplink)
    {
        size_t usedBytes = isUplink ? mUplinkPacketParser.parse(data, size) : mDownlinkPacketParser.parse(data, size);
        if(usedBytes != size)
        {
            throw od::Exception("Parser did not use all packet bytes, but it should have");
        }
    }

    void LocalTunnel::_dispatchThreadWorkerFunc()
    {
        std::unique_lock<std::mutex> lock(mPacketBufferMutex);

        while(!mTerminateDispatchThread.load(std::memory_order_acquire))
        {
            if(mPacketBuffer.empty())
            {
                mDispatchCondition.wait(lock);

            }else
            {
                if(mPacketBuffer.front().arriveBy > std::chrono::high_resolution_clock::now())
                {
                    mDispatchCondition.wait_until(lock, mPacketBuffer.front().arriveBy);
                }
            }

            // dispatch all packets that should have arrived by now
            auto now = std::chrono::high_resolution_clock::now();
            while(!mPacketBuffer.empty())
            {
                auto &packet = mPacketBuffer.front();
                if(packet.arriveBy > now)
                {
                    break;
                }

                _parsePacket(packet.data.data(), packet.size, packet.isUplink);

                Logger::info() << "dispatched delayed packet in " << (packet.isUplink ? "uplink" : "downlink");

                mPacketBuffer.pop_front();
            }
        }
    }

}
