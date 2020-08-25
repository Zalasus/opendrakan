
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

    enum class EventType
    {
        ACTION,
        OBJECT_ANIM,
        OBJECT_MESSAGE_RECEIVED
    };


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
    };


    struct ActionEvent final : public Event
    {
        ActionEvent(odInput::ActionCode code, bool down)
        : actionCode(code)
        , keyDown(down)
        {
        }

        odInput::ActionCode actionCode;
        bool keyDown;
    };


    struct ObjectAnimEvent final : public Event
    {
        ObjectAnimEvent(od::LevelObject &obj, odDb::AssetRef anim, bool start)
        : object(obj)
        , animRef(anim)
        , started(start)
        {
        }

        od::LevelObject &object;
        odDb::AssetRef animRef;
        bool started;
    };


    struct ObjectMessageReceivedEvent final : public Event
    {
        ObjectMessageReceivedEvent(od::LevelObject &sender, od::LevelObject &receiver, const od::Message &msg)
        : senderObject(sender)
        , receiverObject(receiver)
        , message(msg)
        {
        }

        od::LevelObject &senderObject;
        od::LevelObject &receiverObject;
        od::Message message;
    };


    /**
     * @brief Variant that can contain any event type.
     *
     * Man, I wish I could use C++17...
     */
    class EventVariant
    {
    public:

        EventVariant(const ActionEvent &e)
        : mActionEvent(e)
        , mVariantType(EventType::ACTION)
        {
        }

        EventVariant(const ObjectMessageReceivedEvent &e)
        : mMessageReceivedEvent(e)
        , mVariantType(EventType::OBJECT_MESSAGE_RECEIVED)
        {
        }

        EventVariant(const ObjectAnimEvent &e)
        : mAnimEvent(e)
        , mVariantType(EventType::OBJECT_ANIM)
        {
        }

        inline EventType getEventType() const { return mVariantType; }

        template <typename T>
        void visit(T &visitor) const
        {
            switch(mVariantType)
            {
            case EventType::ACTION:
                visitor(mActionEvent);
                break;

            case EventType::OBJECT_MESSAGE_RECEIVED:
                visitor(mMessageReceivedEvent);
                break;

            case EventType::OBJECT_ANIM:
                visitor(mAnimEvent);
                break;

            default:
                throw od::Exception("Invalid event variant");
            }
        }


    private:

        union
        {
            ActionEvent mActionEvent;
            ObjectMessageReceivedEvent mMessageReceivedEvent;
            ObjectAnimEvent mAnimEvent;
        };

        EventType mVariantType;

    };

}

#endif
