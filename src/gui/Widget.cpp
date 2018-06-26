/*
 * Widget.cpp
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#include "gui/Widget.h"

#include <osg/Geometry>

#include "NodeMasks.h"

namespace od
{

    Widget::Widget()
    : mWidgetGroup(new osg::Group)
    {
        mWidgetGroup->setNodeMask(NodeMasks::Gui);
    }

    void Widget::setVisible(bool b)
    {
        mWidgetGroup->setNodeMask(b ? NodeMasks::Gui : NodeMasks::Hidden);
    }

    void Widget::addGeometry(osg::Geometry *g)
    {
        if(mWidgetGeode == nullptr)
        {
            mWidgetGeode = new osg::Geode;
            mWidgetGroup->addChild(mWidgetGeode);
        }

        mWidgetGeode->addDrawable(g);
    }



}
