/*
 * InputManager.cpp
 *
 *  Created on: 29 Dec 2018
 *      Author: zal
 */

#include <odCore/input/InputManager.h>

#include <odCore/gui/Gui.h>

#include <odCore/input/CursorListener.h>

namespace odInput
{

    InputManager::InputManager()
    : mGui(nullptr)
    {
    }

    InputManager::~InputManager()
    {
    }

    void InputManager::mouseMoved(float absX, float absY)
    {
        glm::vec2 pos(absX, absY);

        if(mGui != nullptr)
        {
            mGui->setCursorPosition(pos);
        }

        for(auto &cl : mCursorListeners)
        {
            if(!cl.expired())
            {
                auto listener = cl.lock();
                if(listener != nullptr)
                {
                    listener->triggerCallback(pos);
                }
            }
        }
    }

    void InputManager::mouseButtonDown(int buttonCode)
    {
        if(mGui != nullptr)
        {
            mGui->mouseDown();
        }
    }

    void InputManager::mouseButtonUp(int buttonCode)
    {

    }

    void InputManager::keyDown(Key key)
    {
        auto it = mBindings.find(key);
        if(it == mBindings.end())
        {
            return; // no bindings for this key
        }

        InputEvent event;
        event.key = key;
        event.type = it->second.down ? InputEvent::Type::Repeat : InputEvent::Type::Down;

        it->second.down = true;

        for(auto &a : it->second.actions)
        {
            if(a != 0)
            {
                _triggerCallbackOnAction(a, event);
            }
        }
    }

    void InputManager::keyUp(Key key)
    {
        auto it = mBindings.find(key);
        if(it == mBindings.end())
        {
            return; // no bindings for this key
        }

        InputEvent event;
        event.key = key;
        event.type = InputEvent::Type::Up;

        it->second.down = false;

        for(auto &a : it->second.actions)
        {
            if(a != 0)
            {
                _triggerCallbackOnAction(a, event);
            }
        }
    }

    void InputManager::bindActionToKey(std::shared_ptr<IAction> iaction, Key key)
    {
        Binding &binding = mBindings[key];
        for(auto &a : binding.actions)
        {
            if(a == 0)
            {
                a = iaction->getActionCode();
                return;
            }
        }

        throw od::Exception("Exceeded maximum of actions per key");
    }

    void InputManager::unbindActionFromKey(std::shared_ptr<IAction> iaction, Key key)
    {
        auto it = mBindings.find(key);
        if(it == mBindings.end())
        {
            return;
        }

        for(auto &a : it->second.actions)
        {
            if(a == iaction->getActionCode())
            {
                a = 0;
            }
        }
    }

    std::shared_ptr<CursorListener> InputManager::createCursorListener()
    {
        auto listener = std::make_shared<CursorListener>();

        mCursorListeners.emplace_back(listener);

        return listener;
    }

    void InputManager::_triggerCallbackOnAction(ActionCode action, InputEvent event)
    {
        auto it = mActions.find(action);
        if(it == mActions.end())
        {
            return;
        }

        if(!it->second.expired())
        {
            std::shared_ptr<IAction> action(it->second);

            if(event.type == InputEvent::Type::Repeat && !action->isRepeatable())
            {
                return;

            }else if(event.type == InputEvent::Type::Up && action->ignoresUpEvents())
            {
                return;
            }

            action->triggerCallback(event);
        }
    }

}
