
#include <odCore/state/Event.h>

#include <limits>

#include <odCore/DataStream.h>

namespace odState
{

    ActionEvent::ActionEvent(odInput::ActionCode code, bool down)
    : actionCode(code)
    , keyDown(down)
    {
    }

    ActionEvent::ActionEvent(od::DataReader &dr)
    {
        uint8_t kd;

        dr >> actionCode
           >> kd;

        keyDown = kd;
    }

    void ActionEvent::serialize(od::DataWriter &dw) const
    {
        dw << actionCode
           << static_cast<uint8_t>(keyDown ? 1 : 0);
    }


    ObjectAnimEvent::ObjectAnimEvent(od::LevelObjectId objId, const odDb::GlobalAssetRef &anim, const odAnim::AnimModes &m)
    : objectId(objId)
    , animRef(anim)
    , modes(m)
    {
    }

    ObjectAnimEvent::ObjectAnimEvent(od::DataReader &dr)
    {
        uint8_t flags;

        dr >> objectId
           >> animRef
           >> modes.channel
           >> modes.speed
           >> modes.startTime
           >> modes.transitionTime
           >> flags;

        // FIXME: validate
        modes.boneModes[0] = static_cast<odAnim::BoneMode>((flags & 0x03) >> 0);
        modes.boneModes[1] = static_cast<odAnim::BoneMode>((flags & 0x0c) >> 2);
        modes.boneModes[2] = static_cast<odAnim::BoneMode>((flags & 0x30) >> 4);
        modes.playbackType = static_cast<odAnim::PlaybackType>((flags & 0xc0) >> 6);
    }

    void ObjectAnimEvent::serialize(od::DataWriter &dw) const
    {
        // FIXME: this only works as long as there are no more than four bone modes/playback types
        uint8_t flags =
              (static_cast<uint8_t>(modes.boneModes[0]) << 0)
            | (static_cast<uint8_t>(modes.boneModes[1]) << 2)
            | (static_cast<uint8_t>(modes.boneModes[2]) << 4)
            | (static_cast<uint8_t>(modes.playbackType) << 6);

        dw << objectId
           << animRef
           << modes.channel
           << modes.speed
           << modes.startTime
           << modes.transitionTime
           << flags;
    }


    ObjectMessageEvent::ObjectMessageEvent(od::LevelObjectId sender, od::LevelObjectId receiver, const od::Message &msg)
    : senderObjectId(sender)
    , receiverObjectId(receiver)
    , message(msg)
    {
    }

    ObjectMessageEvent::ObjectMessageEvent(od::DataReader &dr)
    {
        std::underlying_type_t<od::Message> msg;

        dr >> senderObjectId
           >> receiverObjectId
           >> msg;

        message = static_cast<od::Message>(msg);
    }

    void ObjectMessageEvent::serialize(od::DataWriter &dw) const
    {
        dw << senderObjectId
           << receiverObjectId
           << static_cast<std::underlying_type_t<od::Message>>(message);
    }


    // we serialize the index as a narrower type than size_t to save some bytes
    using SerializedIndex = uint8_t;
    static_assert(std::variant_size_v<EventVariant> <= std::numeric_limits<SerializedIndex>::max(), "Too many event variants. Need a bigger SerializedIndex type");

    void EventVariantSerializer::serialize(const EventVariant &ev, od::DataWriter &dw)
    {
        dw << static_cast<SerializedIndex>(ev.index());
        std::visit([&dw](auto &e){ e.serialize(dw); }, ev);
    }

    template <typename T>
    struct DeserializeConstructorWrapper
    {
        T make(od::DataReader &dr) const
        {
            return T{dr};
        }
    };

    template <typename>
    struct DummyTag
    {
    };

    template <typename..._Types>
    static EventVariant deserializeImpl(size_t index, od::DataReader &dr, DummyTag<std::variant<_Types...>>)
    {
        using WrapperVariant = std::variant<DeserializeConstructorWrapper<_Types>...>;

        static constexpr WrapperVariant wrappers[] = { DeserializeConstructorWrapper<_Types>()... };

        auto visitor = [&dr](auto &w){ return EventVariant{ w.make(dr)}; };
        return std::visit(visitor, wrappers[index]);
    }

    EventVariant EventVariantSerializer::deserialize(od::DataReader &dr)
    {
        SerializedIndex index;
        dr >> index;

        return deserializeImpl(static_cast<size_t>(index), dr, DummyTag<EventVariant>());
    }

}
