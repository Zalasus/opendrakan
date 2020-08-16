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
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mMouseMoveTarget = {absX, absY};
        mMouseMoved = true;
    }

    void InputManager::mouseButtonDown(int buttonCode)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mMouseButtonQueue.push_back(std::make_pair(buttonCode, false));
    }

    void InputManager::mouseButtonUp(int buttonCode)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mMouseButtonQueue.push_back(std::make_pair(buttonCode, true));
    }

    void InputManager::keyDown(Key key)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mKeyQueue.push_back(std::make_pair(key, false));
    }

    void InputManager::keyUp(Key key)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mKeyQueue.push_back(std::make_pair(key, true));
    }

    void InputManager::bindActionToKey(std::shared_ptr<IAction> action, Key key)
    {
        Binding &binding = mBindings[key];
        for(auto &a : binding.actions)
        {
            if(a.expired())
            {
                a = action;
                return;
            }
        }

        throw od::Exception("Exceeded maximum number of actions per key");
    }

    void InputManager::unbindActionFromKey(std::shared_ptr<IAction> action, Key key)
    {
        auto it = mBindings.find(key);
        if(it == mBindings.end())
        {
            return;
        }

        for(auto &a : it->second.actions)
        {
            auto boundAction = a.lock();
            if(boundAction == action)
            {
                a.reset();
            }
        }
    }

    std::shared_ptr<CursorListener> InputManager::createCursorListener()
    {
        auto listener = std::make_shared<CursorListener>();

        mCursorListeners.emplace_back(listener);

        return listener;
    }

    void InputManager::update(float relTime)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);

        if(mMouseMoved)
        {
            _processMouseMove(mMouseMoveTarget);
            mMouseMoved = false;
        }

        for(auto &key : mKeyQueue)
        {
            if(key.second)
            {
                _processKeyUp(key.first);

            }else
            {
                _processKeyDown(key.first);
            }
        }
        mKeyQueue.clear();

        for(auto &button : mMouseButtonQueue)
        {
            if(button.second)
            {
                _processMouseUp(button.first);

            }else
            {
                _processMouseDown(button.first);
            }
        }
        mMouseButtonQueue.clear();
    }

    void InputManager::_processMouseMove(glm::vec2 pos)
    {
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

    void InputManager::_processMouseDown(int buttonCode)
    {
        if(mGui != nullptr)
        {
            mGui->mouseDown();
        }
    }

    void InputManager::_processMouseUp(int buttonCode)
    {
    }

    void InputManager::_processKeyDown(Key key)
    {
        auto it = mBindings.find(key);
        if(it == mBindings.end())
        {
            return; // no bindings for this key
        }

        ActionState state = it->second.down ? ActionState::REPEAT : ActionState::BEGIN;

        it->second.down = true;

        for(auto &boundAction : it->second.actions)
        {
            if(!boundAction.expired())
            {
                auto action = boundAction.lock();
                if(action != nullptr)
                {
                    _triggerCallbackOnAction(*action, state);
                }
            }
        }
    }

    void InputManager::_processKeyUp(Key key)
    {
        auto it = mBindings.find(key);
        if(it == mBindings.end())
        {
            return; // no bindings for this key
        }

        it->second.down = false;

        for(auto &boundAction : it->second.actions)
        {
            if(!boundAction.expired())
            {
                auto action = boundAction.lock();
                if(action != nullptr)
                {
                    _triggerCallbackOnAction(*action, ActionState::END);
                }
            }
        }
    }

    void InputManager::_triggerCallbackOnAction(IAction &action, ActionState state)
    {
        if(state == ActionState::REPEAT && !action.isRepeatable())
        {
            return;

        }else if(state == ActionState::END && action.ignoresUpEvents())
        {
            return;
        }

        action.triggerCallback(state);
    }

}
