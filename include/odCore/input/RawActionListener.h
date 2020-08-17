
#ifndef INCLUDE_ODCORE_INPUT_RAWACTIONLISTENER_H_
#define INCLUDE_ODCORE_INPUT_RAWACTIONLISTENER_H_

#include <functional>

#include <odCore/input/Action.h>

namespace odInput
{

    struct RawActionListener
    {
        using Callback = std::function<void(ActionCode, ActionState)>;

        Callback callback;
    };

}

#endif
