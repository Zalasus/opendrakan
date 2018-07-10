/*
 * MainMenu.h
 *
 *  Created on: 29 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_MAINMENU_H_
#define INCLUDE_GUI_MAINMENU_H_

#include "gui/DrawableWidget.h"

namespace od
{

    class GuiManager;

    class MainMenu : public DrawableWidget
    {
    public:

        MainMenu(GuiManager &gm);

        virtual WidgetDimensionType getDimensionType() const override;
        virtual osg::Vec2 getDimensions() const override;

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
