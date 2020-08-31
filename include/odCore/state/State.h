
#ifndef INCLUDE_ODCORE_STATE_STATE_H_
#define INCLUDE_ODCORE_STATE_STATE_H_

#include <odCore/ObjectTransform.h>

namespace od
{
    class LevelObject;
}

namespace odState
{

    class ObjectStateChange
    {
    public:

        ObjectStateChange();

        inline bool transformed() const { return mFlags & TRANSFORMED; }
        inline bool visibilityChanged() const { return mFlags & VISIBILITY_CHANGED; }
        inline bool animationFrame() const { return mFlags & ANIMATION_FRAME; }

        inline const od::ObjectTransform &getTransform() const { return mTransform; }
        inline bool getVisibility() const { return mVisibility; }
        inline float getAnimationFrameTime() const { return mAnimationFrameTime; }

        void setTransform(const od::ObjectTransform &tf);
        void setVisibility(bool b);
        void setAnimationFrame(float t);

        /**
         * @brief Applies the transition t on top of this one. States unaffected by t will retain their original value.
         */
        void merge(const ObjectStateChange &t);

        /**
         * @brief Returns the number of discrete state changes in this object.
         *
         * Transform, visibility change, animation etc. each count as one
         * discrete change.
         */
        size_t getDiscreteChangeCount() const;

        void apply(od::LevelObject &obj);

        /**
         * @brief Applies to the object a linear interpolation between this and another state.
         *
         * States present in this but not in rhs will be applied without
         * interpolation. State present in rhs but not in this will be ignored.
         *
         * @param delta  A value between 0.0 and 1.0 (not enforced).
         */
        void applyInterpolated(od::LevelObject &obj, const ObjectStateChange &rhs, float delta);

        /**
         * @brief Removes from this all states that are the same in this and rhs.
         */
        void removeSteadyStates(const ObjectStateChange &rhs);


    private:

        static constexpr uint32_t TRANSFORMED        = (1 << 0);
        static constexpr uint32_t VISIBILITY_CHANGED = (1 << 1);
        static constexpr uint32_t ANIMATION_FRAME    = (1 << 2);
        static constexpr uint32_t CUSTOM             = (1 << 3);

        uint32_t mFlags;

        od::ObjectTransform mTransform;
        bool mVisibility;
        float mAnimationFrameTime;

    };

}

#endif
