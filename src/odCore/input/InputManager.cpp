/*
 * InputManager.cpp
 *
 *  Created on: 29 Dec 2018
 *      Author: zal
 */

#include <odCore/input/InputManager.h>

#include <algorithm>

#include <odCore/Exception.h>

#include <odCore/gui/Gui.h>

#include <odCore/input/InputListener.h>
#include <odCore/input/RawActionListener.h>

namespace odInput
{

    InputManager::InputManager()
    : mMouseMoved(false)
    {
    }

    InputManager::~InputManager()
    {
    }

    void InputManager::injectMouseMovement(float absX, float absY)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mMouseMoveTarget = {absX, absY};
        mMouseMoved = true;
    }

    void InputManager::injectKey(Key key, bool pressed)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mKeyQueue.push_back(std::make_pair(key, pressed));
    }

    void InputManager::injectAction(ActionCode actionCode, ActionState state)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mInjectedActionQueue.push_back(std::make_pair(actionCode, state));
    }

    void InputManager::injectAnalogAction(ActionCode actionCode, const glm::vec2 &axisData)
    {
        std::lock_guard<std::mutex> lock(mInputEventQueueMutex);
        mInjectedAnalogActionQueue.push_back(std::make_pair(actionCode, axisData));
    }

    std::shared_ptr<InputListener> InputManager::createInputListener()
    {
        auto listener = std::make_shared<InputListener>();
        mInputListeners.emplace_back(listener);
        return listener;
    }

    std::shared_ptr<RawActionListener> InputManager::createRawActionListener()
    {
        auto listener = std::make_shared<RawActionListener>();
        mRawActionListeners.emplace_back(listener);
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
            _processKey(key.first, key.second);
        }
        mKeyQueue.clear();

        for(auto &action : mInjectedActionQueue)
        {
            auto it = mActions.find(action.first);
            if(it != mActions.end())
            {
                _triggerAction(*(it->second), action.second);
            }
        }
        mInjectedActionQueue.clear();

        for(auto &analogAction : mInjectedAnalogActionQueue)
        {
            auto it = mAnalogActions.find(analogAction.first);
            if(it != mAnalogActions.end())
            {
                _triggerAnalogAction(*(it->second), analogAction.second);
            }
        }
        mInjectedAnalogActionQueue.clear();
    }

    void InputManager::_bind(ActionHandleBase &action, Key key)
    {
        KeyBinding &binding = mKeyBindings[key];
        for(auto &a : binding.actions)
        {
            if(a == nullptr)
            {
                a = &action;
                return;
            }
        }

        throw od::Exception("Exceeded maximum number of actions per key");
    }

    void InputManager::_unbind(ActionHandleBase &action, Key key)
    {
        auto it = mKeyBindings.find(key);
        if(it == mKeyBindings.end())
        {
            return;
        }

        bool inUse = false;
        for(auto &a : it->second.actions)
        {
            if(a == &action)
            {
                a = nullptr;

            }else if(a != nullptr)
            {
                inUse = true;
            }
        }

        if(!inUse)
        {
            mKeyBindings.erase(it);
        }
    }

    void InputManager::_bind(AnalogActionHandleBase &action, AnalogSource source)
    {
        if(source != AnalogSource::MOUSE_POSITION)
        {
            throw od::Exception("Only mouse position is implemented at the moment");
        }

        mAnalogActionsBoundToMousePos.push_back(&action);
    }

    void InputManager::_unbind(AnalogActionHandleBase &action, AnalogSource source)
    {
        auto it = std::find(mAnalogActionsBoundToMousePos.begin(), mAnalogActionsBoundToMousePos.end(), &action);
        if(it != mAnalogActionsBoundToMousePos.end())
        {
            mAnalogActionsBoundToMousePos.erase(it);
        }
    }

    void InputManager::_processMouseMove(glm::vec2 pos)
    {
        _forEachInputListener([=](auto listener){ listener.mouseMoveEvent(pos); });

        for(auto analogAction : mAnalogActionsBoundToMousePos)
        {
            glm::vec2 axis = pos;
            if(analogAction->flipYAxis()) axis.y *= -1;
            _triggerAnalogAction(*analogAction, axis);
        }
    }

    void InputManager::_processKey(Key key, bool pressed)
    {
        _forEachInputListener([=](auto listener){ listener.keyEvent(key, pressed); });

        auto it = mKeyBindings.find(key);
        if(it != mKeyBindings.end())
        {
            ActionState state;
            if(pressed)
            {
                state = it->second.down ? ActionState::REPEAT : ActionState::BEGIN;
                it->second.down = true;

            }else
            {
                state = ActionState::END;
                it->second.down = false;
            }

            for(auto &boundAction : it->second.actions)
            {
                if(boundAction != nullptr)
                {
                    if(state == ActionState::REPEAT && !boundAction->isRepeatable())
                    {
                        continue;

                    }else if(state == ActionState::END && boundAction->ignoresUpEvents())
                    {
                        continue;
                    }

                    _triggerAction(*boundAction, state);
                }
            }
        }
    }

    void InputManager::_triggerAction(ActionHandleBase &action, ActionState state)
    {
        for(auto &l : mRawActionListeners)
        {
            if(!l.expired())
            {
                auto listener = l.lock();
                if(listener != nullptr && listener->callback != nullptr)
                {
                    listener->callback(action.getActionCode(), state);
                }
            }
        }

        action.triggerCallback(state);
    }

    void InputManager::_triggerAnalogAction(AnalogActionHandleBase &analogAction, const glm::vec2 &axisData)
    {
        for(auto &l : mRawActionListeners)
        {
            if(!l.expired())
            {
                auto listener = l.lock();
                if(listener != nullptr && listener->analogCallback != nullptr)
                {
                    listener->analogCallback(analogAction.getActionCode(), axisData);
                }
            }
        }

        analogAction.triggerCallback(axisData);
    }

}
