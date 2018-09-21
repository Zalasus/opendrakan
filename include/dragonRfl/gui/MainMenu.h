/*
 * MainMenu.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_MAINMENU_H_
#define INCLUDE_GUI_MAINMENU_H_

#include <odCore/gui/ContainerWidget.h>

namespace od
{

    class GuiManager;
    class UserInterfaceProperties;
    class Model;

    class MainMenu : public ContainerWidget
    {
    public:

        MainMenu(GuiManager &gm, od::UserInterfaceProperties *uiProps);

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
                od::UserInterfaceProperties *uiProps, ContainerWidget *cont, int buttonCode);

        void _buttonClicked(int buttonCode);

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
