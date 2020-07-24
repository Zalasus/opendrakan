
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

#include <odCore/Exception.h>
#include <odCore/Message.h>

#include <odCore/input/Keys.h>

#include <odCore/state/ObjectTransform.h>

namespace od
{
    class LevelObject;
}

namespace odState
{

    enum class EventType
    {
        KEY,
        OBJECT_TRANSFORMED,
        OBJECT_ANIM_STARTED,
        OBJECT_ANIM_STOPPED,
        OBJECT_MESSAGE_RECEIVED,
        OBJECT_VISIBILITY_CHANGED
    };


    /**
     * @brief Empty base for event classes so a visitor can implement a catch-all functor.
     */
    struct Event
    {
    };


    struct KeyEvent : public Event
    {
        KeyEvent(odInput::Key k, bool down)
        : key(k)
        , keyDown(down)
        {
        }

        odInput::Key key;
        bool keyDown;
    };


    struct ObjectTransformEvent : public Event
    {

        ObjectTransformEvent(od::LevelObject &obj, const ObjectTransform &tf)
        : object(obj)
        , transform(tf)
        {
        }

        od::LevelObject &object;
        ObjectTransform transform;
    };


    struct ObjectMessageReceivedEvent : public Event
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


    struct ObjectVisibilityChangedEvent : public Event
    {
        ObjectVisibilityChangedEvent(od::LevelObject &obj, bool v)
        : object(obj)
        , visible(v)
        {
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

        EventVariant(const KeyEvent &e)
        : mKeyEvent(e)
        , mVariantType(EventType::KEY)
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
            case EventType::KEY:
                visitor(mKeyEvent);
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
            KeyEvent mKeyEvent;
            ObjectTransformEvent mTransformEvent;
            ObjectMessageReceivedEvent mMessageReceivedEvent;
            ObjectVisibilityChangedEvent mVisibilityEvent;
        };

        EventType mVariantType;

    };

}

#endif
