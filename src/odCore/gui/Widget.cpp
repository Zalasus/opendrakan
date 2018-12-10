/*
 * Widget.cpp
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#include <odCore/gui/Widget.h>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <odCore/gui/GuiManager.h>

namespace odGui
{

    Widget::Widget(GuiManager &gm)
    : mGuiManager(gm)
    , mOrigin(WidgetOrigin::TopLeft)
    , mDimensionType(WidgetDimensionType::ParentRelative)
    , mDimensions(1.0, 1.0)
    , mPositionInParentSpace(0.0, 0.0)
    , mZIndexInParentSpace(0)
    , mParentWidget(nullptr)
    , mMatrixDirty(false)
    , mMouseOver(false)
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

    void Widget::onUpdate(double simTime, double relTime)
    {
    }

    void Widget::intersect(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, const glm::mat4 &parentInverseMatrix, std::vector<HitWidgetInfo> &hitWidgets)
    {
        glm::mat4 currentMatrix = mParentSpaceToWidgetSpace * parentMatrix;
        glm::mat4 currentInverseMatrix = mWidgetSpaceToParentSpace * parentInverseMatrix;

        glm::vec3 min = glm::vec4(0.0, 1.0, 0.0, 1.0) * currentMatrix;
        glm::vec3 max = glm::vec4(1.0, 0.0, 0.0, 1.0) * currentMatrix;

        if(pointNdc.x < min.x ||
           pointNdc.x > max.x ||
           pointNdc.y < min.y ||
           pointNdc.y > max.y)
        {
            return;
        }

        glm::vec3 pointInWidget = glm::vec4(pointNdc, 0.0, 1.0) * currentInverseMatrix;

        if(this->liesWithinLogicalArea(glm::vec2(pointInWidget)))
        {
            HitWidgetInfo info;
            info.hitPointInWidget = pointInWidget;
            info.widget = this;
            hitWidgets.push_back(info);
        }

        this->intersectChildren(pointNdc, currentMatrix, currentInverseMatrix, hitWidgets);
    }

    void Widget::intersectChildren(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, const glm::mat4 &parentInverseMatrix, std::vector<HitWidgetInfo> &hitWidgets)
    {
    }

    glm::vec2 Widget::getDimensionsInPixels()
    {
        if(this->getDimensionType() == WidgetDimensionType::Pixels)
        {
            return this->getDimensions();

        }else
        {
            return this->getParentDimensionsInPixels() * this->getDimensions();
        }
    }

    glm::vec2 Widget::getParentDimensionsInPixels()
    {
        return (mParentWidget != nullptr) ? mParentWidget->getDimensionsInPixels() : mGuiManager.getScreenResolution();
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

        glm::mat4 matrix(1.0);

        matrix = glm::translate(matrix, glm::vec3(-_getOriginVector(), 0.0));

        glm::vec2 widgetSizeInParentSpace = this->getDimensionsInPixels() / this->getParentDimensionsInPixels();
        matrix = glm::scale(matrix, glm::vec3(widgetSizeInParentSpace, 1.0));
        matrix = glm::translate(matrix, glm::vec3(mPositionInParentSpace, 0.0));

        mParentSpaceToWidgetSpace = matrix;
        mWidgetSpaceToParentSpace = glm::inverse(matrix);

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
