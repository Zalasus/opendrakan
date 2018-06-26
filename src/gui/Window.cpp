/*
 * Window.cpp
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */


#include "gui/Window.h"

#include "NodeMasks.h"
#include "gui/GuiManager.h"

namespace od
{

    Window::Window(GuiManager &guiManager)
    : mGuiManager(guiManager)
    , mOrigin(WindowOrigin::TopLeft)
    , mScale(1.0, 1.0)
    {
        this->setNodeMask(NodeMasks::Gui);
    }

    void Window::setScale(const osg::Vec2 &scale)
    {
        mScale = scale;

        _updateMatrix();
    }

    void Window::setOrigin(WindowOrigin origin)
    {
        mOrigin = origin;

        _updateMatrix();
    }

    void Window::setPosition(const osg::Vec2 &pos)
    {
        mPositionInScreenSpace = pos;

        _updateMatrix();
    }

    void Window::setChildWidget(Widget *w)
    {
        if(mChildWidget != nullptr)
        {
            this->removeChild(mChildWidget->mWidgetGroup);
        }

        mChildWidget = w;

        if(mChildWidget != nullptr)
        {
            this->addChild(mChildWidget->mWidgetGroup);
        }

        _updateMatrix();
    }

    void Window::_updateMatrix()
    {
        if(mChildWidget == nullptr)
        {
            return;
        }

        osg::Matrix matrix = osg::Matrix::identity();

        osg::Vec2 origin;
        switch(mOrigin)
        {
        case WindowOrigin::TopRight:
            origin = osg::Vec2(1.0, 0.0);
            break;

        case WindowOrigin::BottomRight:
            origin = osg::Vec2(1.0, 1.0);
            break;

        case WindowOrigin::BottomLeft:
            origin = osg::Vec2(0.0, 1.0);
            break;

        case WindowOrigin::Center:
            origin = osg::Vec2(0.5, 0.5);
            break;

        case WindowOrigin::TopLeft:
        default:
            origin = osg::Vec2(0.0, 0.0);
            break;
        }

        osg::Vec2 widgetSizePixel = osg::componentMultiply(mChildWidget->getFullScaleDimensions(), mScale);
        osg::Vec2 widgetSizeInScreenSpace = osg::componentDivide(widgetSizePixel*2.0, mGuiManager.getScreenResolution());

        osg::Vec2 originOffset = mPositionInScreenSpace - osg::componentMultiply(origin, widgetSizeInScreenSpace);
        matrix.postMultTranslate(osg::Vec3(-originOffset, 0.0));

        osg::Vec2 scaleFactor = osg::componentDivide(osg::Vec2(1.0, 1.0), widgetSizeInScreenSpace);
        matrix.postMultScale(osg::Vec3(scaleFactor, 1.0));

        // flip y axis
        matrix.postMultScale(osg::Vec3(1.0, -1.0, 1.0));

        this->setMatrix(matrix);
    }

}
