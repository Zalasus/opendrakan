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

    ActionHandleBase::ActionHandleBase(InputManager &im, ActionCode actionCode)
    : mActionCode(actionCode)
    , mInputManager(im)
    , mRepeatable(false)
    , mIgnoreUpEvents(false)
    {
    }

    void ActionHandleBase::setRepeatable(bool b)
    {
        mRepeatable = b;
    }

    void ActionHandleBase::setIgnoreUpEvents(bool b)
    {
        mIgnoreUpEvents = b;
    }

}
