
/*
 * @file Event.h
 * @author zal
 *
 * Types used to represent various events that  will affect the state of the
 * level. These are used by the StateManager to keep track of every change in
 * the past or the future. They are not intended to be passed over the network,
 * however, as they may directly reference the affected objects for performance.
 * For passing them between server and client, a translation step is needed
 * (likely going to be done in the Connector implementation).
 */

#ifndef INCLUDE_ODCORE_STATE_EVENT_H_
#define INCLUDE_ODCORE_STATE_EVENT_H_

#include <variant>

#include <odCore/IdTypes.h>
#include <odCore/Exception.h>
#include <odCore/Message.h>

#include <odCore/db/Asset.h>

#include <odCore/input/Action.h>

namespace od
{
    class LevelObject;
}

namespace odState
{

    /**
     * @brief Empty base for event classes so a visitor can implement a catch-all functor.
     *
     * These are not supposed to be virtual. Dispatch should be static based on a variant over all event types.
     * Make sure to keep these as lightweigt as possible.
     *
     * These are only used locally, so host-dependent info like pointers is okay. The net submodule later translates
     * them into host-independet packets.
     */
    struct Event
    {
        Event(double rt)
        : realtime(rt)
        {
        }

        double realtime;
    };


    struct ActionEvent final : public Event
    {
        ActionEvent(double realtime, odInput::ActionCode code, bool down)
        : Event(realtime)
        , actionCode(code)
        , keyDown(down)
        {
        }

        odInput::ActionCode actionCode;
        bool keyDown;
    };


    struct ObjectAnimEvent final : public Event
    {
        ObjectAnimEvent(double realtime, od::LevelObject &obj, const odDb::AssetRef &anim, int32_t channel, float speed)
        : Event(realtime)
        , object(obj)
        , animRef(anim)
        , channelIndex(channel)
        , speedModifier(speed)
        {
        }

        od::LevelObject &object;
        odDb::AssetRef animRef;
        std::shared_ptr<odDb::Animation> animation;
        int32_t channelIndex;
        float speedModifier;
    };


    struct ObjectMessageEvent final : public Event
    {
        ObjectMessageEvent(double realtime, od::LevelObject &sender, od::LevelObject &receiver, const od::Message &msg)
        : Event(realtime)
        , senderObject(sender)
        , receiverObject(receiver)
        , message(msg)
        {
        }

        od::LevelObject &senderObject;
        od::LevelObject &receiverObject;
        od::Message message;
    };


    /*struct SoundEvent final : public Event
    {
    };*/


    using EventVariant = std::variant<ActionEvent, ObjectAnimEvent, ObjectMessageEvent>;

    double realtimeForEventVariant(const EventVariant &o);
}

#endif
