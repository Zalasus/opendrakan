
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
            static_assert(HasGlmMix<T>::value, "glm::mix is not defined for T. You need to provide a specialization of StateLerp<T>");

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


        template <typename T>
        struct StateDiffCompare
        {
            using ScalarType = float;

            bool diffExceedsThreshold(const T &a, const T &b, ScalarType threshold)
            {
                return a != b;
            }
        };

        template <>
        struct StateDiffCompare<glm::quat>
        {
            using ScalarType = float;

            bool diffExceedsThreshold(const glm::quat &a, const glm::quat &b, ScalarType threshold)
            {
                glm::vec3 testVec(0, 0, 1);
                auto testVec1 = a * testVec;
                auto testVec2 = b * testVec1;
                float angleDiff = std::acos(glm::dot(testVec1, testVec2));

                return angleDiff > threshold;
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

            template <typename _StateType, StateFlags::Type _Flags>
            StateCountOp &operator()(State<_StateType, _Flags> _Bundle::* state)
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

            template <typename _StateType, StateFlags::Type _Flags>
            StateMergeOp &operator()(State<_StateType, _Flags> _Bundle::* state)
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

            template <typename _StateType, StateFlags::Type _Flags>
            StateLerpOp &operator()(State<_StateType, _Flags> _Bundle::* state)
            {
                if constexpr(_Flags & StateFlags::LERPED)
                {
                    if((mLeftBundle.*state).hasValue() && (mRightBundle.*state).hasValue() && !(mRightBundle.*state).isJump())
                    {
                        StateLerp<_StateType> lerper;
                        mResultBundle.*state = lerper((mLeftBundle.*state).get(), (mRightBundle.*state).get(), mDelta);

                    }else
                    {
                        mResultBundle.*state = (mLeftBundle.*state);
                    }

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

            template <typename _StateType, StateFlags::Type _Flags>
            StateDeltaEncOp &operator()(State<_StateType, _Flags> _Bundle::* state)
            {
                if((mReference.*state).hasValue() && mToEncode.*state == mReference.*state)
                {
                    mResultBundle.*state = State<_StateType, _Flags>();

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

        template <typename _StateType, StateFlags::Type _Flags>
        bool shouldBeIncludedInSerialization(StateSerializationPurpose purpose, const State<_StateType, _Flags> &state)
        {
            switch(purpose)
            {
            case StateSerializationPurpose::NETWORK:
                return state.isNetworked();

            case StateSerializationPurpose::SAVEGAME:
                return state.isSaved();
            }

            OD_UNREACHABLE();
        }


        template <typename _Bundle>
        class StateSerializeOp
        {
        public:

            StateSerializeOp(const _Bundle &bundle, od::DataWriter &writer, StateSerializationPurpose purpose)
            : mBundle(bundle)
            , mWriter(writer)
            , mPurpose(purpose)
            , mStateIndexInMask(0)
            , mValueMask(0)
            , mJumpMask(0)
            , mBitFieldSize(0)
            , mBitField(0)
            {
                mMaskOffset = mWriter.tell();
                mWriter << mValueMask << mJumpMask;
            }

            ~StateSerializeOp()
            {
                _flushBitField();
                _updateMask();
            }

            template <typename _StateType, StateFlags::Type _Flags>
            StateSerializeOp &operator()(State<_StateType, _Flags> _Bundle::* state)
            {
                auto &stateRef = mBundle.*state;

                if(shouldBeIncludedInSerialization(mPurpose, stateRef) && stateRef.hasValue())
                {
                    mValueMask |= (1 << mStateIndexInMask);

                    if(stateRef.isJump())
                    {
                        mJumpMask |= (1 << mStateIndexInMask);
                    }

                    _write(stateRef.get());
                }

                ++mStateIndexInMask;

                if(mStateIndexInMask >= MAX_MASK_STATES)
                {
                    mValueMask |= EXTENDED_BIT;
                    _updateMask();

                    mStateIndexInMask = 0;
                    mValueMask = 0;
                    mJumpMask = 0;

                    mMaskOffset = mWriter.tell();
                    mWriter << mValueMask << mJumpMask;
                }

                return *this;
            }


        private:

            void _updateMask()
            {
                auto p = mWriter.tell();
                mWriter.seek(mMaskOffset);
                mWriter << mValueMask << mJumpMask;
                mWriter.seek(p);
            }

            void _flushBitField()
            {
                if(mBitFieldSize > 0)
                {
                    mWriter << mBitField;
                    mBitFieldSize = 0;
                    mBitField = 0;
                }
            }

            template <typename T>
            void _write(const T &v)
            {
                _flushBitField();
                mWriter << v;
            }

            void _write(bool v)
            {
                if(v)
                {
                    mBitField |= (1 << mBitFieldSize);
                }

                mBitFieldSize += 1;
                if(mBitFieldSize >= 8)
                {
                    _flushBitField();
                }
            }


            const _Bundle &mBundle;
            od::DataWriter &mWriter;
            StateSerializationPurpose mPurpose;

            size_t mStateIndexInMask;
            MaskType mValueMask;
            MaskType mJumpMask;
            std::streamoff mMaskOffset;

            int mBitFieldSize;
            uint8_t mBitField;
        };


        template <typename _Bundle>
        class StateDeserializeOp
        {
        public:

            StateDeserializeOp(_Bundle &bundle, od::DataReader &reader, StateSerializationPurpose purpose)
            : mBundle(bundle)
            , mReader(reader)
            , mPurpose(purpose)
            , mStateIndexInMask(0)
            , mValueMask(0)
            , mJumpMask(0)
            , mBitFieldSize(0)
            , mBitField(0)
            {
                mReader >> mValueMask >> mJumpMask;
            }

            template <typename _StateType, StateFlags::Type _Flags>
            StateDeserializeOp &operator()(State<_StateType, _Flags> _Bundle::* state)
            {
                auto &stateRef = mBundle.*state;

                if(mValueMask & (1 << mStateIndexInMask))
                {
                    _StateType value;
                    _read(value);

                    if(shouldBeIncludedInSerialization(mPurpose, stateRef))
                    {
                        stateRef = value;
                        stateRef.setJump(mJumpMask & (1 << mStateIndexInMask));

                    }else
                    {
                        Logger::warn() << "State with index " << mStateIndexInMask << " from bundle " << typeid(_Bundle).name()
                          << " was included in serialization when it shouldn't have been. Ignoring state";
                    }
                }

                ++mStateIndexInMask;

                if(mStateIndexInMask >= MAX_MASK_STATES)
                {
                    mStateIndexInMask = 0;

                    if(mValueMask & EXTENDED_BIT)
                    {
                        mReader >> mValueMask >> mJumpMask;

                    }else
                    {
                        mValueMask = 0;
                        mJumpMask = 0;
                    }
                }

                return *this;
            }


        private:

            template <typename T>
            void _read(T &v)
            {
                if(mBitFieldSize > 0)
                {
                    mBitFieldSize = 0;
                }

                mReader >> v;
            }

            void _read(bool &b)
            {
                if(mBitFieldSize == 0)
                {
                    mReader >> mBitField;
                }

                b = mBitField & (1 << mBitFieldSize);

                mBitFieldSize += 1;
                if(mBitFieldSize >= 8)
                {
                    mBitFieldSize = 0;
                }
            }

            _Bundle &mBundle;
            od::DataReader &mReader;
            StateSerializationPurpose mPurpose;

            size_t mStateIndexInMask;
            MaskType mValueMask;
            MaskType mJumpMask;

            int mBitFieldSize;
            uint8_t mBitField;
        };

    }

}

#endif
