/*
 * WidgetIntersectVisitor.cpp
 *
 *  Created on: 8 Aug 2018
 *      Author: zal
 */

#include <odCore/gui/WidgetIntersectVisitor.h>

#include <osg/Transform>

#include <odCore/Logger.h>

namespace odGui
{

    WidgetIntersectVisitor::WidgetIntersectVisitor(const osg::Matrix &topWidgetToNdcXform, const osg::Matrix &ndcToTopWidgetXform)
    : osg::NodeVisitor(NODE_VISITOR, TRAVERSE_ALL_CHILDREN) // or is this an INTERSECTION_VISITOR?
    , mTopWidgetToNdcXform(topWidgetToNdcXform)
    , mNdcToTopWidgetXform(ndcToTopWidgetXform)
    , mCurrentMatrix(topWidgetToNdcXform)
    , mCurrentInverseMatrix(ndcToTopWidgetXform)
    {
    }

    void WidgetIntersectVisitor::reset()
    {
        mCurrentMatrix = mTopWidgetToNdcXform;
        mCurrentInverseMatrix = mNdcToTopWidgetXform;
        mHitWidgets.clear();
    }

    void WidgetIntersectVisitor::apply(osg::Node &node)
    {
        traverse(node);
    }

    void WidgetIntersectVisitor::apply(osg::Transform& node)
    {
        osg::Matrix parentMatrix = mCurrentMatrix;
        osg::Matrix parentInverseMatrix = mCurrentInverseMatrix;

        osg::Matrix transformMatrix;
        bool valid = node.computeLocalToWorldMatrix(mCurrentMatrix, this) && node.computeWorldToLocalMatrix(mCurrentInverseMatrix, this);
        if(!valid)
        {
            mCurrentMatrix = parentMatrix;
            mCurrentInverseMatrix = parentInverseMatrix;
        }

        osg::ref_ptr<Widget> widget = dynamic_cast<Widget*>(&node);
        if(widget != nullptr)
        {
            bool shouldTraverse = _intersect(widget);
            if(shouldTraverse)
            {
                traverse(node);
            }

        }else
        {
            // always traverse for non-widget transforms
            traverse(node);
        }

        mCurrentMatrix = parentMatrix;
        mCurrentInverseMatrix = parentInverseMatrix;
    }

    bool WidgetIntersectVisitor::_intersect(Widget *widget)
    {
        // widget min and max in NDC
        osg::Vec3 min = osg::Vec3(0.0, 1.0, 0.0) * mCurrentMatrix;
        osg::Vec3 max = osg::Vec3(1.0, 0.0, 0.0) * mCurrentMatrix;

        if(mPointNdc.x() < min.x() ||
           mPointNdc.x() > max.x() ||
           mPointNdc.y() < min.y() ||
           mPointNdc.y() > max.y())
        {
            return false;
        }

        osg::Vec3 pointInWidget = osg::Vec3(mPointNdc, 0.0) * mCurrentInverseMatrix;

        if(widget->liesWithinLogicalArea(osg::Vec2(pointInWidget.x(), pointInWidget.y())))
        {
            WidgetHitInfo info;
            info.hitPointInWidget.set(pointInWidget.x(), pointInWidget.y());
            info.widget = widget;
            mHitWidgets.push_back(info);
        }

        return true;
    }

}
