/*
 * InputManager.cpp
 *
 *  Created on: 29 Dec 2018
 *      Author: zal
 */

#include <odCore/input/InputManager.h>

#include <odCore/gui/Gui.h>

namespace odInput
{

    InputManager::InputManager()
    : mGui(nullptr)
    {
    }

    void InputManager::mouseMoved(float absX, float absY)
    {
        if(mGui != nullptr)
        {
            mGui->setCursorPosition(glm::vec2(absX, absY));
        }
    }

    void InputManager::mouseButtonDown(int buttonCode)
    {
        if(mGui != nullptr)
        {
            mGui->mouseDown();
        }
    }

    void InputManager::mouseButtonUp(int buttonCode)
    {

    }

}
