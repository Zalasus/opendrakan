
#include <odCore/state/State.h>

namespace odState
{

    ObjectStateChange::ObjectStateChange()
    : mFlags(0)
    {
    }

    void ObjectStateChange::setTranslation(const glm::vec3 &p)
    {
        mTranslation = p;
        mFlags |= TRANSLATION;
    }

    void ObjectStateChange::setRotation(const glm::quat &r)
    {
        mRotation = r;
        mFlags |= ROTATION;
    }

    void ObjectStateChange::setScale(const glm::vec3 &s)
    {
        mScale = s;
        mFlags |= SCALE;
    }

    void ObjectStateChange::setVisibility(bool b)
    {
        mVisibility = b;
        mFlags |= VISIBILITY;
    }

    void ObjectStateChange::setAnimationFrame(float t)
    {
        mAnimationFrameTime = t;
        mFlags |= ANIMATION_FRAME;
    }

    void ObjectStateChange::merge(const ObjectStateChange &t)
    {
        // NOTE: by implementing all these operations as visitors, we get compile errors when we add states but don't account for them in all operations. neato!
        struct MergeVisitor
        {
            ObjectStateChange &me;

            MergeVisitor(ObjectStateChange &_me)
            : me(_me)
            {
            }

            void translated(const glm::vec3 &p) { me.setTranslation(p); }
            void rotated(const glm::quat &r) { me.setRotation(r); }
            void scaled(const glm::vec3 &s) { me.setScale(s); }
            void visibilityChanged(bool b) { me.setVisibility(b); }
            void animationFrameChanged(float t) { me.setAnimationFrame(t); }
        };

        MergeVisitor visitor(*this);
        t.visit(visitor);
    }

    size_t ObjectStateChange::getDiscreteChangeCount() const
    {
        // hackish bit-twiddling for counting one-bits in flag field.
        //  source: https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
        uint32_t v = mFlags;
        v = v - ((v >> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
        return (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
    }

    ObjectStateChange ObjectStateChange::lerp(const ObjectStateChange &rhs, float delta) const
    {
        struct LerpVisitor
        {
            const ObjectStateChange &rhs;
            ObjectStateChange result;
            float delta;

            LerpVisitor(const ObjectStateChange &_rhs, float _delta)
            : rhs(_rhs)
            , delta(_delta)
            {
            }

            void translated(const glm::vec3 &p)
            {
                if(rhs.isTranslated())
                {
                    result.setTranslation(glm::mix(p, rhs.mTranslation, delta));

                }else
                {
                    result.setTranslation(p);
                }
            }

            void rotated(const glm::quat &r)
            {
                if(rhs.isRotated())
                {
                    result.setRotation(glm::slerp(r, rhs.mRotation, delta));

                }else
                {
                    result.setRotation(r);
                }
            }

            void scaled(const glm::vec3 &s)
            {
                if(rhs.isScaled())
                {
                    result.setScale(glm::mix(s, rhs.mScale, delta));

                }else
                {
                    result.setScale(s);
                }
            }

            void visibilityChanged(bool b)
            {
                // can't interpolate this
                result.setVisibility(b);
            }

            void animationFrameChanged(float t)
            {
                if(rhs.isAnimationFrameChanged())
                {
                    float lerpedTime = glm::mix(t, rhs.mAnimationFrameTime, delta);
                    result.setAnimationFrame(lerpedTime);

                }else
                {
                    result.setAnimationFrame(t);
                }
            }
        };

        LerpVisitor visitor(rhs, delta);
        this->visit(visitor);

        return visitor.result;
    }

    void ObjectStateChange::removeSteadyStates(const ObjectStateChange &rhs)
    {
        // TODO: epsilon

        struct RemoveSteadyStatesVisitor
        {
            ObjectStateChange &me;

            RemoveSteadyStatesVisitor(ObjectStateChange &_me)
            : me(_me)
            {
            }

            void translated(const glm::vec3 &p)
            {
                if(me.isTranslated() && me.mTranslation == p) me.mFlags &= ~TRANSLATION;
            }

            void rotated(const glm::quat &r)
            {
                if(me.isRotated() && me.mRotation == r) me.mFlags &= ~ROTATION;
            }

            void scaled(const glm::vec3 &s)
            {
                 if(me.isScaled() && me.mScale == s) me.mFlags &= ~SCALE;
            }

            void visibilityChanged(bool b)
            {
                if(me.isVisibilityChanged() && me.mVisibility == b) me.mFlags &= ~VISIBILITY;
            }

            void animationFrameChanged(float t)
            {
                if(me.isAnimationFrameChanged() && me.mAnimationFrameTime == t) me.mFlags &= ~ANIMATION_FRAME;
            }
        };

        RemoveSteadyStatesVisitor visitor(*this);
        rhs.visit(visitor);
    }

}
