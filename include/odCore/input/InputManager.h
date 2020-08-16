/*
 * InputManager.h
 *
 *  Created on: 28 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_
#define INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_

#include <map>
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

  /*
   the server needs an inputmanager which does the same dispatch-to-subscribers as this one,
   but lacks the keybinding facilities. the server creates one of these for every connected client and the class in question can subscribe to the one
   of the relevant player. 
  */

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
         */
        void mouseMoved(float absX, float absY);
        void mouseButtonDown(int buttonCode);
        void mouseButtonUp(int buttonCode);

        void keyDown(Key key);
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


    private:

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

        odGui::Gui *mGui;
        std::map<Key, Binding> mBindings;

        std::map<ActionCode, std::weak_ptr<IAction>> mActions;

        std::vector<std::weak_ptr<CursorListener>> mCursorListeners;
    };

}

#endif /* INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_ */
