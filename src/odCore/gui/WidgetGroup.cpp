/*
 * WidgetGroup.cpp
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#include <odCore/gui/WidgetGroup.h>

#include <algorithm>

#include <odCore/gui/Widget.h>

namespace od
{


    WidgetGroup::WidgetGroup()
    {
    }

    WidgetGroup::WidgetGroup(const WidgetGroup &group, const osg::CopyOp &copyOp)
    : Group(group, copyOp)
    {
    }

    void WidgetGroup::childRemoved(unsigned int pos, unsigned int numChildrenToRemove)
    {
    }

    void WidgetGroup::childInserted(unsigned int pos)
    {
        auto pred = [](osg::ref_ptr<osg::Node> left, osg::ref_ptr<osg::Node> right)
        {
            Widget *l = dynamic_cast<Widget*>(left.get());
            if(l == nullptr) return false;

            Widget *r = dynamic_cast<Widget*>(right.get());
            if(r == nullptr) return false;

            return l->getZIndex() > r->getZIndex();
        };

        std::sort(this->_children.begin(), this->_children.end(), pred);
    }

}
