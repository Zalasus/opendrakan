
/**
 * @file
 * @brief Detail templates for StateBundle
 *
 * This file contains all the gritty details that are used to auto-generate
 * lerp-, merge-, serialize-methods etc. from a compact StateBundle
 * implementation. The outside-facing interface you should worry about is
 * defined in StateBundle.h
 */

#ifndef INCLUDE_ODCORE_STATE_STATEBUNDLEDETAIL_H_
#define INCLUDE_ODCORE_STATE_STATEBUNDLEDETAIL_H_

#include <utility>
#include <type_traits>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/Logger.h>
#include <odCore/DataStream.h>

#include <odCore/state/State.h>

namespace odState
{

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
                    StateLerp<_StateType> lerper;
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


        // ========== serialization & deserialization ===========

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
                if(shouldBeIncludedInSerialization(mPurpose, flags) && (mBundle.*state).hasValue())
                {
                    mMask |= (1 << mStateCount);
                    wrappedStateValueWrite(mWriter, (mBundle.*state).get());
                }

                ++mStateCount;

                if(mStateCount >= MAX_MASK_STATES)
                {
                    mMask |= EXTENDED_BIT;
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
            MaskType mMask;
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
                    wrappedStateValueRead(mReader, value);

                    if(shouldBeIncludedInSerialization(mPurpose, flags))
                    {
                        (mBundle.*state) = value;

                    }else
                    {
                        Logger::warn() << "State with index " << mStateCount << " from bundle " << typeid(_Bundle).name()
                          << " was included in serialization when it shouldn't have. Ignoring state";
                    }
                }

                ++mStateCount;

                if(mStateCount >= MAX_MASK_STATES)
                {
                    mStateCount = 0;

                    if(mMask & EXTENDED_BIT)
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
            MaskType mMask;
        };

    }

}

#endif
