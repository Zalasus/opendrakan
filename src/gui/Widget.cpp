/*
 * Widget.cpp
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#include "gui/Widget.h"

#include <osg/Geometry>

#include "NodeMasks.h"
#include "gui/GuiManager.h"

namespace od
{

    class WidgetUpdateCallback : public osg::NodeCallback
    {
    public:

        WidgetUpdateCallback(Widget *widget)
        : mWidget(widget)
        {
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            if(mWidget->isMatrixDirty())
            {
                mWidget->updateMatrix();
            }

            traverse(node, nv);
        }

    private:

        Widget *mWidget;

    };

    Widget::Widget(GuiManager &gm)
    : mGuiManager(gm)
    , mOrigin(WidgetOrigin::TopLeft)
    , mDimensionType(WidgetDimensionType::ParentRelative)
    , mDimensions(1.0, 1.0)
    , mPositionInParentSpace(0.0, 0.0)
    , mZIndexInParentSpace(0)
    , mParentWidget(nullptr)
    , mMatrixDirty(false)
    , mUpdateCallback(new WidgetUpdateCallback(this))
    {
        this->setNodeMask(NodeMasks::Gui);
        this->setMatrix(osg::Matrix::identity());

        this->addUpdateCallback(mUpdateCallback);
    }

    Widget::~Widget()
    {
        this->removeUpdateCallback(mUpdateCallback);
    }

    osg::Vec2 Widget::getDimensionsInPixels()
    {
        if(this->getDimensionType() == WidgetDimensionType::Pixels)
        {
            return this->getDimensions();

        }else
        {
            return osg::componentMultiply(this->getParentDimensionsInPixels(), this->getDimensions());
        }
    }

    osg::Vec2 Widget::getParentDimensionsInPixels()
    {
        return (mParentWidget != nullptr) ? mParentWidget->getDimensionsInPixels() : mGuiManager.getScreenResolution();
    }

    void Widget::setVisible(bool b)
    {
        this->setNodeMask(b ? NodeMasks::Gui : NodeMasks::Hidden);
    }

    void Widget::updateMatrix()
    {
        if(!mMatrixDirty)
        {
            return;
        }

        osg::Matrix matrix = osg::Matrix::identity();

        osg::Vec2 origin;
        switch(mOrigin)
        {
        case WidgetOrigin::TopRight:
            origin = osg::Vec2(1.0, 0.0);
            break;

        case WidgetOrigin::BottomRight:
            origin = osg::Vec2(1.0, 1.0);
            break;

        case WidgetOrigin::BottomLeft:
            origin = osg::Vec2(0.0, 1.0);
            break;

        case WidgetOrigin::Center:
            origin = osg::Vec2(0.5, 0.5);
            break;

        case WidgetOrigin::TopLeft:
        default:
            origin = osg::Vec2(0.0, 0.0);
            break;
        }

        matrix.postMultTranslate(osg::Vec3(-origin, 0.0));

        osg::Vec2 widgetSizeInParentSpace = osg::componentDivide(this->getDimensionsInPixels(), this->getParentDimensionsInPixels());
        matrix.postMultScale(osg::Vec3(widgetSizeInParentSpace, 0.0)); // remove z component

        matrix.postMultTranslate(osg::Vec3(mPositionInParentSpace, 0.0));

        this->setMatrix(matrix);

        mMatrixDirty = false;
    }

    void Widget::addDrawable(osg::Drawable *drawable)
    {
        if(mGeode == nullptr)
        {
            mGeode = new osg::Geode;
            this->addChild(mGeode);
        }

        mGeode->addDrawable(drawable);
    }

}
