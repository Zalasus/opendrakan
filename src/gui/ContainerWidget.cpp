/*
 * ContainerWidget.cpp
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#include "gui/ContainerWidget.h"

namespace od
{

    ContainerWidget::ContainerWidget(GuiManager &gm)
    : Widget(gm)
    , mDimType(WidgetDimensionType::ParentRelative)
    , mDimensions(1.0, 1.0)
    {
    }

    WidgetDimensionType ContainerWidget::getDimensionType() const
    {
        return mDimType;
    }

    osg::Vec2 ContainerWidget::getDimensions() const
    {
        return mDimensions;
    }

    void ContainerWidget::addWidget(Widget *w)
    {
        this->addChild(w);

        w->setParent(this);
    }

}
