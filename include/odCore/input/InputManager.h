/*
 * InputManager.h
 *
 *  Created on: 28 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_
#define INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_

#include <map>
#include <functional>

#include <odCore/RefCounted.h>
#include <odCore/WeakRefPtr.h>
#include <odCore/Exception.h>

#include <odCore/input/Action.h>
#include <odCore/input/Keys.h>

namespace odGui
{
    class Gui;
}

namespace odInput
{

    class InputManager
    {
    public:

        InputManager();

        /// @brief Sets GUI to which to report cursor updates. May be nullptr.
        inline void setGui(odGui::Gui *gui) { mGui = gui; }

        void mouseMoved(float absX, float absY);
        void mouseButtonDown(int buttonCode);
        void mouseButtonUp(int buttonCode);

        void keyDown(Key key);
        void keyUp(Key key);

        template <typename _ActionEnum>
        od::RefPtr<ActionHandle<_ActionEnum>> getOrCreateAction(_ActionEnum action)
        {
            int actionCode = static_cast<int>(action);

            od::RefPtr<ActionHandle<_ActionEnum>> actionHandle;
            auto &weakRef = mActions[actionCode];
            if(weakRef == nullptr)
            {
                actionHandle = od::make_refd<ActionHandle<_ActionEnum>>(*this, action);
                weakRef = actionHandle.get();

            }else
            {
                actionHandle = dynamic_cast<ActionHandle<_ActionEnum>*>(weakRef.get());
                if(actionHandle == nullptr)
                {
                    throw od::Exception("Upcast during retrieval of action failed. Are you mixing action implementations?");
                }
            }

            return actionHandle;
        }

        void bindActionToKey(IAction *action, Key key);
        void unbindActionFromKey(IAction *action, Key key);


    private:

        void _triggerCallbackOnAction(int action, InputEvent event);

        struct Binding
        {
            static const size_t MAX_BINDINGS = 4;

            Binding()
            : actions({0})
            {
            }

            // NOTE: storing direct references to the actions here is faster, but may overflow the max
            //  amount of observers per action very quickly
            //  FIXME: however, using an int here will prevent bindings from getting removed automatically
            std::array<int, MAX_BINDINGS> actions;
        };

        odGui::Gui *mGui;
        std::map<Key, Binding> mBindings;

        std::map<int, od::WeakRefPtr<IAction>> mActions;

    };

}

#endif /* INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_ */
