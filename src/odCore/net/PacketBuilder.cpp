
#include <odCore/net/PacketBuilder.h>

namespace odNet
{

    DownlinkPacketBuilder::DownlinkPacketBuilder()
    {
    }

    void DownlinkPacketBuilder::loadLevel(const std::string &path)
    {
    }

    void DownlinkPacketBuilder::objectTranslated(odState::TickNumber tick, od::LevelObjectId id, const glm::vec3 &p)
    {
    }

    void DownlinkPacketBuilder::objectRotated(odState::TickNumber tick, od::LevelObjectId id, const glm::quat &q)
    {

    }

    void DownlinkPacketBuilder::objectScaled(odState::TickNumber tick, od::LevelObjectId id, const glm::vec3 &s)
    {

    }

    void DownlinkPacketBuilder::objectVisibilityChanged(odState::TickNumber tick, od::LevelObjectId id, bool visible)
    {

    }

    void DownlinkPacketBuilder::spawnObject(od::LevelObjectId id)
    {

    }

    void DownlinkPacketBuilder::despawnObject(od::LevelObjectId id)
    {

    }

    void DownlinkPacketBuilder::destroyObject(od::LevelObjectId id)
    {

    }

    void DownlinkPacketBuilder::confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount)
    {

    }


}
