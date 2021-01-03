
#ifndef INCLUDE_ODCORE_INPUT_INPUTLISTENER_H_
#define INCLUDE_ODCORE_INPUT_INPUTLISTENER_H_

#include <functional>

#include <glm/vec2.hpp>

#include <odCore/input/Keys.h>

namespace odInput
{

    class InputListener
    {
    public:

        /**
         * @brief Callback for when the cursor moved.
         * The first parameter is the cursor position in GUI space (top-left is 0/0, bottom-right is 1/1)
         */
        using MouseMovedCallback = std::function<void(const glm::vec2&)>;

        using KeyCallback = std::function<void(Key key)>;

        inline void setMouseMoveCallback(const MouseMovedCallback &callback) { mMouseMovedCallback = callback; }
        inline void setKeyDownCallback(const KeyCallback &callback) { mKeyDownCallback = callback; }
        inline void setKeyUpCallback(const KeyCallback &callback) { mKeyUpCallback = callback; }

        void keyEvent(Key key, bool pressed);
        void mouseMoveEvent(const glm::vec2 &pos);


    private:

        MouseMovedCallback mMouseMovedCallback;
        KeyCallback mKeyDownCallback;
        KeyCallback mKeyUpCallback;

    };

}

#endif
