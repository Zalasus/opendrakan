
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
            OD_STATE(position)
            OD_STATE(rotation)
            OD_STATE(scale)
            OD_STATE(visibility)
            OD_STATE(running)
        OD_END_STATE_LIST()

        using odState::StateFlags::LERPED;
        using odState::StateFlags::DEFAULT;

        odState::State<glm::vec3, DEFAULT | LERPED> position;
        odState::State<glm::quat, DEFAULT | LERPED> rotation;
        odState::State<glm::vec3, DEFAULT | LERPED> scale;
        odState::State<bool>                        visibility;
        odState::State<bool>                        running;
    };

}

#endif
