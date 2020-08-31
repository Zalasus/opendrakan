
#ifndef INCLUDE_ODCORE_STATE_STATE_H_
#define INCLUDE_ODCORE_STATE_STATE_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace odState
{

    class ObjectStateChange
    {
    public:

        ObjectStateChange();

        inline bool isTranslated() const { return mFlags & TRANSLATION; }
        inline bool isRotated() const { return mFlags & ROTATION; }
        inline bool isScaled() const { return mFlags & SCALE; }
        inline bool isVisibilityChanged() const { return mFlags & VISIBILITY; }
        inline bool isAnimationFrameChanged() const { return mFlags & ANIMATION_FRAME; }

        inline glm::vec3 getTranslation() const { return mTranslation; }
        inline glm::quat getRotation() const { return mRotation; }
        inline glm::vec3 getScale() const { return mScale; }
        inline bool getVisibility() const { return mVisibility; }
        inline float getAnimationFrameTime() const { return mAnimationFrameTime; }

        template <typename T>
        void visit(T &t) const
        {
            if(isTranslated()) t.translated(mTranslation);
            if(isRotated()) t.rotated(mRotation);
            if(isScaled()) t.scaled(mScale);
            if(isVisibilityChanged()) t.visibilityChanged(mVisibility);
            if(isAnimationFrameChanged()) t.animationFrameChanged(mAnimationFrameTime);
        }

        void setTranslation(const glm::vec3 &p);
        void setRotation(const glm::quat &r);
        void setScale(const glm::vec3 &s);
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

        /**
         * @bried Creates a new ObjectStateChange where states appearing in both this and rhs are linearly interpolated.
         *
         * States that only appear in this and not in rhs are added to the result without interpolation. States present
         * in rhs but not in this will be ignored.
         *
         * @param delta  A value between 0.0 and 1.0 (not enforced).
         */
        ObjectStateChange lerp(const ObjectStateChange &rhs, float delta) const;

        /**
         * @brief Removes from this all states that are the same in this and rhs.
         */
        void removeSteadyStates(const ObjectStateChange &rhs);


    private:

        static constexpr uint32_t TRANSLATION        = (1 << 0);
        static constexpr uint32_t ROTATION           = (1 << 1);
        static constexpr uint32_t SCALE              = (1 << 2);
        static constexpr uint32_t VISIBILITY         = (1 << 3);
        static constexpr uint32_t ANIMATION_FRAME    = (1 << 4);
        static constexpr uint32_t CUSTOM             = (1 << 5);

        uint32_t mFlags;

        glm::vec3 mTranslation;
        glm::quat mRotation;
        glm::vec3 mScale;
        bool mVisibility;
        float mAnimationFrameTime;

    };

}

#endif
