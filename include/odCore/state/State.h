
#ifndef INCLUDE_ODCORE_STATE_STATE_H_
#define INCLUDE_ODCORE_STATE_STATE_H_

#include <utility>
#include <type_traits>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/Logger.h>
#include <odCore/Downcast.h>
#include <odCore/DataStream.h>

namespace odState
{

    struct StateFlags
    {
        using Type = int;

        static const Type NOT_SAVED     = (1 << 0);
        static const Type NOT_NETWORKED = (1 << 1);
        static const Type LERPED        = (1 << 2);
    };


    namespace detail
    {

        template <typename, typename = void>
        struct HasGlmMix : std::false_type
        {
        };

        template <typename T>
        struct HasGlmMix<T, std::void_t<decltype(glm::mix(std::declval<T>(), std::declval<T>(), std::declval<float>()))>> : std::true_type
        {
        };

        template <typename T>
        struct StateLerp
        {
            struct LerpableTag {};
            struct NonLerpableTag {};
            using LerpTagType = typename std::conditional<HasGlmMix<T>::value, LerpableTag, NonLerpableTag>::type;

            T operator()(const T &left, const T &right, float delta)
            {
                return doTheThing(left, right, delta, LerpTagType());
            }

            T doTheThing(const T &left, const T &right, float delta, LerpableTag)
            {
                return glm::mix(left, right, delta);
            }

            T doTheThing(const T &left, const T &right, float delta, NonLerpableTag)
            {
                return left;
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

    } // namespace detail

    /**
     * A template for a simple state type. This should handle most basic types
     * of states (ints, floats, glm vectors etc.).
     *
     * This works like an Optional, so this can either contain a value or not.
     */
    template <typename T>
    struct State
    {
    public:

        using ThisType = State<T>;

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

        template <typename _StateType>
        StateCountOp &operator()(State<_StateType> _Bundle::* state, StateFlags::Type flags)
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

        template <typename _StateType>
        StateMergeOp &operator()(State<_StateType> _Bundle::* state, StateFlags::Type flags)
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

        template <typename _StateType>
        StateLerpOp &operator()(State<_StateType> _Bundle::* state, StateFlags::Type flags)
        {
            if((flags & StateFlags::LERPED) && (mLeftBundle.*state).hasValue() && (mRightBundle.*state).hasValue())
            {
                detail::StateLerp<_StateType> lerper;
                mResultBundle.*state = lerper((mLeftBundle.*state).get(), (mRightBundle.*state).get(), mDelta);

            }else
            {
                mResultBundle.*state = (mLeftBundle.*state);
            }

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

        template <typename _StateType>
        StateDeltaEncOp &operator()(State<_StateType> _Bundle::* state, StateFlags::Type flags)
        {
            if((mReference.*state).hasValue() && mToEncode.*state == mReference.*state)
            {
                mResultBundle.*state = State<_StateType>();

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


    enum class StateSerializationPurpose
    {
        NETWORK,
        SAVEGAME
    };

    namespace detail
    {
        using MaskType = uint8_t;

        constexpr size_t MAX_MASK_STATES{ sizeof(MaskType)*8 - 1 };
        constexpr size_t EXTENDED_BIT{ 1 << MAX_MASK_STATES };

        bool shouldBeIncludedInSerialization(StateSerializationPurpose purpose, StateFlags::Type flags);

        template <typename _StateType>
        void wrappedStateValueWrite(od::DataWriter &writer, const _StateType &value)
        {
            writer << value;
        }

        template <>
        void wrappedStateValueWrite<bool>(od::DataWriter &writer, const bool &value);

        template <typename _StateType>
        void wrappedStateValueRead(od::DataReader &reader, _StateType &value)
        {
            reader >> value;
        }

        template <>
        void wrappedStateValueRead<bool>(od::DataReader &reader, bool &value);

    } // namespace detail

    template <typename _Bundle>
    class StateSerializeOp
    {
    public:

        StateSerializeOp(const _Bundle &bundle, od::DataWriter &writer, StateSerializationPurpose purpose)
        : mBundle(bundle)
        , mWriter(writer)
        , mPurpose(purpose)
        , mStateCount(0)
        , mMask(0)
        {
            mMaskOffset = mWriter.tell();
            mWriter << mMask;
        }

        ~StateSerializeOp()
        {
             _updateMask();
        }

        template <typename _StateType>
        StateSerializeOp &operator()(State<_StateType> _Bundle::* state, StateFlags::Type flags)
        {
            if(detail::shouldBeIncludedInSerialization(mPurpose, flags) && (mBundle.*state).hasValue())
            {
                mMask |= (1 << mStateCount);
                detail::wrappedStateValueWrite(mWriter, (mBundle.*state).get());
            }

            ++mStateCount;

            if(mStateCount >= detail::MAX_MASK_STATES)
            {
                mMask |= detail::EXTENDED_BIT;
                _updateMask();

                mStateCount = 0;
                mMask = 0;

                mMaskOffset = mWriter.tell();
                mWriter << mMask;
            }

            return *this;
        }

        // TODO: later, add specialization for boolean states that implements squeezing them into bitfields


    private:

        void _updateMask()
        {
            auto p = mWriter.tell();
            mWriter.seek(mMaskOffset);
            mWriter << mMask;
            mWriter.seek(p);
        }

        const _Bundle &mBundle;
        od::DataWriter &mWriter;
        StateSerializationPurpose mPurpose;

        size_t mStateCount;
        detail::MaskType mMask;
        std::streamoff mMaskOffset;
    };


    template <typename _Bundle>
    class StateDeserializeOp
    {
    public:

        StateDeserializeOp(_Bundle &bundle, od::DataReader &reader, StateSerializationPurpose purpose)
        : mBundle(bundle)
        , mReader(reader)
        , mPurpose(purpose)
        , mStateCount(0)
        , mMask(0)
        {
            mReader >> mMask;
        }

        template <typename _StateType>
        StateDeserializeOp &operator()(State<_StateType> _Bundle::* state, StateFlags::Type flags)
        {
            if(mMask & (1 << mStateCount))
            {
                _StateType value;
                detail::wrappedStateValueRead(mReader, value);

                if(detail::shouldBeIncludedInSerialization(mPurpose, flags))
                {
                    (mBundle.*state) = value;

                }else
                {
                    Logger::warn() << "State with index " << mStateCount << " from bundle " << typeid(_Bundle).name()
                      << " was included in serialization when it shouldn't have. Ignoring state";
                }
            }

            ++mStateCount;

            if(mStateCount >= detail::MAX_MASK_STATES)
            {
                mStateCount = 0;

                if(mMask & detail::EXTENDED_BIT)
                {
                    mReader >> mMask;

                }else
                {
                    mMask = 0;
                }
            }

            return *this;
        }

        // TODO: later, add specialization for boolean states that implements squeezing them into bitfields


    private:

        _Bundle &mBundle;
        od::DataReader &mReader;
        StateSerializationPurpose mPurpose;

        size_t mStateCount;
        detail::MaskType mMask;
    };


    struct StateBundleBase
    {
        virtual ~StateBundleBase() = default;
        virtual size_t countStatesWithValue() = 0;
        virtual void merge(const StateBundleBase &lhs, const StateBundleBase &rhs) = 0;
        virtual void lerp(const StateBundleBase &lhs, const StateBundleBase &rhs, float delta) = 0;
        virtual void deltaEncode(const StateBundleBase &reference, const StateBundleBase &toEncode) = 0;
        virtual void serialize(od::DataWriter &writer, StateSerializationPurpose purpose) = 0;
        virtual void deserialize(od::DataReader &reader, StateSerializationPurpose purpose) = 0;
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

        virtual void serialize(od::DataWriter &writer, StateSerializationPurpose purpose) override final
        {
            auto &bundle = static_cast<_Bundle&>(*this);
            StateSerializeOp<_Bundle> op(bundle, writer, purpose);
            _Bundle::stateOp(op);
        }

        virtual void deserialize(od::DataReader &reader, StateSerializationPurpose purpose) override final
        {
            auto &bundle = static_cast<_Bundle&>(*this);
            StateDeserializeOp<_Bundle> op(bundle, reader, purpose);
            _Bundle::stateOp(op);
        }


    protected:

        using BundleType = _Bundle; // this alias is used by the OD_STATE macros


    private:

        friend _Bundle;

        // private constructor, but _Bundle is a friend, so it is the only thing that can construct us
        StateBundle() = default;

    };

    /**
     * @brief Helper macro for generating the stateOp function in StateBundles.
     *
     * This begins the list of states. Do not terminate with a semicolon!
     */
    #define OD_BEGIN_STATE_LIST() template <typename T> static void stateOp(T &op) { op

    /**
     * @brief Helper macro for generating the stateOp function in StateBundles.
     *
     * Adds a state to the state list. Call for every state in your bundle,
     * between a call to OD_BEGIN_STATE_LIST and OD_END_STATE_LIST. Do not put
     * a semicolon after this!
     */
    #define OD_STATE(name, flags) (&BundleType::name, (flags))

    /**
     * @brief Helper macro for generating the stateOp function in StateBundles.
     *
     * Finalizes the state list. Do not put a semicolon after this!
     */
    #define OD_END_STATE_LIST() ; }

}

#endif
