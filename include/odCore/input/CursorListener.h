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

#include <odCore/RefCounted.h>

namespace odInput
{

    class CursorListener : public od::RefCounted
    {
    public:

        enum class Mode
        {
            GameMode,
            MenuMode,
            Both
        };

        typedef std::function<void(const glm::vec2&)> CallbackType;

        inline void setCallback(const CallbackType &callback) { mCallback = callback; }

        void triggerCallback(const glm::vec2 &ndc);


    private:

        CallbackType mCallback;

    };

}


#endif /* INCLUDE_ODCORE_INPUT_CURSORLISTENER_H_ */
