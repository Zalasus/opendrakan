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

#include <odCore/RefCounted.h>

#include <odCore/input/Keys.h>

namespace odInput
{

    class InputManager;

    struct InputEvent
    {
        enum class Type
        {
            Down,
            Repeat,
            Up
        };

        Key key;
        float time;
        Type type;
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
    class IAction : public od::RefCounted
    {
    public:

        IAction(InputManager &im, int actionCode);
        virtual ~IAction() = default;

        inline int getActionAsInt() const { return mActionCode; }
        inline bool isRepeatable() const { return mRepeatable; }
        inline bool isDown() const { return mDown; }
        inline void setDown(bool b) { mDown = b; }
        /**
         * @brief When set to true, enables repeated sending of events if action key is held down. Default is false.
         *
         * The interval at which repeated events are sent might become adjustable in the future, but as of now is fixed
         * to the typematic interval defined by the OS.
         */
        void setRepeatable(bool b);
        void bindToKey(Key key);
        void unbindFromKey(Key key);

        virtual void triggerCallback(InputEvent) = 0;


    private:

        InputManager &mInputManager;
        int mActionCode;
        bool mRepeatable;

        bool mDown;

    };


    template <typename _ActionEnum>
    class ActionHandle : public IAction
    {
    public:

        typedef std::function<void(ActionHandle<_ActionEnum>*, InputEvent)> CallbackType;

        ActionHandle(InputManager &im, _ActionEnum action)
        : IAction(im, static_cast<int>(action))
        , mAction(action)
        {
        }

        inline _ActionEnum getAction() const { return mAction; }

        void addCallback(const CallbackType &callback)
        {
            mCallbacks.push_back(callback);
        }

        virtual void triggerCallback(InputEvent event) override
        {
            for(auto &cb : mCallbacks)
            {
                cb(this, event);
            }
        }


    private:

        _ActionEnum mAction;
        std::vector<CallbackType> mCallbacks;

    };

}

#endif /* INCLUDE_ODCORE_INPUT_ACTION_H_ */
