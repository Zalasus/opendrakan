

/*
 * @file LocalTunnel.h
 * @author zal
 *
 * ServerConnector and ClientConnector are designed to be able to "plug" right
 * into a Client/Server, so an absolute-zero-latency connection is possible if
 * the client and server run on the same machine.
 *
 * This is not always desireable. One might want to simulate network latency or
 * simply test the network protocol. This header defines a set of classes that
 * create a buffer-backed tunnel between Server-/ClientConnectors that uses the
 * network protocol, and can also simulate latency and packet drops.
 */

#ifndef INCLUDE_ODCORE_NET_LOCALTUNNEL_H_
#define INCLUDE_ODCORE_NET_LOCALTUNNEL_H_

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

#include <odCore/net/PacketParser.h>
#include <odCore/net/PacketBuilder.h>

namespace odNet
{
    class UplinkConnector;
    class DownlinkConnector;

    class LocalTunnel
    {
    public:

        LocalTunnel(std::shared_ptr<DownlinkConnector> downlinkOutput, std::shared_ptr<UplinkConnector> uplinkOutput);
        ~LocalTunnel();

        inline std::shared_ptr<DownlinkConnector> getDownlinkInput() { return mDownlinkPacketBuilder; }
        inline std::shared_ptr<UplinkConnector> getUplinkInput() { return mUplinkPacketBuilder; }

        inline void setDropRate(double dropRate) { mDropRate = dropRate; }

        void setLatency(double min, double max);


    private:

        using time_point = std::chrono::high_resolution_clock::time_point;

        static constexpr size_t MAX_PAYLOAD_SIZE = 512;

        struct Packet
        {
            std::array<char, MAX_PAYLOAD_SIZE> data;
            size_t size;
            time_point arriveBy;
            bool isUplink;
        };

        bool _shouldDrop();
        double _getRandomLatency();
        void _addPacket(const char *data, size_t size, PacketBuilder::LinkType linkType, bool isUplink);
        void _parsePacket(const char *data, size_t size, bool isUplink);
        void _dispatchThreadWorkerFunc();

        std::shared_ptr<PacketBuilder> mDownlinkPacketBuilder;
        PacketParser mDownlinkPacketParser;

        std::shared_ptr<PacketBuilder> mUplinkPacketBuilder;
        PacketParser mUplinkPacketParser;

        double mLatencyMin;
        double mLatencyMax;
        double mDropRate;

        std::thread mDispatchThread;
        bool mDispatchThreadStarted;
        std::atomic_bool mTerminateDispatchThread;
        std::deque<Packet> mPacketBuffer;
        std::mutex mPacketBufferMutex;
        std::condition_variable mDispatchCondition;

    };

}

#endif
