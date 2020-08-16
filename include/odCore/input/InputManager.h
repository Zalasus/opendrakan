/*
 * InputManager.h
 *
 *  Created on: 28 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_
#define INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_

#include <map>
#include <mutex>
#include <vector>
#include <functional>
#include <glm/vec2.hpp>

#include <odCore/Downcast.h>
#include <odCore/Exception.h>

#include <odCore/input/Action.h>
#include <odCore/input/Keys.h>

namespace odGui
{
    class Gui;
}

namespace odInput
{
    class CursorListener;

    class InputManager
    {
    public:

        InputManager();
        ~InputManager();

        /// @brief Sets GUI to which to report cursor updates. May be nullptr.
        inline void setGui(odGui::Gui *gui) { mGui = gui; }

        /**
         * @brief Tells the engine that the mouse moved.
         *
         * Coordinates are in GUI space (top-left is 0/0, bottom-right is 1/1).
         *
         * Call this from your input listener (if applicable). This method is
         * fully synchronized, so it is safe to call this from an asynchronous
         * input listener or a different thread.
         */
        void mouseMoved(float absX, float absY);

        /**
         * @brief Tells the engine that a mouse button was pressed.
         *
         * TODO: buttonCode is yet to be defined as an enum. stay tuned!
         *
         * Call this from your input listener (if applicable). This method is
         * fully synchronized, so it is safe to call this from an asynchronous
         * input listener or a different thread.
         */
        void mouseButtonDown(int buttonCode);

        /**
         * @brief Tells the engine that a mouse button was released.
         *
         * TODO: buttonCode is yet to be defined as an enum. stay tuned!
         *
         * Call this from your input listener (if applicable). This method is
         * fully synchronized, so it is safe to call this from an asynchronous
         * input listener or a different thread.
         */
        void mouseButtonUp(int buttonCode);

        /**
         * @brief Tells the engine that a keyboard key was pressed.
         *
         * It is okay to call this repeatedly if the key is being held down.
         *
         * Call this from your input listener (if applicable). This method is
         * fully synchronized, so it is safe to call this from an asynchronous
         * input listener or a different thread.
         */
        void keyDown(Key key);

        /**
         * @brief Tells the engine that a keyboard key was released.
         *
         * Call this from your input listener (if applicable). This method is
         * fully synchronized, so it is safe to call this from an asynchronous
         * input listener or a different thread.
         */
        void keyUp(Key key);

        void injectAction(ActionCode actionCode, ActionState state)
        {
            if(actionCode == 0)
            {
                throw od::InvalidArgumentException("Can't inject actions whose integer representation is 0");
            }

            auto &weakRef = mActions[actionCode];
            if(!weakRef.expired())
            {
                auto actionHandle = weakRef.lock();
                if(actionHandle != nullptr)
                {
                    actionHandle->triggerCallback(state);
                }
            }
        }

        template <typename _ActionEnum>
        std::shared_ptr<ActionHandle<_ActionEnum>> getOrCreateAction(_ActionEnum action)
        {
            ActionCode actionCode = static_cast<ActionCode>(action);
            if(actionCode == 0)
            {
                throw od::InvalidArgumentException("Can't create actions whose integer representation is 0");
            }

            std::shared_ptr<ActionHandle<_ActionEnum>> actionHandle;
            auto &weakRef = mActions[actionCode];
            if(weakRef.expired())
            {
                actionHandle = std::make_shared<ActionHandle<_ActionEnum>>(*this, action);
                weakRef = actionHandle;

            }else
            {
                std::shared_ptr<IAction> ref(weakRef);
                actionHandle = od::downcast<ActionHandle<_ActionEnum>>(ref);
            }

            return actionHandle;
        }

        void bindActionToKey(std::shared_ptr<IAction> action, Key key);
        void unbindActionFromKey(std::shared_ptr<IAction> action, Key key);

        std::shared_ptr<CursorListener> createCursorListener();

        void update(float relTime);


    private:

        void _processMouseMove(glm::vec2 pos);
        void _processMouseDown(int buttonCode);
        void _processMouseUp(int buttonCode);
        void _processKeyDown(Key key);
        void _processKeyUp(Key key);

        void _triggerCallbackOnAction(ActionCode action, ActionState state);

        struct Binding
        {
            static const size_t MAX_BINDINGS = 4;

            Binding()
            : down(false)
            , actions({0})
            {
            }

            bool down;

            // NOTE: storing direct references to the actions here is faster, but may overflow the max
            //  amount of observers per action very quickly
            //  FIXME: however, using an int here will prevent bindings from getting removed automatically
            //  TODO: now that we have a weakptr that uses a control block instead of observers we might want to fix this
            std::array<ActionCode, MAX_BINDINGS> actions;
        };

        // TODO: combine into a single queue of a variant
        std::vector<std::pair<Key, bool>> mKeyQueue; // second = button up
        std::vector<std::pair<int, bool>> mMouseButtonQueue;
        glm::vec2 mMouseMoveTarget;
        bool mMouseMoved;
        std::mutex mInputEventQueueMutex;

        odGui::Gui *mGui;
        std::map<Key, Binding> mBindings;

        std::map<ActionCode, std::weak_ptr<IAction>> mActions;

        std::vector<std::weak_ptr<CursorListener>> mCursorListeners;
    };

}

#endif /* INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_ */
