
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


    namespace StateFlags
    {
        using Type = int;

        constexpr Type NOT_SAVED     = (1 << 0);
        constexpr Type NOT_NETWORKED = (1 << 1);
        constexpr Type LERPED        = (1 << 2);
    }


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
        : mInternalFlags(0)
        , mValue()
        {
        }

        explicit State(T v)
        : mInternalFlags(HAS_VALUE)
        , mValue(v)
        {
        }

        bool hasValue() const { return mInternalFlags & HAS_VALUE; }
        bool isJump() const { return mInternalFlags & IS_JUMP; } ///< Ths only makes sense if the state has a value

        void setJump(bool b)
        {
            if(b)
            {
                mInternalFlags |= IS_JUMP;

            }else
            {
                mInternalFlags &= ~IS_JUMP;
            }
        }

        bool operator==(const ThisType &rhs) const
        {
            if(this->hasValue() && rhs.hasValue())
            {
                return mValue == rhs.mValue;

            }else
            {
                return false;
            }
        }

        T get() const
        {
            return mValue;
        }

        ThisType &operator=(const T &v)
        {
            mValue = v;
            mInternalFlags |= HAS_VALUE;
            return *this;
        }

        ThisType &operator=(const ThisType &s)
        {
            mValue = s.mValue;
            mInternalFlags = s.mInternalFlags;
            return *this;
        }


    private:

        static constexpr uint8_t HAS_VALUE = (1 << 0);
        static constexpr uint8_t IS_JUMP   = (1 << 1);

        uint8_t mInternalFlags;
        T mValue;
    };

}

#endif
