/*
 * ContainerWidget.cpp
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#include <odCore/gui/ContainerWidget.h>

#include <limits>

#include <odCore/gui/WidgetGroup.h>

namespace odGui
{

    ContainerWidget::ContainerWidget(GuiManager &gm)
    : Widget(gm)
    , mChildWidgetGroup(new WidgetGroup)
    {
        this->addChild(mChildWidgetGroup);
    }

    void ContainerWidget::addWidget(Widget *w)
    {
        mChildWidgetGroup->addChild(w);
        mChildWidgets.push_back(osg::ref_ptr<Widget>(w));

        w->setParent(this);
    }

}
