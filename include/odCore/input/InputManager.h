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

#include <odCore/RefCounted.h>
#include <odCore/WeakRefPtr.h>
#include <odCore/Exception.h>
#include <odCore/Upcast.h>

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

        void mouseMoved(float absX, float absY);
        void mouseButtonDown(int buttonCode);
        void mouseButtonUp(int buttonCode);

        void keyDown(Key key);
        void keyUp(Key key);

        template <typename _ActionEnum>
        od::RefPtr<ActionHandle<_ActionEnum>> getOrCreateAction(_ActionEnum action)
        {
            int actionCode = static_cast<int>(action);

            if(actionCode == 0)
            {
                throw od::InvalidArgumentException("Can't create actions whose int representation is 0");
            }

            od::RefPtr<ActionHandle<_ActionEnum>> actionHandle;
            auto &weakRef = mActions[actionCode];
            if(weakRef.isNull())
            {
                actionHandle = od::make_refd<ActionHandle<_ActionEnum>>(*this, action);
                weakRef = actionHandle.get();

            }else
            {
                auto ref = weakRef.aquire();
                actionHandle = od::upcast<ActionHandle<_ActionEnum>>(ref.get());
                if(actionHandle == nullptr)
                {
                    throw od::Exception("Upcast during retrieval of action failed. Are you mixing action implementations?");
                }
            }

            return actionHandle;
        }

        void bindActionToKey(IAction *action, Key key);
        void unbindActionFromKey(IAction *action, Key key);

        od::RefPtr<CursorListener> createCursorListener();


    private:

        void _triggerCallbackOnAction(int action, InputEvent event);

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
            std::array<int, MAX_BINDINGS> actions;
        };

        odGui::Gui *mGui;
        std::map<Key, Binding> mBindings;

        std::map<int, od::WeakObserverRefPtr<IAction>> mActions;

        std::vector<od::WeakObserverRefPtr<CursorListener>> mCursorListeners;
    };

}

#endif /* INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_ */
