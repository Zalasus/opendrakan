
#ifndef INCLUDE_ODCORE_OBJECTSTATES_H_
#define INCLUDE_ODCORE_OBJECTSTATES_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/state/StateBundle.h>

namespace od
{
    struct ObjectStates;
}

namespace odState
{
    // since StateBundle can be quite a heavy template to instantiate, we moved
    //  the explicit instatiation to LevelObject.cpp to reduce the compiler
    //  overhead a bit (hopefully. not sure whether this even does anything when
    //  inheriting from a template).
    extern template class StateBundle<od::ObjectStates>;
}

namespace od
{

    struct ObjectStates final : public odState::StateBundle<ObjectStates>
    {

        OD_BEGIN_STATE_LIST()
            OD_STATE(position,   odState::StateFlags::LERPED)
            OD_STATE(rotation,   odState::StateFlags::LERPED)
            OD_STATE(scale,      odState::StateFlags::LERPED)
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
