
#include <odCore/state/StateBundleDetail.h>

namespace odState
{

    namespace detail
    {
        template <>
        void wrappedStateValueWrite<bool>(od::DataWriter &writer, const bool &value)
        {
            writer << static_cast<uint8_t>(value ? 1 : 0);
        }

        template <>
        void wrappedStateValueRead<bool>(od::DataReader &reader, bool &value)
        {
            uint8_t v;
            reader >> v;
            value = (v != 0);
        }
    }

}
