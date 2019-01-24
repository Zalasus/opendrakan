/*
 * InputManager.cpp
 *
 *  Created on: 29 Dec 2018
 *      Author: zal
 */

#include <odCore/input/InputManager.h>

#include <odCore/gui/Gui.h>

namespace odInput
{

    InputManager::InputManager()
    : mGui(nullptr)
    {
    }

    void InputManager::mouseMoved(float absX, float absY)
    {
        if(mGui != nullptr)
        {
            mGui->setCursorPosition(glm::vec2(absX, absY));
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

    void InputManager::bindActionToKey(IAction *iaction, Key key)
    {
        Binding &binding = mBindings[key];
        for(auto &a : binding.actions)
        {
            if(a == 0)
            {
                a = iaction->getActionAsInt();
                return;
            }
        }

        throw od::Exception("Exceeded maximum of actions per key");
    }

    void InputManager::unbindActionFromKey(IAction *iaction, Key key)
    {
        auto it = mBindings.find(key);
        if(it == mBindings.end())
        {
            return;
        }

        for(auto &a : it->second.actions)
        {
            if(a == iaction->getActionAsInt())
            {
                a = 0;
            }
        }
    }

    void InputManager::_triggerCallbackOnAction(int action, InputEvent event)
    {
        auto it = mActions.find(action);
        if(it == mActions.end())
        {
            return;
        }

        if(it->second != nullptr)
        {
            if(event.type == InputEvent::Type::Repeat && !it->second->isRepeatable())
            {
                return;
            }

            it->second->triggerCallback(event);
        }
    }

}
