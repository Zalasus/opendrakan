
#ifndef INCLUDE_ODCORE_INPUT_INPUTEVENT_H_
#define INCLUDE_ODCORE_INPUT_INPUTEVENT_H_

namespace odInput
{
    enum class InputEventType
    {
        MOUSE_MOVE,
        MOUSE_BUTTON,
        KEY
    };

    struct MouseMoveEvent
    {
        glm::vec2 posInGuiSpace;
    };

    struct MouseButtonEvent
    {
        MouseButton button;
        bool up;
    };

    struct KeyEvent
    {
        Key key;
        bool up;
    };

    /**
     * @brief Variant over any input event.
     */
    class InputEvent
    {
    public:

        InputEvent(const MouseMoveEvent &event)
        : mType(InputEventType::MOUSE_MOVE)
        , mMouseMove(event)
        {
        }

        InputEvent(const MouseButtonEvent &event)
        : mType(InputEventType::MOUSE_BUTTON)
        , mMouseButton(event)
        {
        }

        InputEvent(const KeyEvent &event)
        : mType(InputEventType::KEY)
        , mKey(event)
        {
        }

        inline InputEventType getType() const { return mType; }

        template <typename T>
        void visit(const T &f)
        {
            switch(mType)
            {
            case MOUSE_MOVE:
                f(mMouseMove);
                break;

            case MOUSE_BUTTON:
                f(mMouseButton);
                break;

            case KEY:
                f(mKey);
                break;
            }
        }


    private:

        InputEventType mType;

        union
        {
            MouseMoveEvent mMouseMove;
            MouseButtonEvent mMouseButton;
            KeyEvent mKey;
        };

    };

}

#endif
