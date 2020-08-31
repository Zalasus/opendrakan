
#include <odCore/state/State.h>

#include <odCore/LevelObject.h>

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

    void ObjectStateChange::apply(od::LevelObject &obj)
    {
        if(transformed())
        {
            obj.transform(mTransform);
        }

        if(visibilityChanged())
        {
            obj.setVisible(mVisibility);
        }

        /*if(animationFrame())
        {
            obj.setAnimationFrame(mAnimationFrameTime);
        }*/
    }

    void ObjectStateChange::applyInterpolated(od::LevelObject &obj, const ObjectStateChange &rhs, float delta)
    {
        if(transformed())
        {
            if(rhs.transformed())
            {
                obj.transform(mTransform.lerp(rhs.mTransform, delta));

            }else
            {
                obj.transform(mTransform);
            }
        }

        if(visibilityChanged())
        {
            obj.setVisible(mVisibility);
        }

        if(animationFrame())
        {
            if(rhs.transformed())
            {
                float lerpedTime = glm::mix(mAnimationFrameTime, rhs.mAnimationFrameTime, delta);
                //obj.setAnimationFrame(lerpedTime);

            }else
            {
                //obj.setAnimationFrame(mAnimationFrameTime);
            }

        }
    }

    void ObjectStateChange::removeSteadyStates(const ObjectStateChange &rhs)
    {
        // TODO: epsilon
        
        if(transformed() && rhs.transformed() && mTransform == rhs.mTransform)
        {
            mFlags &= ~TRANSFORMED;
        }

        if(visibilityChanged() && rhs.visibilityChanged() && mVisibility == rhs.mVisibility)
        {
            mFlags &= ~VISIBILITY_CHANGED;
        }

        if(animationFrame() && rhs.animationFrame() && mAnimationFrameTime == rhs.mAnimationFrameTime)
        {
            mFlags &= ~ANIMATION_FRAME;
        }
    }

}
