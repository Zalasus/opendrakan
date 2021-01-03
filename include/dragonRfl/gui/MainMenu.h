/*
 * MainMenu.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_MAINMENU_H_
#define INCLUDE_GUI_MAINMENU_H_

#include <odCore/gui/Widget.h>

namespace odDb
{
    class Model;
}

namespace dragonRfl
{

    class DragonGui;
    class UserInterfacePropertiesFields;

    class MainMenu : public odGui::Widget
    {
    public:

        MainMenu(DragonGui &gui, UserInterfacePropertiesFields *uiProps);


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

        void _addCrystal(DragonGui &gui, std::shared_ptr<odDb::Model> crystalModel, float noteOffset, float dia, float x, float y,
            UserInterfacePropertiesFields *uiProps, std::shared_ptr<odGui::Widget> cont, int buttonCode);

        void _buttonClicked(int buttonCode);

        DragonGui &mDragonGui;

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
