
#include <odCore/state/Event.h>

namespace odState
{

    Event::Event(double rt)
    : realtime(rt)
    {
    }


    ActionEvent::ActionEvent(double realtime, odInput::ActionCode code, bool down)
    : Event(realtime)
    , actionCode(code)
    , keyDown(down)
    {
    }


    ObjectAnimEvent::ObjectAnimEvent(double realtime, od::LevelObject &obj, const odDb::AssetRef &anim, int32_t channel, float speed)
    : Event(realtime)
    , object(obj)
    , animRef(anim)
    , channelIndex(channel)
    , speedModifier(speed)
    {
    }

    ObjectAnimEvent::~ObjectAnimEvent()
    {
    }


    ObjectMessageEvent::ObjectMessageEvent(double realtime, od::LevelObject &sender, od::LevelObject &receiver, const od::Message &msg)
    : Event(realtime)
    , senderObject(sender)
    , receiverObject(receiver)
    , message(msg)
    {
    }


    double realtimeForEventVariant(const EventVariant &o)
    {
        auto v = [](const Event &e){ return e.realtime; };
        return std::visit(v, o);
    }

}
