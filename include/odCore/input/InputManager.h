/*
 * InputManager.h
 *
 *  Created on: 28 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_
#define INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_

#include <map>

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



    private:

        odGui::Gui *mGui;

    };

}

#endif /* INCLUDE_ODCORE_INPUT_INPUTMANAGER_H_ */
