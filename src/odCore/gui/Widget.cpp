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
#include <odCore/render/Handle.h>
#include <odCore/render/Group.h>

namespace odGui
{
    Widget::Widget(Gui &gui)
    : mGui(gui)
    , mOrigin(WidgetOrigin::TopLeft)
    , mDimensionType(WidgetDimensionType::ParentRelative)
    , mDimensions(1.0, 1.0)
    , mPositionInParentSpace(0.0, 0.0)
    , mZIndex(0)
    , mVisible(true)
    , mParentWidget(nullptr)
    , mChildrenInNeedOfUpdate(0)
    , mNeedsUpdate(false)
    , mMatrixDirty(true)
    , mMouseOver(false)
    {
    }

    Widget::~Widget()
    {
    }

    void Widget::setPosition(const glm::vec2 &pos)
    {
        // TODO: handle edge cases where we have no children and could directly apply the change from a stored, previously flattened matrix
        mPositionInParentSpace = pos;
        mMatrixDirty = true;

        mGui.markTransformsDirty();
    }

    void Widget::setDimensions(const glm::vec2 &dim)
    {
        mDimensions = dim;
        mMatrixDirty = true;

        mGui.markMeasurementsDirty();
        mGui.markTransformsDirty();
    }

    void Widget::setDimensions(const glm::vec2 &dim, WidgetDimensionType type)
    {
        mDimensions = dim;
        mDimensionType = type;
        mMatrixDirty = true;

        mGui.markMeasurementsDirty();
        mGui.markTransformsDirty();
    }

    void Widget::setDimensionType(WidgetDimensionType type)
    {
        mDimensionType = type;
        mMatrixDirty = true;

        mGui.markMeasurementsDirty();
        mGui.markTransformsDirty();
    }

    void Widget::setOrigin(WidgetOrigin origin)
    {
        mOrigin = origin;
        mMatrixDirty = true;

        mGui.markTransformsDirty();
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

    void Widget::addChild(std::shared_ptr<Widget> w)
    {
        OD_CHECK_ARG_NONNULL(w);

        if(w->mParentWidget != nullptr)
        {
            throw od::Exception("Tried to add Widget that already had a parent");
        }

        mChildWidgets.push_back(w);
        w->mParentWidget = this;
        mGui.markDepthDirty();
        mGui.markTransformsDirty();
        mGui.markMeasurementsDirty();
    }

    void Widget::removeChild(std::shared_ptr<Widget> w)
    {
        OD_CHECK_ARG_NONNULL(w);

        auto it = std::find(mChildWidgets.begin(), mChildWidgets.end(), w);
        if(it != mChildWidgets.end())
        {
            w->mParentWidget = nullptr;
            w->_moveToNoneRenderSpaceRecursive();
            mChildWidgets.erase(it);
        }

        // removing a widget from the tree does not invalidate the flattened hierarchy!
    }

    void Widget::addRenderHandle(std::shared_ptr<odRender::Handle> r)
    {
        OD_CHECK_ARG_NONNULL(r);

        if(mRenderGroup == nullptr)
        {
            // create without renderspace. will be moved to GUI space once encountered during flattening
            mRenderGroup = mGui.getRenderer().createGroup(odRender::RenderSpace::NONE);
            mRenderGroup->setVisible(mVisible);
        }

        mRenderGroup->addHandle(r);
    }

    void Widget::removeRenderHandle(std::shared_ptr<odRender::Handle> r)
    {
        OD_CHECK_ARG_NONNULL(r);

        if(mRenderGroup != nullptr)
        {
            mRenderGroup->removeHandle(r);
        }
    }

    void Widget::intersect(const glm::vec2 &point, std::vector<HitWidgetInfo> &hitWidgets)
    {
        glm::mat4 m(1.0);
        _intersectRecursive(point, m, hitWidgets);
    }

    void Widget::setVisible(bool b)
    {
        mVisible = b;

        if(mRenderGroup != nullptr)
        {
            mRenderGroup->setVisible(mVisible);
        }

        for(auto &child : mChildWidgets)
        {
            child->setVisible(b);
        }
    }

    void Widget::setZIndex(int32_t zIndex)
    {
        mZIndex = zIndex;
        mGui.markDepthDirty();
    }

    void Widget::setNeedsUpdate(bool needsUpdate)
    {
        if(needsUpdate == mNeedsUpdate)
        {
            // important! if this would not change anything, we must also not change the parent's need-update count
            return;
        }

        mNeedsUpdate = needsUpdate;

        Widget *parent = mParentWidget;
        while(parent != nullptr)
        {
            if(needsUpdate)
            {
                parent->mChildrenInNeedOfUpdate++;

            }else
            {
                if(parent->mChildrenInNeedOfUpdate <= 0)
                {
                    throw od::Exception("Invalid child-update-needed count decrement past zero. This widget tree seems broken");
                }

                parent->mChildrenInNeedOfUpdate--;
            }

            parent = parent->mParentWidget;
        }
    }

    void Widget::update(float relTime)
    {
        if(mNeedsUpdate)
        {
            this->onUpdate(relTime);
        }

        if(mChildrenInNeedOfUpdate > 0)
        {
            for(auto &child : mChildWidgets)
            {
                child->update(relTime);
            }
        }
    }

    void Widget::measure(glm::vec2 parentDimensionsPx)
    {
        switch(mDimensionType)
        {
        case WidgetDimensionType::ParentRelative:
            mMeasuredDimensionsPx = parentDimensionsPx * mDimensions;
            break;

        case WidgetDimensionType::Pixels:
            mMeasuredDimensionsPx = mDimensions;
            break;
        }

        for(auto &child : mChildWidgets)
        {
            child->measure(mMeasuredDimensionsPx);
        }
    }

    void Widget::flattenTransform()
    {
        glm::mat4 m(1.0);
        _flattenTransformRecursive(m);
    }

    void Widget::flattenDepth()
    {
        size_t nextGlobalRenderOrderIndex = 0;
        _flattenDepthRecursive(nextGlobalRenderOrderIndex);
    }

    void Widget::_moveToNoneRenderSpaceRecursive()
    {
        mGui.getRenderer().moveToRenderSpace(mRenderGroup, odRender::RenderSpace::NONE);

        for(auto &child : mChildWidgets)
        {
            child->_moveToNoneRenderSpaceRecursive();
        }
    }

    void Widget::_recalculateMatrix()
    {
        if(mParentWidget == nullptr)
        {
            mParentSpaceToMySpace = glm::mat4(1.0);
            mMySpaceToParentSpace = glm::mat4(1.0);

        }else
        {
            glm::vec2 mySizeInParentSpace;
            switch(mDimensionType)
            {
            case WidgetDimensionType::ParentRelative:
                mySizeInParentSpace = mDimensions;
                break;

            case WidgetDimensionType::Pixels:
                mySizeInParentSpace = getMeasuredDimensions() / mParentWidget->getMeasuredDimensions();
                break;
            }

            mParentSpaceToMySpace = glm::mat4(1.0);
            auto offsetOfCoordinateOrigin = mPositionInParentSpace - _getOriginVector()*mySizeInParentSpace;
            mParentSpaceToMySpace = glm::scale(mParentSpaceToMySpace, {1.0/mySizeInParentSpace.x, 1.0/mySizeInParentSpace.y, 1.0});
            mParentSpaceToMySpace = glm::translate(mParentSpaceToMySpace, {-offsetOfCoordinateOrigin, 0.0});

            mMySpaceToParentSpace = glm::inverse(mParentSpaceToMySpace);
        }

        mMatrixDirty = false;
    }

    void Widget::_flattenTransformRecursive(const glm::mat4 &parentToRoot)
    {
        if(mMatrixDirty)
        {
            _recalculateMatrix();
        }

        glm::mat4 thisToRoot = parentToRoot * mMySpaceToParentSpace;
        if(mRenderGroup != nullptr)
        {
            mRenderGroup->setMatrix(thisToRoot);
        }

        for(auto &child : mChildWidgets)
        {
            child->_flattenTransformRecursive(thisToRoot);
        }
    }

    void Widget::_flattenDepthRecursive(size_t &nextGlobalRenderOrderIndex)
    {
        // if this gets called, we are connected to the root widget and thus
        //  can move our render group (if any) to the GUI space
        if(mRenderGroup != nullptr)
        {
            mGui.getRenderer().moveToRenderSpace(mRenderGroup, odRender::RenderSpace::GUI);
        }

        // algorithm for flattening the GUI depth:
        // walk through widget tree in pre-order:
        //      sort render handles by z-index
        //      for each handle:
        //          if prev_handle.z_index != handle.z_index: ++z
        //          handle.z_coord = z
        //      sort child widgets by z-index
        //      for each child:
        //          if last child's z-index != current child's z-index: ++z
        //          descend into child

        auto childPred = [](auto &left, auto &right) { return left->getZIndex() < right->getZIndex(); };
        std::sort(mChildWidgets.begin(), mChildWidgets.end(), childPred);

        for(size_t i = 0; i < mChildWidgets.size(); ++i)
        {
            if(i > 0 && mChildWidgets[i]->getZIndex() != mChildWidgets[i-1]->getZIndex())
            {
                ++nextGlobalRenderOrderIndex;
            }

            mChildWidgets[i]->_flattenDepthRecursive(nextGlobalRenderOrderIndex);
        }
    }

    void Widget::_intersectRecursive(const glm::vec2 &point, const glm::mat4 &rootToParent, std::vector<HitWidgetInfo> &hitWidgets)
    {
        glm::mat4 rootToThis = mParentSpaceToMySpace * rootToParent;

        glm::vec4 pointInWidget = rootToThis * glm::vec4(point, 0.0, 1.0);

        if(this->liesWithinLogicalArea(glm::vec2(pointInWidget)))
        {
            HitWidgetInfo info;
            info.hitPointInWidget.x = pointInWidget.x;
            info.hitPointInWidget.y = pointInWidget.y;
            info.widget = this;
            hitWidgets.push_back(info);

            for(auto &child : mChildWidgets)
            {
                child->_intersectRecursive(point, rootToThis, hitWidgets);
            }
        }
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
