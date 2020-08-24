
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

#include <unordered_map>

#include <odCore/IdTypes.h>
#include <odCore/Exception.h>
#include <odCore/Message.h>
#include <odCore/ObjectTransform.h>

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
        OBJECT_TRANSFORMED,
        OBJECT_ANIM_STARTSTOP,
        OBJECT_ANIM_FRAME,
        OBJECT_MESSAGE_RECEIVED,
        OBJECT_VISIBILITY_CHANGED
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


    /**
     * @brief An event that directly relates to a level state change.
     *
     * This includes object movement, animation frames, visibility change etc.
     *
     * When rolling back the level to a certain point in time, the state manager
     * only has to consider these type of events.
     *
     * Every event of this type must be invertible.
     */
    struct StateEvent : public Event
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


    struct ObjectTransformEvent final : public StateEvent
    {
        ObjectTransformEvent(od::LevelObject &obj, const od::ObjectTransform &tf)
        : object(obj)
        , transform(tf)
        {
        }

        ObjectTransformEvent invert() const
        {
            return ObjectTransformEvent(object, transform.invert());
        }

        od::LevelObject &object;
        od::ObjectTransform transform;
    };


    struct ObjectAnimStartStopEvent final : public Event
    {
        ObjectAnimStartStopEvent(od::LevelObject &obj, odDb::AssetRef anim, bool start)
        : object(obj)
        , animRef(anim)
        , started(start)
        {
        }

        od::LevelObject &object;
        odDb::AssetRef animRef;
        bool started;
    };


    struct ObjectAnimFrameEvent final : public StateEvent
    {
        ObjectAnimFrameEvent(od::LevelObject &obj, float frame)
        : object(obj)
        , frameTime(frame)
        {
        }

        od::LevelObject &object;
        float frameTime;
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


    struct ObjectVisibilityChangedEvent final : public StateEvent
    {
        ObjectVisibilityChangedEvent(od::LevelObject &obj, bool v)
        : object(obj)
        , visible(v)
        {
        }

        ObjectVisibilityChangedEvent invert() const
        {
            return ObjectVisibilityChangedEvent(object, !visible);
        }

        od::LevelObject &object;
        bool visible;
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

        EventVariant(const ObjectTransformEvent &e)
        : mTransformEvent(e)
        , mVariantType(EventType::OBJECT_TRANSFORMED)
        {
        }

        EventVariant(const ObjectMessageReceivedEvent &e)
        : mMessageReceivedEvent(e)
        , mVariantType(EventType::OBJECT_MESSAGE_RECEIVED)
        {
        }

        EventVariant(const ObjectVisibilityChangedEvent &e)
        : mVisibilityEvent(e)
        , mVariantType(EventType::OBJECT_VISIBILITY_CHANGED)
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

            case EventType::OBJECT_TRANSFORMED:
                visitor(mTransformEvent);
                break;

            case EventType::OBJECT_MESSAGE_RECEIVED:
                visitor(mMessageReceivedEvent);
                break;

            case EventType::OBJECT_VISIBILITY_CHANGED:
                visitor(mVisibilityEvent);
                break;

            default:
                throw od::Exception("Invalid event variant");
            }
        }


    private:

        union
        {
            ActionEvent mActionEvent;
            ObjectTransformEvent mTransformEvent;
            ObjectMessageReceivedEvent mMessageReceivedEvent;
            ObjectVisibilityChangedEvent mVisibilityEvent;
        };

        EventType mVariantType;

    };


    struct ObjectStateTransition
    {
        bool transformed;
        bool visibilityChanged;
        bool animationFrame;

        od::ObjectTransform transform;
        bool visibility;
        float animFrameTime;
    };


}

#endif
