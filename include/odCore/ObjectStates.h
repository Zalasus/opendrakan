
#ifndef INCLUDE_ODCORE_OBJECTSTATES_H_
#define INCLUDE_ODCORE_OBJECTSTATES_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/state/State.h>

namespace od
{
    using odState::StateFlags;

    struct ObjectStates final : public odState::StateBundle<ObjectStates>
    {

        OD_BEGIN_STATE_LIST()
            OD_STATE(position,   StateFlags::LERPED)
            OD_STATE(rotation,   StateFlags::LERPED)
            OD_STATE(scale,      StateFlags::LERPED)
            OD_STATE(visibility, 0)
            OD_STATE(running,    0)
        OD_END_STATE_LIST()

        odState::State<glm::vec3> position;
        odState::State<glm::quat> rotation;
        odState::State<glm::vec3> scale;
        odState::State<bool>      visibility;
        odState::State<bool>      running;
    };

}

#endif
