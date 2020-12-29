
/*
 * @file Event.h
 * @author zal
 */

#ifndef INCLUDE_ODCORE_STATE_EVENT_H_
#define INCLUDE_ODCORE_STATE_EVENT_H_

#include <variant>
#include <memory>

#include <odCore/IdTypes.h>
#include <odCore/Exception.h>
#include <odCore/Message.h>

#include <odCore/db/Asset.h>
#include <odCore/db/Animation.h>

#include <odCore/input/Action.h>

namespace od
{
    class LevelObject;
}

namespace odDb
{
    class Animation;
}

namespace odState
{

    /**
     * @brief Empty base for event classes so a visitor can implement a catch-all functor.
     *
     * These are not supposed to be virtual. Dispatch should be static based on a variant over all event types.
     * Make sure to keep these as lightweigt as possible.
     */
    struct Event
    {
    };


    struct ActionEvent final : public Event
    {
        ActionEvent(odInput::ActionCode code, bool down);

        odInput::ActionCode actionCode;
        bool keyDown;
    };
    

    struct ObjectAnimEvent final : public Event
    {
        ObjectAnimEvent(od::LevelObjectId obj, const odDb::GlobalAssetRef &anim, int32_t channel, float speed);

        od::LevelObjectId objectId;
        odDb::GlobalAssetRef animRef;
        std::shared_ptr<odDb::Animation> anim;
        int32_t channelIndex;
        float speedModifier;
    };


    struct ObjectMessageEvent final : public Event
    {
        ObjectMessageEvent(od::LevelObjectId sender, od::LevelObjectId receiver, const od::Message &msg);

        od::LevelObjectId senderObjectId;
        od::LevelObjectId receiverObjectId;
        od::Message message;
    };


    /*struct SoundEvent final : public Event
    {
    };*/


    using EventVariant = std::variant<ActionEvent, ObjectAnimEvent, ObjectMessageEvent>;

}

#endif
