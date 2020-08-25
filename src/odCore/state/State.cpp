
#include <odCore/state/State.h>

namespace odState
{

    ObjectStateTransition::ObjectStateTransition()
    : mFlags(0)
    {
    }

    void ObjectStateTransition::setTransform(const od::ObjectTransform &tf)
    {
        mTransform = tf;
        mFlags |= TRANSFORMED;
    }

    void ObjectStateTransition::setVisibility(bool b)
    {
        mVisibility = b;
        mFlags |= VISIBILITY_CHANGED;
    }

    void ObjectStateTransition::setAnimationFrame(float t)
    {
        mAnimationFrameTime = t;
        mFlags |= ANIMATION_FRAME;
    }

    /**
     * @brief Applies the transition t on top of this one. States unaffected by t will retain their original value.
     */
    void ObjectStateTransition::merge(const ObjectStateTransition &t)
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

}
