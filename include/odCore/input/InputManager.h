/*
 * InputManager.h
 *
 *  Created on: 28 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_
#define INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_

#include <unordered_map>
#include <mutex>
#include <vector>
#include <functional>
#include <glm/vec2.hpp>

#include <odCore/Downcast.h>

#include <odCore/input/Action.h>
#include <odCore/input/Keys.h>

namespace odInput
{
    class InputListener;
    class RawActionListener;

    class InputManager
    {
    public:

        InputManager();
        ~InputManager();

        /**
         * @brief Tells the engine that the mouse moved.
         *
         * Coordinates are in GUI space (top-left is 0/0, bottom-right is 1/1).
         *
         * Call this from your input listener (if applicable). This method is
         * fully synchronized, so it is safe to call this from an asynchronous
         * input listener or a different thread.
         */
        void injectMouseMovement(float absX, float absY);

        /**
         * @brief Tells the engine that a keyboard or mouse key was pressed or released.
         *
         * It is okay to call this repeatedly if the key is being held down (though
         * this is not necessary to get repeated action).
         *
         * Call this from your input listener (if applicable). This method is
         * fully synchronized, so it is safe to call this from an asynchronous
         * input listener or a different thread.
         *
         * @param pressed  true if button was pressed
         */
        void injectKey(Key key, bool pressed);

        /**
         * @brief Injects an action state without need for a keypress.
         *
         * This is useful for servers, where an InputManager exists for each
         * client and basically serves as a dispatch mechanism for the callbacks.
         */
        void injectAction(ActionCode actionCode, ActionState state);

        template <typename _ActionEnum>
        ActionHandle<_ActionEnum> &getAction(_ActionEnum action)
        {
            ActionCode actionCode = static_cast<ActionCode>(action);

            auto &actionPtr = mActions[actionCode];
            if(actionPtr == nullptr)
            {
                auto newAction = std::make_unique<ActionHandle<_ActionEnum>>(*this, action);
                auto &result = *newAction;
                actionPtr = std::move(newAction);
                return result;
            }

            return *od::downcast<ActionHandle<_ActionEnum>>(actionPtr.get());
        }

        template <typename _ActionEnum>
        void bindAction(_ActionEnum action, Key key)
        {
            _bind(getAction(action), key);
        }

        template <typename _ActionEnum>
        void unbindAction(_ActionEnum action, Key key)
        {
            _unbind(getAction(action), key);
        }

        /**
         * @brief Creates a raw action listener that reports every action that's being triggered.
         *
         * This does not have much use for game mechanics. It's main use is for the engine subsystems to
         * be notfied when an action needs to be dispatched somewhere.
         */
        std::shared_ptr<RawActionListener> createRawActionListener();

        /**
         * @brief Creates an input listener which reports raw input events, but within the update loop.
         *
         * This is useful for the GUI, which needs no Action-based abstraction because it's events are
         * never sent over the network.
         */
        std::shared_ptr<InputListener> createInputListener();

        void update(float relTime);


    private:

        void _bind(ActionHandleBase &action, Key key);
        void _unbind(ActionHandleBase &action, Key key);

        void _processMouseMove(glm::vec2 pos);
        void _processKey(Key key, bool pressed);

        void _triggerCallbackOnAction(ActionHandleBase &action, ActionState state);

        template <typename T>
        void _forEachInputListener(const T &functor)
        {
            for(auto &l : mInputListeners)
            {
                if(!l.expired())
                {
                    auto listener = l.lock();
                    if(listener != nullptr)
                    {
                        functor(*listener);
                    }
                }
            }
        }

        struct KeyBinding
        {
            static const size_t MAX_BINDINGS = 4;

            KeyBinding()
            : down(false)
            , actions({nullptr})
            {
            }

            bool down;

            // raw pointers are okay here because we never deallocate the actions as long as the IM lives
            std::array<ActionHandleBase*, MAX_BINDINGS> actions;
        };

        std::vector<std::pair<Key, bool>> mKeyQueue; // second = button pressed
        glm::vec2 mMouseMoveTarget;
        bool mMouseMoved;
        std::mutex mInputEventQueueMutex;

        std::unordered_map<ActionCode, std::unique_ptr<ActionHandleBase>> mActions;
        std::unordered_map<Key, KeyBinding> mKeyBindings;

        std::vector<std::weak_ptr<InputListener>> mInputListeners;
        std::vector<std::weak_ptr<RawActionListener>> mRawActionListeners;
    };

}

#endif /* INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_ */
