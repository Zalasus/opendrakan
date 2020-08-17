
#include <odCore/input/InputListener.h>

namespace odInput
{

    void InputListener::keyEvent(Key key, bool up)
    {
        if(up && mKeyUpCallback != nullptr)
        {
            mKeyUpCallback(key);

        }else if(mKeyDownCallback != nullptr)
        {
            mKeyDownCallback(key);
        }
    }

    void InputListener::mouseButtonEvent(int button, bool up)
    {
        if(up && mMouseButtonUpCallback != nullptr)
        {
            mMouseButtonUpCallback(button);

        }else if(mMouseButtonDownCallback != nullptr)
        {
            mMouseButtonDownCallback(button);
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
