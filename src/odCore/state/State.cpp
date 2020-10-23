
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

        template <>
        void wrappedStateValueRead<bool>(od::DataReader &reader, bool &value)
        {
            uint8_t v;
            reader >> v;
            value = (v != 0);
        }

        bool shouldBeIncludedInSerialization(StateSerializationPurpose purpose, StateFlags::Type flags)
        {
            switch(purpose)
            {
            case StateSerializationPurpose::NETWORK:
                return !(flags & StateFlags::NOT_NETWORKED);
                break;

            case StateSerializationPurpose::SAVEGAME:
                return !(flags & StateFlags::NOT_SAVED);
                break;
            }

            OD_UNREACHABLE();
        }

    }

}
