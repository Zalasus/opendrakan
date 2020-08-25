
#include <odCore/input/InputListener.h>

namespace odInput
{

    void InputListener::keyEvent(Key key, bool pressed)
    {
        if(pressed && mKeyDownCallback != nullptr)
        {
            mKeyDownCallback(key);

        }else if(mKeyUpCallback != nullptr)
        {
            mKeyUpCallback(key);
        }
    }

    void InputListener::mouseMoveEvent(const glm::vec2 &pos)
    {
        if(mMouseMovedCallback != nullptr)
        {
            mMouseMovedCallback(pos);
        }
    }

}
