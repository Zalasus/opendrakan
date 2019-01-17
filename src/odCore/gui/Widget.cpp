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
    : mGui(gui)
    , mOrigin(WidgetOrigin::TopLeft)
    , mDimensionType(WidgetDimensionType::ParentRelative)
    , mDimensions(1.0, 1.0)
    , mPositionInParentSpace(0.0, 0.0)
    , mZIndex(0)
    , mParentWidget(nullptr)
    , mMatrixDirty(true)
    , mGlobalZIndex(0)
    , mMouseOver(false)
    , mChildOrderDirty(false)
    , mNeedsFlattening(true)
    {
        mRenderNode = mGui.getRenderer().createGuiNode(this);
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
    }

    void Widget::addChild(Widget *w)
    {
        if(w == nullptr || w == this)
        {
            return;
        }

        mChildWidgets.push_back(od::RefPtr<Widget>(w));

        w->setParent(this);
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

        if(mParentWidget != nullptr)
        {
            mParentWidget->mChildOrderDirty = true;
        }
    }

    void Widget::reorderChildren()
    {
        auto pred = [](od::RefPtr<Widget> &left, od::RefPtr<Widget> &right) { return left->getZIndex() < right->getZIndex(); };
        std::sort(mChildWidgets.begin(), mChildWidgets.end(), pred);

        mChildOrderDirty = false;
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

    void Widget::flatten()
    {
        glm::mat4 m(1.0);
        int32_t globalZIndex = 0;

        if(mParentWidget != nullptr)
        {
            m = mParentWidget->mMySpaceToRootSpace;
            globalZIndex = mParentWidget->mGlobalZIndex;
        }

        flatten(m, globalZIndex);
    }

    void Widget::flatten(const glm::mat4 &parentMatrix, int32_t &parentGlobalZIndex)
    {
        if(mMatrixDirty)
        {
            updateMatrix();
        }

        mMySpaceToRootSpace = mWidgetSpaceToParentSpace * parentMatrix;
        mGlobalZIndex = ++parentGlobalZIndex;
        if(mRenderNode != nullptr)
        {
            mRenderNode->setMatrix(glm::transpose(mMySpaceToRootSpace));
            mRenderNode->setZIndex(parentGlobalZIndex);
        }

        if(mChildOrderDirty)
        {
            reorderChildren();
        }

        for(auto &child : mChildWidgets)
        {
            if(child->mNeedsFlattening)
            {
                child->flatten(mMySpaceToRootSpace, parentGlobalZIndex);
            }
        }

        mNeedsFlattening = false;
    }

    void Widget::update(float relTime)
    {
        if(mNeedsFlattening)
        {
            flatten();
        }

        this->onUpdate(relTime);
    }

    odRender::GuiNode *Widget::getRenderNode()
    {
        return mRenderNode;
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

    void Widget::_markThisAndParentsAsNeedingFlattening()
    {
        mNeedsFlattening = true;

        Widget *w = mParentWidget;
        while(w != nullptr)
        {
            w->mNeedsFlattening = true;
            w = w->mParentWidget;
        }
    }

}
