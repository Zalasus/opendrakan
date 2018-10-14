/*
 * Widget.cpp
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#include <odCore/gui/Widget.h>

#include <osg/Geometry>
#include <osgGA/EventVisitor>

#include <odCore/NodeMasks.h>
#include <odCore/UpdateCallback.h>
#include <odCore/gui/GuiManager.h>

namespace odGui
{

    class WidgetUpdateCallback : public od::UpdateCallback
    {
    public:

        WidgetUpdateCallback(Widget *widget)
        : mWidget(widget)
        {
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            tick(nv->getFrameStamp());

            mWidget->onUpdate(mSimTime, mRelTime);

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
    , mMouseOver(false)
    {
        this->setNodeMask(od::NodeMasks::Gui);
        this->setMatrix(osg::Matrix::identity());

        this->addUpdateCallback(mUpdateCallback);
    }

    Widget::~Widget()
    {
        this->removeUpdateCallback(mUpdateCallback);
    }

    bool Widget::liesWithinLogicalArea(const osg::Vec2 &pos)
    {
        return pos.x() >= 0.0 && pos.x() <= 1.0 && pos.y() >= 0.0 && pos.y() <= 1.0;
    }

    void Widget::onMouseDown(const osg::Vec2 &pos, int button)
    {
    }

    void Widget::onMouseEnter(const osg::Vec2 &pos)
    {
    }

    void Widget::onMouseLeave(const osg::Vec2 &pos)
    {
    }

    void Widget::onUpdate(double simTime, double relTime)
    {
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
        this->setNodeMask(b ? od::NodeMasks::Gui : od::NodeMasks::Hidden);
    }

    void Widget::updateMatrix()
    {
        if(!mMatrixDirty)
        {
            return;
        }

        osg::Matrix matrix = osg::Matrix::identity();

        matrix.postMultTranslate(osg::Vec3(-_getOriginVector(), 0.0));

        osg::Vec2 widgetSizeInParentSpace = osg::componentDivide(this->getDimensionsInPixels(), this->getParentDimensionsInPixels());
        matrix.postMultScale(osg::Vec3(widgetSizeInParentSpace, 1.0)); // remove z component

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

    osg::Vec2 Widget::_getOriginVector()
    {
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

        return origin;
    }

}
