
#include <odCore/state/State.h>

namespace odState
{

    ObjectStateChange::ObjectStateChange()
    : mFlags(0)
    {
    }

    void ObjectStateChange::setTransform(const od::ObjectTransform &tf)
    {
        mTransform = tf;
        mFlags |= TRANSFORMED;
    }

    void ObjectStateChange::setVisibility(bool b)
    {
        mVisibility = b;
        mFlags |= VISIBILITY_CHANGED;
    }

    void ObjectStateChange::setAnimationFrame(float t)
    {
        mAnimationFrameTime = t;
        mFlags |= ANIMATION_FRAME;
    }

    /**
     * @brief Applies the transition t on top of this one. States unaffected by t will retain their original value.
     */
    void ObjectStateChange::merge(const ObjectStateChange &t)
    {
        if(t.transformed())
        {
            setTransform(t.mTransform);
        }

        if(t.visibilityChanged())
        {
            setVisibility(t.mVisibility);
        }

        if(t.animationFrame())
        {
            setAnimationFrame(t.mAnimationFrameTime);
        }
    }

    size_t ObjectStateChange::getDiscreteChangeCount() const
    {
        return transformed() + visibilityChanged() + animationFrame();
    }

}
