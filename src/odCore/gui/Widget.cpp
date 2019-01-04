/*
 * Widget.cpp
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#include <odCore/gui/Widget.h>

#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <odCore/Exception.h>

#include <odCore/gui/Gui.h>

#include <odCore/render/Renderer.h>
#include <odCore/render/GuiNode.h>

namespace odGui
{

    Widget::Widget(Gui &gui)
    : Widget(gui, nullptr)
    {
        // doing this in the initializer list somehow seems to cause the RefCounted base
        //  to still be unitialized when creating the weakptr in the GuiNode??? WTF?
        mRenderNode = gui.getRenderer().createGuiNode(this);
    }

    Widget::Widget(Gui &gui, odRender::GuiNode *node)
    : mGui(gui)
    , mOrigin(WidgetOrigin::TopLeft)
    , mDimensionType(WidgetDimensionType::ParentRelative)
    , mDimensions(1.0, 1.0)
    , mPositionInParentSpace(0.0, 0.0)
    , mZIndex(0)
    , mParentWidget(nullptr)
    , mMatrixDirty(true)
    , mMouseOver(false)
    , mRenderNode(node)
    {
    }

    Widget::~Widget()
    {
    }

    bool Widget::liesWithinLogicalArea(const glm::vec2 &pos)
    {
        return pos.x >= 0.0 && pos.x <= 1.0 && pos.y >= 0.0 && pos.y <= 1.0;
    }

    void Widget::onMouseDown(const glm::vec2 &pos, int button)
    {
    }

    void Widget::onMouseEnter(const glm::vec2 &pos)
    {
    }

    void Widget::onMouseLeave(const glm::vec2 &pos)
    {
    }

    void Widget::onUpdate(float relTime)
    {
        if(mMatrixDirty)
        {
            updateMatrix();
        }
    }

    void Widget::addChild(Widget *w)
    {
        if(w == nullptr || w == this)
        {
            return;
        }

        mChildWidgets.push_back(od::RefPtr<Widget>(w));

        w->setParent(this);

        if(mRenderNode != nullptr)
        {
            mRenderNode->addChild(w->getRenderNode());
        }
    }

    void Widget::removeChild(Widget *w)
    {
        if(w == nullptr || w == this)
        {
            return;
        }

        auto it = std::find(mChildWidgets.begin(), mChildWidgets.end(), w);
        if(it != mChildWidgets.end())
        {
            mChildWidgets.erase(it);

            if(mRenderNode != nullptr)
            {
                mRenderNode->removeChild(w->getRenderNode());
            }
        }

        w->setParent(nullptr);
    }

    void Widget::intersect(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, std::vector<HitWidgetInfo> &hitWidgets)
    {
        glm::mat4 currentMatrix = mParentSpaceToWidgetSpace * parentMatrix;

        glm::vec4 pointInWidget = currentMatrix * glm::vec4(pointNdc, 0.0, 1.0);

        if(this->liesWithinLogicalArea(glm::vec2(pointInWidget)))
        {
            HitWidgetInfo info;
            info.hitPointInWidget.x = pointInWidget.x;
            info.hitPointInWidget.y = pointInWidget.y;
            info.widget = this;
            hitWidgets.push_back(info);

            for(auto &&child : mChildWidgets)
            {
                child->intersect(pointNdc, currentMatrix, hitWidgets);
            }
        }
    }

    glm::vec2 Widget::getDimensionsInPixels()
    {
        if(this->getDimensionType() == WidgetDimensionType::Pixels)
        {
            return this->getDimensions();

        }else
        {
            if(mParentWidget == nullptr)
            {
                throw od::Exception("Widget without parent asked to translate parent-relative dimensions to pixels. Is the GuiManager's root widget configured properly?");
            }

            return mParentWidget->getDimensionsInPixels() * this->getDimensions();
        }
    }

    void Widget::setVisible(bool b)
    {
        if(mRenderNode != nullptr)
        {
            mRenderNode->setVisible(b);
        }
    }

    void Widget::setZIndex(int32_t zIndex)
    {
        mZIndex = zIndex;

        if(mRenderNode != nullptr)
        {
            mRenderNode->setZIndex(zIndex);
        }

        if(mParentWidget != nullptr && mParentWidget->getRenderNode() != nullptr)
        {
            mParentWidget->getRenderNode()->reorderChildren();
        }
    }

    void Widget::updateMatrix()
    {
        if(mParentWidget == nullptr)
        {
            mParentSpaceToWidgetSpace = mGui.getNdcToWidgetSpaceTransform();
            mWidgetSpaceToParentSpace = mGui.getWidgetSpaceToNdcTransform();

        }else
        {
            mWidgetSpaceToParentSpace = glm::mat4(1.0);
            mWidgetSpaceToParentSpace = glm::translate(mWidgetSpaceToParentSpace, glm::vec3(mPositionInParentSpace, 0.0));
            glm::vec2 widgetSizeInParentSpace =
                    (mDimensionType == WidgetDimensionType::ParentRelative) ?
                      mDimensions : (getDimensionsInPixels() / mParentWidget->getDimensionsInPixels());
            mWidgetSpaceToParentSpace = glm::scale(mWidgetSpaceToParentSpace, glm::vec3(widgetSizeInParentSpace, 1.0));
            mWidgetSpaceToParentSpace = glm::translate(mWidgetSpaceToParentSpace, glm::vec3(-_getOriginVector(), 0.0));

            mParentSpaceToWidgetSpace = glm::inverse(mWidgetSpaceToParentSpace);
        }

        if(mRenderNode != nullptr)
        {
            mRenderNode->setMatrix(glm::transpose(mWidgetSpaceToParentSpace));
        }

        mMatrixDirty = false;
    }

    glm::vec2 Widget::_getOriginVector()
    {
        glm::vec2 origin;
        switch(mOrigin)
        {
        case WidgetOrigin::TopRight:
            origin = glm::vec2(1.0, 0.0);
            break;

        case WidgetOrigin::BottomRight:
            origin = glm::vec2(1.0, 1.0);
            break;

        case WidgetOrigin::BottomLeft:
            origin = glm::vec2(0.0, 1.0);
            break;

        case WidgetOrigin::Center:
            origin = glm::vec2(0.5, 0.5);
            break;

        case WidgetOrigin::TopLeft:
        default:
            origin = glm::vec2(0.0, 0.0);
            break;
        }

        return origin;
    }

}
