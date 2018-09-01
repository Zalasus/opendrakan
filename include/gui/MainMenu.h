/*
 * MainMenu.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_MAINMENU_H_
#define INCLUDE_GUI_MAINMENU_H_

#include "gui/ContainerWidget.h"
#include "rfl/dragon/UserInterfaceProperties.h"

namespace od
{

    class GuiManager;

    class MainMenu : public ContainerWidget
    {
    public:

        MainMenu(GuiManager &gm, odRfl::UserInterfaceProperties *uiProps);

    private:

        enum ButtonCode
        {
            BC_MULTIPLAYER,
            BC_LOAD,
            BC_NEW,
            BC_SAVE,
            BC_OPTIONS,
            BC_CREDITS,
            BC_QUIT
        };

        void _addCrystal(GuiManager &gm, Model *crystalModel, float dia, float x, float y,
                odRfl::UserInterfaceProperties *uiProps, ContainerWidget *cont, int buttonCode);

        void _buttonClicked(int buttonCode);

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
