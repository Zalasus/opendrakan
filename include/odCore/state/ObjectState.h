
#ifndef INCLUDE_ODCORE_STATE_OBJECTSTATE_H_
#define INCLUDE_ODCORE_STATE_OBJECTSTATE_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace od
{
    class Layer;
}

namespace odState
{

    /**
     * @brief Interface abstracting state changes that can happen on a level object.
     *
     * The idea is that there are two implementations of this provided by a level object:
     * one that applies the changes directly to the object and it's render/physics/etc. handles,
     * and another one that does exactly that but also tells the StateManager and the RFL class
     * instance about the change. This way, we have one state handle that affects only the level
     * state and another one that also can trigger game mechanics (by notifying the RFLclass).
     *
     * The latter is the one the RFL class instance will use to make it's changes, while the former
     * will be used solely by the StateManager to move the level in into any state it needs without
     * causing side effects.
     *
     * This implies that any side effects that are triggered by the *recording* handle must, in some way
     * decay to state changes that are tracked.
     */
    class ObjectStateHandle
    {
    public:

        virtual ~ObjectStateHandle() = default;

        virtual void setPosition(const glm::vec3 &v) = 0;
        virtual void setRotation(const glm::quat &q) = 0;
        virtual void setScale(const glm::vec3 &scale) = 0;

        virtual void setVisible(bool v) = 0;

        virtual void setAssociatedLayer(od::Layer *l) = 0;

    };

}

#endif
