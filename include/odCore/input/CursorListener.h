/*
 * CursorListener.h
 *
 *  Created on: Jan 27, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_INPUT_CURSORLISTENER_H_
#define INCLUDE_ODCORE_INPUT_CURSORLISTENER_H_

#include <functional>

#include <glm/vec2.hpp>

namespace odInput
{

    class CursorListener
    {
    public:

        enum class Mode
        {
            GameMode,
            MenuMode,
            Both
        };

        /**
         * @brief Callback for when the cursor moved.
         * The first parameter is the cursor position in GUI space (top-left is 0/0, bottom-right is 1/1)
         */
        typedef std::function<void(const glm::vec2&)> CallbackType;

        inline void setCallback(const CallbackType &callback) { mCallback = callback; }

        /**
         * @brief Triggers the callback.
         * @param cursorPos Cursor position in GUI space
         */
        void triggerCallback(const glm::vec2 &cursorPos);


    private:

        CallbackType mCallback;

    };

}


#endif /* INCLUDE_ODCORE_INPUT_CURSORLISTENER_H_ */
