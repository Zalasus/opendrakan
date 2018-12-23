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
    class UserInterfaceProperties;

    class MainMenu : public odGui::Widget
    {
    public:

        MainMenu(DragonGui &gui, UserInterfaceProperties *uiProps);


    private:

        DragonGui &mDragonGui;

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
