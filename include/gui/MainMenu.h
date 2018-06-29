/*
 * MainMenu.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_MAINMENU_H_
#define INCLUDE_GUI_MAINMENU_H_

#include "gui/Widget.h"

namespace od
{

    class GuiManager;

    class MainMenu : public Widget
    {
    public:

        MainMenu(GuiManager &gm);

        virtual osg::Vec2 getFullScaleDimensions() const override;

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
