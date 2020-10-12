
#ifndef INCLUDE_ODCORE_OBJECTSTATES_H_
#define INCLUDE_ODCORE_OBJECTSTATES_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/state/State.h>

namespace od
{
    using odState::StateFlags;

    struct ObjectStates : public odState::StateBundle<ObjectStates>
    {
        template <typename T>
        static void stateOp(T &op)
        {
            op(&ObjectStates::position)
              (&ObjectStates::rotation)
              (&ObjectStates::scale)
              (&ObjectStates::visibility);
        }


        odState::State<glm::vec3, StateFlags::NETWORKED | StateFlags::LERPED> position;
        odState::State<glm::quat, StateFlags::NETWORKED | StateFlags::LERPED> rotation;
        odState::State<glm::vec3, StateFlags::NETWORKED | StateFlags::LERPED> scale;
        odState::State<bool,      StateFlags::NETWORKED>                      visibility;
    };

    enum class ObjectLifecycleState
    {
        LOADED, // aka despawned
        SPAWNED, // aka stopped
        RUNNING,
        DESTROYED
    };

}

#endif
