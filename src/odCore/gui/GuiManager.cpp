/*
 * GuiManager.cpp
 *
 *  Created on: 8 Jun 2018
 *      Author: zal
 */

#include <odCore/gui/GuiManager.h>

#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <odCore/Logger.h>
#include <odCore/SrscRecordTypes.h>
#include <odCore/Engine.h>

#include <odCore/gui/Widget.h>

namespace odGui
{

    GuiManager::GuiManager(od::Engine &engine)
    : mEngine(engine)
    , mMenuMode(false)
    {
        _setupGui();
    }

    void GuiManager::quit()
    {
    }

    void GuiManager::mouseDown()
    {
        mCurrentHitWidgets.clear();
        for(auto it = mRootWidgets.begin(); it != mRootWidgets.end(); ++it)
        {
            (*it)->intersect(mCursorPosInNdc, mNdcToWidgetSpaceTransform, mWidgetSpaceToNdcTransform, mCurrentHitWidgets);
        }

        Logger::debug() << "Hit " << mCurrentHitWidgets.size() << " widgets!";

        for(auto it = mCurrentHitWidgets.begin(); it != mCurrentHitWidgets.end(); ++it)
        {
            it->widget->onMouseDown(it->hitPointInWidget, 0);
        }
    }

    glm::vec2 GuiManager::getScreenResolution()
    {
        return glm::vec2(1, 1);
    }

    void GuiManager::addWidget(Widget *widget)
    {
        if(widget == nullptr)
        {
            return;
        }

        mRootWidgets.push_back(od::RefPtr<Widget>(widget));

        widget->setParent(nullptr);
    }

    void GuiManager::removeWidget(Widget *widget)
    {
        if(widget == nullptr)
        {
            return;
        }

        auto it = std::find(mRootWidgets.begin(), mRootWidgets.end(), widget);
        if(it != mRootWidgets.end())
        {
            mRootWidgets.erase(it);
        }
    }

    size_t GuiManager::getWidgetCount()
    {
        return mRootWidgets.size();
    }

    std::pair<int32_t, int32_t> GuiManager::getWidgetZRange()
    {
        /*if(mWidgets.size() == 0)
        {
            return std::pair<int32_t, int32_t>(0, 0);
        }

        int32_t minZ = std::numeric_limits<int32_t>::max();
        int32_t maxZ = std::numeric_limits<int32_t>::min();
        for(auto it = mWidgets.begin(); it != mWidgets.end(); ++it)
        {
            int32_t z = (*it)->getZIndex();
            minZ = std::min(minZ, z);
            maxZ = std::max(maxZ, z);
        }

        return std::pair<int32_t, int32_t>(minZ, maxZ);*/

        return std::pair<int32_t, int32_t>(-1000, 1000);
    }

    void GuiManager::setMenuMode(bool b)
    {
        mMenuMode = b;

        if(mCursorWidget != nullptr)
        {
            mCursorWidget->setVisible(b);
        }
    }

    void GuiManager::setCursorWidget(Widget *cursor)
    {
        if(mCursorWidget != nullptr)
        {
            this->removeWidget(mCursorWidget);
        }

        mCursorWidget = cursor;
        mCursorWidget->setZIndex(-1000);
        mCursorWidget->setVisible(mMenuMode);
        this->addWidget(mCursorWidget);
    }

    void GuiManager::setCursorPosition(const glm::vec2 &pos)
    {
        if(mCursorPosInNdc == pos)
        {
            // no change -> no need to perform costly update
            return;
        }

        mCursorPosInNdc = pos;

        // pos is in NDC, we need it in widget space
        glm::vec2 posWs = glm::vec4(pos, 0.0, 1.0) * mNdcToWidgetSpaceTransform;

        if(mCursorWidget != nullptr)
        {
            mCursorWidget->setPosition(glm::vec2(posWs));
        }

        // okay, here is the algorithm for determining mouse enter/leave:
        //  whenever the mouse moves, we find all widgets under it. we also store all widgets found under the
        //  cursor the last time the mouse moved. now, as the mouse moved, we join the two lists. all widgets
        //  that are unique in that list were either entered or left by the cursor. which one of those events
        //  happened can be determined from the mouse-over state that is stored in those widgets.
        //  FIXME: this only works if every widget in the scenegraph is unique
        mCurrentHitWidgets.clear();
        for(auto it = mRootWidgets.begin(); it != mRootWidgets.end(); ++it)
        {
            (*it)->intersect(mCursorPosInNdc, mNdcToWidgetSpaceTransform, mWidgetSpaceToNdcTransform, mCurrentHitWidgets);
        }

        mJoinedHitWidgets.clear();
        mJoinedHitWidgets.insert(mJoinedHitWidgets.end(), mCurrentHitWidgets.begin(), mCurrentHitWidgets.end());
        mJoinedHitWidgets.insert(mJoinedHitWidgets.end(), mLastHitWidgets.begin(), mLastHitWidgets.end());

        // find unique widgets in joined list. NOTE: since the apparently is no nice way to do this via STL,
        //  we do it manually using a sort and by iterating over each widget.
        auto pred = [](HitWidgetInfo &a, HitWidgetInfo &b){ return a.widget < b.widget; };
        std::sort(mJoinedHitWidgets.begin(), mJoinedHitWidgets.end(), pred);
        for(auto it = mJoinedHitWidgets.begin(); it != mJoinedHitWidgets.end(); ++it)
        {
            HitWidgetInfo &current = *it;

            // if we ever reach the last element, we know it is unique and need no further checks
            if(it+1 != mJoinedHitWidgets.end())
            {
                HitWidgetInfo &next = *(it+1);
                if(current.widget == next.widget)
                {
                    ++it;
                    continue;
                }
            }

            // widget seems unique -> mouse either left it or entered it
            if(current.widget->isMouseOver())
            {
                current.widget->setMouseOver(false);
                current.widget->onMouseLeave(current.hitPointInWidget);

            }else
            {
                current.widget->setMouseOver(true);
                current.widget->onMouseEnter(current.hitPointInWidget);
            }
        }

        std::swap(mCurrentHitWidgets, mLastHitWidgets);
    }

    void GuiManager::_setupGui()
    {
        glm::mat4 eye(1.0);

        mWidgetSpaceToNdcTransform = glm::scale(eye, glm::vec3(2.0, -2.0, -1.0));
        mWidgetSpaceToNdcTransform = glm::translate(mWidgetSpaceToNdcTransform, glm::vec3(-1.0, 1.0, -0.5));

        mNdcToWidgetSpaceTransform = glm::inverse(mWidgetSpaceToNdcTransform);
    }

}
