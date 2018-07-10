/*
 * DrawableWidget.cpp
 *
 *  Created on: 9 Jul 2018
 *      Author: zal
 */

#include "gui/DrawableWidget.h"

namespace od
{

    DrawableWidget::DrawableWidget(GuiManager &gm)
    : Widget(gm)
    {
    }

    std::pair<int32_t, int32_t> DrawableWidget::getZRange() const
    {
        return std::pair<int32_t, int32_t>(0, 0);
    }

    void DrawableWidget::addDrawable(osg::Drawable *drawable)
    {
        if(mGeode == nullptr)
        {
            mGeode = new osg::Geode;
            this->addChild(mGeode);
        }

        mGeode->addDrawable(drawable);
    }

}
