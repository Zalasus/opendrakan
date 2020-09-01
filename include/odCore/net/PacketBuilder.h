
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
        virtual void objectTranslated(odState::TickNumber tick, od::LevelObjectId id, const glm::vec3 &p) override;
        virtual void objectRotated(odState::TickNumber tick, od::LevelObjectId id, const glm::quat &q) override;
        virtual void objectScaled(odState::TickNumber tick, od::LevelObjectId id, const glm::vec3 &s) override;
        virtual void objectVisibilityChanged(odState::TickNumber tick, od::LevelObjectId id, bool visible) override;
        virtual void spawnObject(od::LevelObjectId id) override;
        virtual void despawnObject(od::LevelObjectId id) override;
        virtual void destroyObject(od::LevelObjectId id) override;
        virtual void confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount) override;


    private:

        std::vector<char> mPacketBuffer;

    };

}

#endif
