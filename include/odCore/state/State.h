
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


    // forward StateValueHolder so we can befriend it to make private state flags accessible
    namespace detail
    {
        template <typename>
        class StateValueHolder;
    }


    /**
     * @brief Contains definitions for the State flags.
     *
     * This is a struct so we can "use" these flags in a StateBundle. We can't
     * "using namespace" in a class definition, which means that when defining a
     * custom StateBundle, we'd have to use fully qualified names for every flag
     * in every state definition, which is obviously very verbose and ugly. But
     * by making StateBundle privately inherit StateFlags, we can "use" the
     * individual flags we need in the custom StateBundles. StateFlags is an
     * empty base, thus this incurs no overhead.
     */
    struct StateFlags
    {
        using Type = uint16_t;

        static constexpr Type SAVED         = (1 << 0);
        static constexpr Type NETWORKED     = (1 << 1);
        static constexpr Type LERPED        = (1 << 2);
        static constexpr Type PREDICTED     = (1 << 3);

        static constexpr Type DEFAULT       = SAVED | NETWORKED;

    private:

        // these private flags are only used internally by State<T> and
        //  StateValueHolder. they must not be used in the static flags.

        template <typename, Type>
        friend class State;

        friend class detail::StateValueHolder<bool>;

        static constexpr Type HAS_VALUE     = (1 << 13);
        static constexpr Type JUMP          = (1 << 14);
        static constexpr Type BOOLEAN       = (1 << 15);


    protected:

        // protected. only inheriting StateBundles should be able to instantiate this
        StateFlags() = default;

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

            inline const T &_get(StateFlags::Type) const
            {
                return mValue;
            }

            inline T &_get(StateFlags::Type)
            {
                return mValue;
            }

            inline void _set(const T &v, StateFlags::Type&)
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

            inline bool _get(StateFlags::Type flags) const
            {
                return flags & StateFlags::BOOLEAN;
            }

            inline void _set(const bool &v, StateFlags::Type &flags)
            {
                if(v)
                {
                    flags |= StateFlags::BOOLEAN;

                }else
                {
                    flags &= ~StateFlags::BOOLEAN;
                }
            }
        };
    }


    /**
     * @brief A template for a tracked state, bundling data with bookkeeping.
     *
     * This handles most basic engine types of states (bools, ints, floats,
     * glm vectors and quats) out of the box.
     *
     * This works like an Optional, so this can either contain a value or not
     * (currently, T still requires DefaultConstructible, but we could lift this
     * requirement later).
     *
     * Some state flags can be statically set via the _GlobalFlags template
     * argument, which will then apply to all states instantiated from that
     * definition. However, most flags can be still modified at runtime for
     * individual states. Some flags are only valid and useful in the
     * _GlobalFlags context, like StateFlags::LERPED.
     *
     * The requirements for T are that it defines an == operator, is
     * CopyConstructible and CopyAssignable. It must be serializable and
     * deserializable via DataWriter and DataWriter, respectively. If
     * _GlobalFlags has LERPED set, StateLerp<T> must be defined for T. The
     * default StateLerp<T> assumes that glm::mix(T,T,float) exists. If that is
     * not the case, you need to provide a specialization.
     */
    template <typename T, StateFlags::Type _GlobalFlags = StateFlags::DEFAULT>
    struct State : private detail::StateValueHolder<T>
    {
    public:

        using ThisType = State<T, _GlobalFlags>;

        State()
        : mFlags(_GlobalFlags)
        , mRevisionCounter(0)
        {
        }

        explicit State(T v)
        : mFlags(_GlobalFlags | StateFlags::HAS_VALUE)
        , mRevisionCounter(0)
        {
            this->_set(v, mFlags);
        }

        State(const ThisType &v) = default;

        bool hasValue() const { return mFlags & StateFlags::HAS_VALUE; }
        bool isJump() const { return mFlags & StateFlags::JUMP; } ///< Ths only makes sense if the state has a value
        bool isPredicted() const { return mFlags & StateFlags::PREDICTED; }
        bool isNetworked() const { return mFlags & StateFlags::NETWORKED; }
        bool isSaved() const { return mFlags & StateFlags::SAVED; }
        uint16_t getRevision() const { return mRevisionCounter; }

        void setJump(bool b)
        {
            _setFlag(b, StateFlags::JUMP);
        }

        void setPredicted(bool b)
        {
            _setFlag(b, StateFlags::PREDICTED);
        }

        void setNetworked(bool b)
        {
            _setFlag(b, StateFlags::NETWORKED);
        }

        void setSaved(bool b)
        {
            _setFlag(b, StateFlags::SAVED);
        }

        bool operator==(const ThisType &rhs) const
        {
            if(this->hasValue() && rhs.hasValue())
            {
                return this->_get(mFlags) == rhs._get(rhs.mFlags);

            }else
            {
                return false;
            }
        }

        T get() const
        {
            return this->_get(mFlags);
        }

        ThisType &operator=(const T &v)
        {
            this->_set(v, mFlags);
            _setFlag(true, StateFlags::HAS_VALUE);
            mRevisionCounter += 1;
            return *this;
        }

        ThisType &operator=(const ThisType &s) = default;


    private:

        inline void _setFlag(bool b, StateFlags::Type mask)
        {
            if(b)
            {
                mFlags |= mask;

            }else
            {
                mFlags &= ~mask;
            }
        }

        StateFlags::Type mFlags; // TODO: it is desirable that this is always 16 bits (most states have 4-byte alignment). how do we convey this intent?
        uint16_t mRevisionCounter;
    };

}

#endif
