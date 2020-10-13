

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

#include <random>
#include <memory>

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

        inline std::shared_ptr<DownlinkConnector> getDownlinkInput() { return mDownlinkPacketBuilder; }
        inline std::shared_ptr<UplinkConnector> getUplinkInput() { return mUplinkPacketBuilder; }

        inline void setLatency(double latencySeconds) { mLatency = latencySeconds; }
        inline void setDropRate(double dropRate) { mDropRate = dropRate; }


    private:

        static constexpr size_t MAX_PAYLOAD_SIZE = 512;

        struct Packet
        {
            std::array<char, MAX_PAYLOAD_SIZE> data;
            size_t size;
            double sendTimer;
        };

        bool _shouldDrop();
        void _addPacket(const char *data, size_t size, std::deque<Packet> &buffer);

        PacketParser mPacketParser;

        std::shared_ptr<PacketBuilder> mDownlinkPacketBuilder;
        std::deque<Packet> mDownlinkPacketBuffer;

        std::shared_ptr<PacketBuilder> mUplinkPacketBuilder;
        std::deque<Packet> mUplinkPacketBuffer;

        double mLatency;

        double mDropRate;
        std::minstd_rand mRandomEngine;
        std::uniform_real_distribution<double> mDropDistribution;

    };

}

#endif
