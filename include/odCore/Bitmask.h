
#ifndef INCLUDE_ODCORE_BITMASK_H_
#define INCLUDE_ODCORE_BITMASK_H_

#include <type_traits>

namespace od
{

    /**
     * Type-safe bitmask wrapper around scoped enums.
     *
     * This is a direct adaption of the type-safe bitmasks from here: https://gpfault.net/posts/typesafe-bitmasks.txt.html
     * The code was modified to match OpenDrakan's code style.
     */
    template <typename _OptionType, typename = typename std::enable_if<std::is_enum<_OptionType>::value>::type>
    class Bitmask
    {
    public:

        // Default ctor creates a bitmask with no options selected.
        constexpr Bitmask()
        : mMask(0)
        {
        }

        constexpr Bitmask(_OptionType o)
        : mMask(_maskValue(o))
        {
        }

        // Set the bit corresponding to the given option.
        constexpr Bitmask operator|(_OptionType t)
        {
            return Bitmask(mMask | _maskValue(t));
        }

        // Get the value of the bit corresponding to the given option.
        constexpr bool operator&(_OptionType t)
        {
            return mMask & _maskValue(t);
        }


    private:

        using UnderlyingType = typename std::underlying_type<_OptionType>::type;

        // This method helps us avoid having to explicitly set enum values to powers of two.
        static constexpr UnderlyingType _maskValue(_OptionType o)
        {
            return 1 << static_cast<UnderlyingType>(o);
        }

        // Private ctor to be used internally.
        explicit constexpr Bitmask(UnderlyingType o)
        : mMask(o)
        {
        }


        UnderlyingType mMask;

    };

    // Creates a bitmask from two options, convenient for stuff like:
    // FunctionExpectingBitmask(Options::Opt1 | Options::Opt2 | Options::Opt3)
    template <typename _OptionType, typename = typename std::enable_if<std::is_enum<_OptionType>::value>::type>
    constexpr Bitmask<_OptionType> operator|(_OptionType lhs, _OptionType rhs)
    {
        return Bitmask<_OptionType>{lhs} | rhs;
    }

}

#endif
