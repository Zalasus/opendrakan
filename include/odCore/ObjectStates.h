
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
            OD_STATE(position)
            OD_STATE(rotation)
            OD_STATE(scale)
            OD_STATE(visibility)
            OD_STATE(running)
        OD_END_STATE_LIST()

        odState::State<glm::vec3, StateFlags::LERPED> position;
        odState::State<glm::quat, StateFlags::LERPED> rotation;
        odState::State<glm::vec3, StateFlags::LERPED> scale;
        odState::State<bool>                          visibility;
        odState::State<bool>                          running;
    };

}

#endif
