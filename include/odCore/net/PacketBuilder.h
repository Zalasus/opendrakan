
#ifndef INCLUDE_ODCORE_NET_PACKETBUILDER_H_
#define INCLUDE_ODCORE_NET_PACKETBUILDER_H_

#include <vector>
#include <memory>

#include <odCore/net/DownlinkConnector.h>

namespace odNet
{

    class DownlinkPacketBuilder : public DownlinkConnector
    {
    public:

        DownlinkPacketBuilder();

        virtual void loadLevel(const std::string &path) override;
        virtual void objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states) override;
        virtual void spawnObject(od::LevelObjectId id) override;
        virtual void despawnObject(od::LevelObjectId id) override;
        virtual void destroyObject(od::LevelObjectId id) override;
        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount) override;


    private:

        std::vector<char> mPacketBuffer;

    };

}

#endif
