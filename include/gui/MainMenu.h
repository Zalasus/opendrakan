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

        virtual WidgetDimensionType getDimensionType() const override;
        virtual osg::Vec2 getDimensions() const override;

    };

}

#endif /* INCLUDE_GUI_MAINMENU_H_ */
