
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
        using LerpTagType = typename std::conditional<((_Flags & StateFlags::LERPED) != 0), LerpableTag, NonLerpableTag>::type;

        static ThisType lerp(const ThisType &left, const ThisType &right, float delta)
        {
            return lerpImpl(left, right, delta, LerpTagType());
        }

        static ThisType lerpImpl(const ThisType &left, const ThisType &right, float delta, NonLerpableTag)
        {
            return left;
        }

        static ThisType lerpImpl(const ThisType &left, const ThisType &right, float delta, LerpableTag)
        {
            if(left.hasValue() && right.hasValue())
            {
                StateLerp<T> lerper;
                return lerper(left.mValue, right.mValue, delta);

            }else
            {
                return left;
            }
        }

        bool hasValue() const { return mHasValue; }

        bool operator==(const ThisType &rhs) const
        {
            return mValue == rhs.mValue;
        }

        T get() const
        {
            return mValue;
        }

        ThisType &operator=(const T &v)
        {
            mValue = v;
            mHasValue = true;
            return *this;
        }


    private:

        bool mHasValue;
        T mValue;
    };


    template <typename _Bundle>
    class StateCountOp
    {
    public:

        StateCountOp(const _Bundle &bundle)
        : mBundle(bundle)
        , mCount(0)
        {
        }

        size_t getCount() const { return mCount; }

        template <typename _State>
        StateCountOp &operator()(_State _Bundle::* state)
        {
            if((mBundle.*state).hasValue())
            {
                mCount++;
            }

            return *this;
        }


    private:

        const _Bundle &mBundle;
        size_t mCount;

    };


    template <typename _Bundle>
    class StateMergeOp
    {
    public:

        StateMergeOp(const _Bundle &left, const _Bundle &right, _Bundle &result)
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

        const _Bundle &mLeftBundle;
        const _Bundle &mRightBundle;
        _Bundle &mResultBundle;
    };


    template <typename _Bundle>
    class StateLerpOp
    {
    public:

        StateLerpOp(const _Bundle &left, const _Bundle &right, _Bundle &result, float delta)
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

        const _Bundle &mLeftBundle;
        const _Bundle &mRightBundle;
        _Bundle &mResultBundle;
        float mDelta;
    };


    template <typename _Bundle>
    class StateDeltaEncOp
    {
    public:

        StateDeltaEncOp(const _Bundle &reference, const _Bundle &toEncode, _Bundle &result)
        : mReference(reference)
        , mToEncode(toEncode)
        , mResultBundle(result)
        {
        }

        template <typename _State>
        StateDeltaEncOp &operator()(_State _Bundle::* state)
        {
            if((mReference.*state).hasValue() && mToEncode.*state == mReference.*state)
            {
                mResultBundle.*state = _State();

            }else
            {
                mResultBundle.*state = mToEncode.*state;
            }

            return *this;
        }


    private:

        const _Bundle &mReference;
        const _Bundle &mToEncode;
        _Bundle &mResultBundle;
    };


    struct StateBundleBase
    {
        virtual ~StateBundleBase() = default;
        virtual size_t countStatesWithValue() = 0;
        virtual void merge(const StateBundleBase &lhs, const StateBundleBase &rhs) = 0;
        virtual void lerp(const StateBundleBase &lhs, const StateBundleBase &rhs, float delta) = 0;
        virtual void deltaEncode(const StateBundleBase &reference, const StateBundleBase &toEncode) = 0;
    };


    template <typename _Bundle>
    class StateBundle : public StateBundleBase
    {
    public:

        virtual size_t countStatesWithValue() override final
        {
            auto &thisSub = static_cast<_Bundle&>(*this);
            StateCountOp<_Bundle> op(thisSub);
            _Bundle::stateOp(op);
            return op.getCount();
        }

        void merge(const _Bundle &lhs, const _Bundle &rhs)
        {
            auto &result = static_cast<_Bundle&>(*this);
            StateMergeOp<_Bundle> op(lhs, rhs, result);
            _Bundle::stateOp(op);
        }

        virtual void merge(const StateBundleBase &left, const StateBundleBase &right) override final
        {
            auto &lhs = *(od::downcast<const _Bundle>(&left));
            auto &rhs = *(od::downcast<const _Bundle>(&right));
            merge(lhs, rhs);
        }

        void lerp(const _Bundle &lhs, const _Bundle &rhs, float delta)
        {
            auto &result = static_cast<_Bundle&>(*this);
            StateLerpOp<_Bundle> op(lhs, rhs, result, delta);
            _Bundle::stateOp(op);
        }

        virtual void lerp(const StateBundleBase &left, const StateBundleBase &right, float delta) override final
        {
            auto &lhs = *(od::downcast<const _Bundle>(&left));
            auto &rhs = *(od::downcast<const _Bundle>(&right));
            lerp(lhs, rhs, delta);
        }

        void deltaEncode(const _Bundle &reference, const _Bundle &toEncode)
        {
            auto &result = static_cast<_Bundle&>(*this);
            StateDeltaEncOp<_Bundle> op(reference, toEncode, result);
            _Bundle::stateOp(op);
        }

        virtual void deltaEncode(const StateBundleBase &prev, const StateBundleBase &current) override final
        {
            auto &prevSub = *(od::downcast<const _Bundle>(&prev));
            auto &currentSub = *(od::downcast<const _Bundle>(&current));
            deltaEncode(prevSub, currentSub);
        }


    private:

        friend _Bundle;

        // private constructor, but _Bundle is a friend, so it is the only thing that can construct us
        StateBundle() = default;

    };

}

#endif
