
#ifndef INCLUDE_ODCORE_STATE_STATE_H_
#define INCLUDE_ODCORE_STATE_STATE_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/Downcast.h>

namespace odState
{

    struct StateFlags
    {
        using Type = int;

        static const Type NOT_SAVED = (1 << 0);
        static const Type NETWORKED = (1 << 1);
        static const Type LERPED    = (1 << 2);
    };


    template <typename T>
    struct StateLerp
    {
        T operator()(const T &left, const T &right, float delta)
        {
            return glm::mix(left, right, delta);
        }
    };

    template <>
    struct StateLerp<glm::quat>
    {
        glm::quat operator()(const glm::quat &left, const glm::quat &right, float delta)
        {
            return glm::slerp(left, right, delta);
        }
    };


    /**
     * A template for a simple state type. This should handle most basic types
     * of states (ints, floats, glm vectors etc.).
     *
     * This works like an Optional, so this can either contain a value or not.
     *
     * You don't have to use this (though you probably should). Any type can be
     * a state, as long as it meets the following requirements:
     *  - DefaultConstructible, CopyAssignable, EqualityComparable
     *  - Defines a static lerp() function
     *  - Defines a non-static hasValue() method
     */
    template <typename T, StateFlags::Type _Flags>
    struct State
    {
    public:

        using ThisType = State<T, _Flags>;

        State()
        : mHasValue(false)
        , mValue()
        {
        }

        State(T v)
        : mHasValue(true)
        , mValue(v)
        {
        }


        class LerpableTag {};
        class NonLerpableTag {};
        using LerpTagType = typename std::conditional<_Flags & StateFlags::LERPED, LerpableTag, NonLerpableTag>::type;

        static ThisType lerp(const ThisType &left, const ThisType &right, float delta)
        {
            return lerpImpl(left, right, delta, LerpTagType());
        }

        static ThisType lerpImpl(const ThisType &left, const ThisType &right, float delta, NonLerpableTag)
        {
            return left.mValue;
        }

        static ThisType lerpImpl(const ThisType &left, const ThisType &right, float delta, LerpableTag)
        {
            StateLerp<T> lerper;
            return lerper(left.mValue, right.mValue, delta);
        }

        bool hasValue() const { return mHasValue; }

        bool operator==(const ThisType &rhs) const
        {
            return mValue == rhs.mValue;
        }

        operator T() const
        {
            return mValue;
        }


    private:

        bool mHasValue;
        T mValue;
    };


    template <typename _Bundle>
    class StateMergeOp
    {
    public:

        StateMergeOp(_Bundle &left, _Bundle &right, _Bundle &result)
        : mLeftBundle(left)
        , mRightBundle(right)
        , mResultBundle(result)
        {
        }

        template <typename _State>
        StateMergeOp &operator()(_State _Bundle::* state)
        {
            if((mRightBundle.*state).hasValue())
            {
                mResultBundle.*state = mRightBundle.*state;

            }else
            {
                mResultBundle.*state = mLeftBundle.*state;
            }

            return *this;
        }


    private:

        _Bundle &mLeftBundle;
        _Bundle &mRightBundle;
        _Bundle &mResultBundle;
    };


    template <typename _Bundle>
    class StateLerpOp
    {
    public:

        StateLerpOp(_Bundle &left, _Bundle &right, _Bundle &result, float delta)
        : mLeftBundle(left)
        , mRightBundle(right)
        , mResultBundle(result)
        , mDelta(delta)
        {
        }

        template <typename _State>
        StateLerpOp &operator()(_State _Bundle::* state)
        {
            mResultBundle.*state = _State::lerp(mLeftBundle.*state, mRightBundle.*state, mDelta);
            return *this;
        }


    private:

        _Bundle &mLeftBundle;
        _Bundle &mRightBundle;
        _Bundle &mResultBundle;
        float mDelta;
    };


    template <typename _Bundle>
    class StateDeltaEncOp
    {
    public:

        StateDeltaEncOp(_Bundle &left, _Bundle &right, _Bundle &result)
        : mLeftBundle(left)
        , mRightBundle(right)
        , mResultBundle(result)
        {
        }

        template <typename _State>
        StateDeltaEncOp &operator()(_State _Bundle::* state)
        {
            if((mRightBundle.*state).hasValue() && mLeftBundle.*state == mRightBundle.*state)
            {
                mResultBundle.*state = mRightBundle.*state;

            }else
            {
                mResultBundle.*state = _State();
            }

            return *this;
        }


    private:

        _Bundle &mLeftBundle;
        _Bundle &mRightBundle;
        _Bundle &mResultBundle;
    };


    struct StateBundleBase
    {
        virtual ~StateBundleBase() = default;
        virtual void merge(StateBundleBase &rhs) = 0;
        virtual void lerp(StateBundleBase &rhs, float delta) = 0;
        virtual void deltaEncode(StateBundleBase &rhs) = 0;
    };


    template <typename _Bundle>
    class StateBundle : public StateBundleBase
    {
    public:

        void merge(_Bundle &rhs)
        {
            auto &lhs = static_cast<_Bundle&>(*this);
            StateMergeOp<_Bundle> op(lhs, rhs, lhs);
            _Bundle::stateOp(op);
        }

        virtual void merge(StateBundleBase &right) override final
        {
            auto &rhs = *(od::downcast<_Bundle>(&right));
            merge(rhs);
        }

        void lerp(_Bundle &rhs, float delta)
        {
            auto &lhs = static_cast<_Bundle&>(*this);
            StateLerpOp<_Bundle> op(lhs, rhs, lhs, delta);
            _Bundle::stateOp(op);
        }

        virtual void lerp(StateBundleBase &right, float delta) override final
        {
            auto &rhs = *(od::downcast<_Bundle>(&right));
            lerp(rhs, delta);
        }

        void deltaEncode(_Bundle &rhs)
        {
            auto &lhs = static_cast<_Bundle&>(*this);
            StateDeltaEncOp<_Bundle> op(lhs, rhs, lhs);
            _Bundle::stateOp(op);
        }

        virtual void deltaEncode(StateBundleBase &right) override final
        {
            auto &rhs = *(od::downcast<_Bundle>(&right));
            deltaEncode(rhs);
        }


    private:

        friend _Bundle;

        // private constructor, but _Bundle is a friend, so it is the only thing that can construct us
        StateBundle() = default;

    };


    struct ObjectState : public StateBundle<ObjectState>
    {
        template <typename T>
        static void stateOp(T &op)
        {
            op(&ObjectState::position)
              (&ObjectState::rotation)
              (&ObjectState::scale)
              (&ObjectState::visibility);
        }

        State<glm::vec3, StateFlags::NETWORKED | StateFlags::LERPED> position;
        State<glm::quat, StateFlags::NETWORKED | StateFlags::LERPED> rotation;
        State<glm::vec3, StateFlags::NETWORKED | StateFlags::LERPED> scale;
        State<bool,      StateFlags::NETWORKED>                      visibility;
    };


    // TODO: this whole thing could be wrapped in a copy-on-write-pointer thingy
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

        // NOTE: by implementing all these operations as visitors, we get compile errors when we add states but don't account for them in all operations. neato!
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
         * @brief Creates a copy of this state, applies the changes in t on top of the copy and returns it.
         *
         * States unaffected by t will retain their original value.
         */
        ObjectStateChange merge(const ObjectStateChange &t) const;

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
