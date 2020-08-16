/*
 * Action.h
 *
 *  Created on: 6 Jan 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_INPUT_ACTION_H_
#define INCLUDE_ODCORE_INPUT_ACTION_H_

#include <functional>
#include <vector>

#include <odCore/input/Keys.h>

namespace odInput
{
    class InputManager;

    using ActionCode = int;

    enum class ActionState
    {
        BEGIN,
        END,
        REPEAT
    };

    /**
     * Interface for a registration of a input action. It allows configuration of the action and manages
     * a set of callbacks. The InputManager keeps weak references to actions, so if all references to an action
     * outside of the InputManager are destroyed, the action itself will be cleanly deallocated and no longer considered
     * by the InputManager. Thus, ActionHandles require no registration and unregistration mechanism
     *
     * Note: the only abstraction happening here is the type of the action enum. In theory, this whole abstract thing
     * could be omitted if we were willing to have the user either use unscoped enums or cast everything to int by himself.
     */
    class IAction
    {
    public:

        IAction(InputManager &im, int actionCode);
        virtual ~IAction() = default;

        inline ActionCode getActionCode() const { return mActionCode; }
        inline bool isRepeatable() const { return mRepeatable; }
        inline bool ignoresUpEvents() const { return mIgnoreUpEvents; }

        /**
         * @brief When set to true, enables repeated sending of events if action key is held down. Default is false.
         *
         * The interval at which repeated events are sent might become adjustable in the future, but as of now is fixed
         * to the typematic interval defined by the OS.
         */
        void setRepeatable(bool b);

        /**
         * @brief If set to true, no callbacks will be triggered when any key bound to this action is released. Default is false.
         */
        void setIgnoreUpEvents(bool b);

        virtual void triggerCallback(ActionState state) = 0;


    protected:

        ActionCode mActionCode;

    private:

        InputManager &mInputManager;
        bool mRepeatable;
        bool mIgnoreUpEvents;

    };


    template <typename _ActionEnum>
    class ActionHandle final : public IAction
    {
    public:

        typedef std::function<void(_ActionEnum, ActionState)> CallbackType;

        ActionHandle(InputManager &im, _ActionEnum action)
        : IAction(im, static_cast<ActionCode>(action))
        {
        }

        inline _ActionEnum getAction() const { return static_cast<_ActionEnum>(mActionCode); }

        void addCallback(const CallbackType &callback)
        {
            mCallbacks.push_back(callback);
        }

        virtual void triggerCallback(ActionState state) override
        {
            for(auto &cb : mCallbacks)
            {
                cb(getAction(), state);
            }
        }


    private:

        std::vector<CallbackType> mCallbacks;

    };

}

#endif /* INCLUDE_ODCORE_INPUT_ACTION_H_ */
