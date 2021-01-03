
#ifndef INCLUDE_RFL_DRAGON_COMMON_ENUMS_H_
#define INCLUDE_RFL_DRAGON_COMMON_ENUMS_H_

#include <odCore/rfl/Field.h>

namespace dragonRfl
{

    enum class StartMode
    {
        RUN_INSTANTLY,
        RUN_WHEN_TRIGGERED
    };
    using EnumStartMode = odRfl::EnumImpl<StartMode, 0, 1>;

}

#endif
