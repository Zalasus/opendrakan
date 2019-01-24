/*
 * Action.cpp
 *
 *  Created on: 13 Jan 2019
 *      Author: zal
 */

#include <odCore/input/Action.h>

#include <odCore/input/InputManager.h>

namespace odInput
{

    IAction::IAction(InputManager &im, int actionCode)
    : mInputManager(im)
    , mActionCode(actionCode)
    , mRepeatable(false)
    , mIgnoreUpEvents(false)
    {
    }

    void IAction::setRepeatable(bool b)
    {
        mRepeatable = b;
    }

    void IAction::setIgnoreUpEvents(bool b)
    {
        mIgnoreUpEvents = b;
    }

    void IAction::bindToKey(Key key)
    {
        mInputManager.bindActionToKey(this, key);
    }

    void IAction::unbindFromKey(Key key)
    {
        mInputManager.unbindActionFromKey(this, key);
    }

}
