
#ifndef INCLUDE_ODCORE_STATE_STATE_H_
#define INCLUDE_ODCORE_STATE_STATE_H_

#include <odCore/Downcast.h>

namespace odState
{

    /**
     * @brief Enum to specify for what purpose a state bundle is serialized.
     *
     * Some states are not sent over the network, while others might not be
     * stored in a savegame (this is determined by state flags). In order for
     * the StateBundle serializer to know when to write a state or not, you have
     * to specify what the serialization is for using this enum.
     *
     * While strictly only used by state bundles, we have to put it here so it
     * is available both to the public and to the StateBundleDetail header.
     */
    enum class StateSerializationPurpose
    {
        NETWORK,
        SAVEGAME
    };


    struct StateFlags
    {
        using Type = int;

        static constexpr Type NOT_SAVED     = (1 << 0);
        static constexpr Type NOT_NETWORKED = (1 << 1);
        static constexpr Type LERPED        = (1 << 2);
    };


    namespace detail
    {
        /**
         * Base class for State<T> holding it's value.
         *
         * This allows us to store bool states inside the flags instead of a
         * dedicated member, saving memory due to empty-base-optimization.
         */
        template <typename T>
        class StateValueHolder
        {
        public:

            inline const T &_get(uint16_t) const
            {
                return mValue;
            }

            inline T &_get(uint16_t)
            {
                return mValue;
            }

            inline void _set(const T &v, uint16_t&)
            {
                mValue = v;
            }


        private:

            T mValue;
        };

        template <>
        class StateValueHolder<bool>
        {
        public:

            inline bool _get(uint16_t flags) const
            {
                return flags & BOOLEAN_FLAG;
            }

            inline void _set(const bool &v, uint16_t &flags)
            {
                if(v)
                {
                    flags |= BOOLEAN_FLAG;

                }else
                {
                    flags &= ~BOOLEAN_FLAG;
                }
            }


        private:

            static constexpr uint16_t BOOLEAN_FLAG = (1 << 15);

        };
    }


    /**
     * A template for a simple state type. This should handle most basic types
     * of states (ints, floats, glm vectors etc.).
     *
     * This works like an Optional, so this can either contain a value or not.
     */
    template <typename T, StateFlags::Type _Flags = 0>
    struct State : private detail::StateValueHolder<T>
    {
    public:

        using ThisType = State<T, _Flags>;

        State()
        : mInternalFlags(0)
        , mRevisionCounter(0)
        {
        }

        explicit State(T v)
        : mInternalFlags(HAS_VALUE)
        , mRevisionCounter(0)
        {
            this->_set(v, mInternalFlags);
        }

        State(const ThisType &v) = default;

        bool hasValue() const { return mInternalFlags & HAS_VALUE; }
        bool isJump() const { return mInternalFlags & IS_JUMP; } ///< Ths only makes sense if the state has a value
        bool isPredictionDisabled() const { return mInternalFlags & DO_NOT_PREDICT; }
        bool isSendingDisabled() const { return mInternalFlags & DO_NOT_SEND; }
        bool isSavingDisabled() const { return mInternalFlags & DO_NOT_SAVE; }
        uint16_t getRevision() const { return mRevisionCounter; }

        void setJump(bool b)
        {
            _setFlag(b, IS_JUMP);
        }

        void setPredictionDisabled(bool b)
        {
            _setFlag(b, DO_NOT_PREDICT);
        }

        void setSendingDisabled(bool b)
        {
            _setFlag(b, DO_NOT_SEND);
        }

        void setSavingDisabled(bool b)
        {
            _setFlag(b, DO_NOT_SAVE);
        }

        bool operator==(const ThisType &rhs) const
        {
            if(this->hasValue() && rhs.hasValue())
            {
                return this->_get(mInternalFlags) == rhs._get(rhs.mInternalFlags);

            }else
            {
                return false;
            }
        }

        T get() const
        {
            return this->_get(mInternalFlags);
        }

        ThisType &operator=(const T &v)
        {
            this->_set(v, mInternalFlags);
            mInternalFlags |= HAS_VALUE;
            mRevisionCounter += 1;
            return *this;
        }

        ThisType &operator=(const ThisType &s) = default;


    private:

        inline void _setFlag(bool b, uint16_t mask)
        {
            if(b)
            {
                mInternalFlags |= mask;

            }else
            {
                mInternalFlags &= ~mask;
            }
        }

        static constexpr uint16_t HAS_VALUE      = (1 << 0);
        static constexpr uint16_t IS_JUMP        = (1 << 1);
        static constexpr uint16_t DO_NOT_PREDICT = (1 << 2);
        static constexpr uint16_t DO_NOT_SEND    = (1 << 3);
        static constexpr uint16_t DO_NOT_SAVE    = (1 << 4);
        // highest bit is reserved for booleans!

        uint16_t mInternalFlags;
        uint16_t mRevisionCounter;
    };

}

#endif
