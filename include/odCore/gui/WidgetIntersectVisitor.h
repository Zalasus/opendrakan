/*
 * WidgetIntersectVisitor.h
 *
 *  Created on: 8 Aug 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_WIDGETINTERSECTVISITOR_H_
#define INCLUDE_GUI_WIDGETINTERSECTVISITOR_H_

#include <osg/NodeVisitor>

#include <odCore/gui/Widget.h>

namespace odGui
{

    struct WidgetHitInfo
    {
        osg::Vec2 hitPointInWidget;
        osg::ref_ptr<Widget> widget;
    };

    class WidgetIntersectVisitor : public osg::NodeVisitor
    {
    public:

        WidgetIntersectVisitor(const osg::Matrix &topWidgetToNdcXform, const osg::Matrix &ndcToTopWidgetXform);

        inline void setPointInNdc(const osg::Vec2 &pointNdc) { mPointNdc = pointNdc; }
        inline const std::vector<WidgetHitInfo> &getHitWidgets() { return mHitWidgets; }

        virtual void reset() override;
        virtual void apply(osg::Node& node) override;
        virtual void apply(osg::Transform& node) override;


    private:

        bool _intersect(Widget *w);

        osg::Matrix mTopWidgetToNdcXform;
        osg::Matrix mNdcToTopWidgetXform;
        osg::Matrix mCurrentMatrix;
        osg::Matrix mCurrentInverseMatrix;
        osg::Vec2 mPointNdc;
        std::vector<WidgetHitInfo> mHitWidgets;

    };

}

#endif /* INCLUDE_GUI_WIDGETINTERSECTVISITOR_H_ */
