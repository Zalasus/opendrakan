/*
 * ContainerWidget.cpp
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#include "gui/ContainerWidget.h"

#include <limits>

#include "gui/WidgetGroup.h"

namespace od
{

    ContainerWidget::ContainerWidget(GuiManager &gm)
    : Widget(gm)
    , mChildWidgetGroup(new WidgetGroup)
    {
        this->addChild(mChildWidgetGroup);
    }

    std::pair<int32_t, int32_t> ContainerWidget::getZRange() const
    {
        int32_t min = std::numeric_limits<int32_t>::max();
        int32_t max = std::numeric_limits<int32_t>::min();
        for(auto it = mChildWidgets.begin(); it != mChildWidgets.end(); ++it)
        {
            min = std::min(min, (*it)->getZIndex());
            max = std::max(max, (*it)->getZIndex());
        }

        return std::make_pair(min, max);
    }

    void ContainerWidget::addWidget(Widget *w)
    {
        mChildWidgetGroup->addChild(w);
        mChildWidgets.push_back(osg::ref_ptr<Widget>(w));

        w->setParent(this);

        for(auto it = mChildWidgets.begin(); it != mChildWidgets.end(); ++it)
        {
            (*it)->updateMatrix();
        }
    }

}
