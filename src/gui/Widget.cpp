/*
 * Widget.cpp
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#include "gui/Widget.h"

#include <osg/Geometry>
#include <osgGA/EventVisitor>

#include "NodeMasks.h"
#include "gui/GuiManager.h"

namespace od
{

    class WidgetEventCallback : public osg::NodeCallback
    {
    public:

        WidgetEventCallback(Widget *widget)
        : mWidget(widget)
        {
        }

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            if(nv->getVisitorType() != osg::NodeVisitor::EVENT_VISITOR)
            {
                traverse(node, nv);
                return;
            }

            osg::ref_ptr<osgGA::EventVisitor> ev = dynamic_cast<osgGA::EventVisitor*>(nv);
            if(ev == nullptr)
            {
                // the visitor type was lying!
                traverse(node, nv);
                return;
            }

            bool shouldTraverse = _handleEvents(ev);
            if(shouldTraverse)
            {
                traverse(node, nv);
            }
        }


    private:

        bool _handleEvents(osgGA::EventVisitor *ev)
        {
            bool shouldTraverse = false;

            osgGA::EventQueue::Events &events = ev->getEvents();

            for(auto it = events.begin(); it != events.end(); ++it)
            {
                osg::ref_ptr<osgGA::GUIEventAdapter> ea = (*it)->asGUIEventAdapter();
                if(ea == nullptr)
                {
                    continue;
                }

                switch(ea->getEventType())
                {
                case osgGA::GUIEventAdapter::MOVE:
                case osgGA::GUIEventAdapter::DRAG:
                case osgGA::GUIEventAdapter::PUSH:
                case osgGA::GUIEventAdapter::RELEASE:
                    shouldTraverse |= _handleMouseEvent(ea);
                    break;

                case osgGA::GUIEventAdapter::KEYUP:
                case osgGA::GUIEventAdapter::KEYDOWN:
                    shouldTraverse = true;
                    // TODO: handle keyboard events
                    break;

                default:
                    shouldTraverse = false;
                    break;
                }
            }

            return shouldTraverse;
        }

        bool _handleMouseEvent(osgGA::GUIEventAdapter *ea)
        {
            osg::Vec2 screenSize(ea->getXmax() - ea->getXmin(), ea->getYmax() - ea->getYmin());
            osg::Vec2 cursorPosRootWidget((ea->getXnormalized()+1)/2, (1-ea->getYnormalized())/2);
            osg::Vec2 cursorPosPixel(osg::componentMultiply(cursorPosRootWidget, screenSize));

            osg::Vec2 widgetMin = mWidget->getTopLeftPositionInPixels();
            osg::Vec2 widgetMax = widgetMin + mWidget->getDimensionsInPixels();

            // check whether mouse event lies within widget
            if(cursorPosPixel.x() < widgetMin.x() ||
               cursorPosPixel.x() > widgetMax.x() ||
               cursorPosPixel.y() < widgetMin.y() ||
               cursorPosPixel.y() > widgetMax.y())
            {
                return false;
            }

            osg::Vec2 cursorWidgetSpace = osg::componentDivide(cursorPosPixel - widgetMin, mWidget->getDimensionsInPixels());

            switch(ea->getEventType())
            {
            case osgGA::GUIEventAdapter::PUSH:
                mWidget->onMouseDown(cursorWidgetSpace, ea->getButton());
                break;

            default:
                break;
            }

            return true;
        }

        Widget *mWidget;

    };


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
    , mMouseOver(false)
    {
        this->setNodeMask(NodeMasks::Gui);
        this->setMatrix(osg::Matrix::identity());

        this->addUpdateCallback(mUpdateCallback);
        this->addEventCallback(new WidgetEventCallback(this));
    }

    Widget::~Widget()
    {
        this->removeUpdateCallback(mUpdateCallback);
    }

    void Widget::onMouseDown(osg::Vec2 pos, int button)
    {
    }

    osg::Vec2 Widget::getTopLeftPositionInPixels()
    {
        if(mParentWidget == nullptr)
        {
            return osg::componentMultiply(mPositionInParentSpace, mGuiManager.getScreenResolution()) -
                    osg::componentMultiply(_getOriginVector(), this->getDimensionsInPixels());

        }else
        {
            return osg::componentMultiply(mPositionInParentSpace, mParentWidget->getDimensionsInPixels())
                + mParentWidget->getTopLeftPositionInPixels()
                 - osg::componentMultiply(_getOriginVector(), this->getDimensionsInPixels());
        }
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

        matrix.postMultTranslate(osg::Vec3(-_getOriginVector(), 0.0));

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
