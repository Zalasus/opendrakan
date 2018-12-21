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
    : Widget(gui, gui.getRenderer().createGuiNode())
    {
    }

    Widget::Widget(Gui &gui, odRender::GuiNode *node)
    : mGui(gui)
    , mOrigin(WidgetOrigin::TopLeft)
    , mDimensionType(WidgetDimensionType::ParentRelative)
    , mDimensions(1.0, 1.0)
    , mPositionInParentSpace(0.0, 0.0)
    , mZIndexInParentSpace(0)
    , mParentWidget(nullptr)
    , mMatrixDirty(false)
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

    void Widget::intersect(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, const glm::mat4 &parentInverseMatrix, std::vector<HitWidgetInfo> &hitWidgets)
    {
        glm::mat4 currentMatrix = mParentSpaceToWidgetSpace * parentMatrix;
        glm::mat4 currentInverseMatrix = mWidgetSpaceToParentSpace * parentInverseMatrix;

        glm::vec4 min = glm::vec4(0.0, 1.0, 0.0, 1.0) * currentMatrix;
        glm::vec4 max = glm::vec4(1.0, 0.0, 0.0, 1.0) * currentMatrix;

        if(pointNdc.x < min.x ||
           pointNdc.x > max.x ||
           pointNdc.y < min.y ||
           pointNdc.y > max.y)
        {
            return;
        }

        glm::vec4 pointInWidget = glm::vec4(pointNdc, 0.0, 1.0) * currentInverseMatrix;

        if(this->liesWithinLogicalArea(glm::vec2(pointInWidget)))
        {
            HitWidgetInfo info;
            info.hitPointInWidget.x = pointInWidget.x;
            info.hitPointInWidget.y = pointInWidget.y;
            info.widget = this;
            hitWidgets.push_back(info);
        }

        for(auto &&child : mChildWidgets)
        {
            child->intersect(pointNdc, currentMatrix, currentInverseMatrix, hitWidgets);
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
    }

    void Widget::updateMatrix()
    {
        if(!mMatrixDirty)
        {
            return;
        }

        if(mParentWidget == nullptr)
        {
            // for the root widget, both of these are identity. the transformation widget space -> NDC is handled
            //  by passing the appropriate matrix to flatten()
            mParentSpaceToWidgetSpace = glm::mat4(1.0);
            mWidgetSpaceToParentSpace = mParentSpaceToWidgetSpace;

        }else
        {
            glm::mat4 matrix(1.0);

            matrix = glm::translate(matrix, glm::vec3(-_getOriginVector(), 0.0));

            glm::vec2 widgetSizeInParentSpace = getDimensionsInPixels() / mParentWidget->getDimensionsInPixels();
            matrix = glm::scale(matrix, glm::vec3(widgetSizeInParentSpace, 1.0));
            matrix = glm::translate(matrix, glm::vec3(mPositionInParentSpace, 0.0));

            mParentSpaceToWidgetSpace = matrix;
            mWidgetSpaceToParentSpace = glm::inverse(matrix);
        }

        if(mRenderNode != nullptr)
        {
            mRenderNode->setMatrix(mWidgetSpaceToParentSpace);
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
