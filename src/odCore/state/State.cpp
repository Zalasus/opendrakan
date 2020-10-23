
#include <odCore/state/State.h>

namespace odState
{

    namespace detail
    {
        template <>
        void wrappedStateValueWrite<bool>(od::DataWriter &writer, const bool &value)
        {
            writer << static_cast<uint8_t>(value ? 1 : 0);
        }
    }

}
