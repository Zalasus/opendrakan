/*
 * WidgetGroup.h
 *
 *  Created on: 13 Jul 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_WIDGETGROUP_H_
#define INCLUDE_GUI_WIDGETGROUP_H_

#include <osg/Group>

namespace od
{

    /**
     * Extension of osg::Group that keeps it's node list sorted by widget depth
     */
    class WidgetGroup : public osg::Group
    {
    public:

        WidgetGroup();
        WidgetGroup(const WidgetGroup &group, const osg::CopyOp &copyOp = osg::CopyOp::SHALLOW_COPY);

        META_Node(od, WidgetGroup);


    protected:

        virtual void childRemoved(unsigned int pos, unsigned int numChildrenToRemove) override;
        virtual void childInserted(unsigned int pos) override;

    };

}

#endif /* INCLUDE_GUI_WIDGETGROUP_H_ */
