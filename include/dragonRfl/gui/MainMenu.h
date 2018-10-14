/*
 * MainMenu.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_MAINMENU_H_
#define INCLUDE_GUI_MAINMENU_H_

#include <odCore/gui/ContainerWidget.h>

namespace odDb
{
    class Model;
}

namespace dragonRfl
{

    class UserInterfaceProperties;

    class MainMenu : public odGui::ContainerWidget
    {
    public:

        MainMenu(odGui::GuiManager &gm, UserInterfaceProperties *uiProps);


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

        void _addCrystal(odGui::GuiManager &gm, odDb::Model *crystalModel, float noteOffset, float dia, float x, float y,
                UserInterfaceProperties *uiProps, odGui::ContainerWidget *cont, int buttonCode);

        void _buttonClicked(int buttonCode);

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
