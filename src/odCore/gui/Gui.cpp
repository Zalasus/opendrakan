/*
 * Gui.cpp
 *
 *  Created on: 21 Dec 2018
 *      Author: zal
 */

#include <odCore/gui/Gui.h>

#include <algorithm>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <odCore/Logger.h>
#include <odCore/SrscRecordTypes.h>

#include <odCore/render/Renderer.h>

#include <odCore/gui/Widget.h>

namespace odGui
{

    Gui::Gui(odRender::Renderer &renderer)
    : mRenderer(renderer)
    , mMenuMode(false)
    , mDepthDirty(false)
    {
        _setupGui();

        mRenderer.addGuiCallback(this);
    }

    Gui::~Gui()
    {
        mRenderer.removeGuiCallback(this);
    }

    void Gui::quit()
    {
    }

    void Gui::mouseDown()
    {
        if(!mMenuMode)
        {
            return;
        }

        mCurrentHitWidgets.clear();
        mRootWidget->intersect(mCursorPos, mCurrentHitWidgets);

        Logger::debug() << "Hit " << mCurrentHitWidgets.size() << " widgets!";

        for(auto it = mCurrentHitWidgets.begin(); it != mCurrentHitWidgets.end(); ++it)
        {
            it->widget->onMouseDown(it->hitPointInWidget, 0);
        }
    }

    void Gui::addWidget(std::shared_ptr<Widget> widget)
    {
        if(widget != nullptr)
        {
            mRootWidget->addChild(widget);
        }
    }

    void Gui::removeWidget(std::shared_ptr<Widget> widget)
    {
        if(widget != nullptr)
        {
            mRootWidget->removeChild(widget);
        }
    }

    void Gui::setMenuMode(bool b)
    {
        mMenuMode = b;

        if(mCursorWidget != nullptr)
        {
            mCursorWidget->setVisible(b);
        }

        this->onMenuModeChanged();
    }

    void Gui::setCursorWidget(std::shared_ptr<Widget> cursor)
    {
        if(mCursorWidget != nullptr)
        {
            this->removeWidget(mCursorWidget);
        }

        mCursorWidget = cursor;

        if(mCursorWidget != nullptr)
        {
            mCursorWidget->setZIndex(-1000);
            mCursorWidget->setVisible(mMenuMode);
            this->addWidget(mCursorWidget);

            mCursorPos = mCursorWidget->getPosition();
        }
    }

    void Gui::setCursorPosition(const glm::vec2 &pos)
    {
        if(mCursorPos == pos)
        {
            // no change -> no need to perform costly update
            return;
        }

        mCursorPos = pos;

        if(mCursorWidget != nullptr)
        {
            mCursorWidget->setPosition({mCursorPos.x, mCursorPos.y});
        }

        if(!mMenuMode)
        {
            return;
        }

        // okay, here is the algorithm for determining mouse enter/leave:
        //  whenever the mouse moves, we find all widgets under it. we also store all widgets found under the
        //  cursor the last time the mouse moved. now, as the mouse moved, we join the two lists. all widgets
        //  that are unique in that list were either entered or left by the cursor. which one of those events
        //  happened can be determined from the mouse-over state that is stored in those widgets.
        //  FIXME: this only works if every widget in the scenegraph is unique
        mCurrentHitWidgets.clear();
        mRootWidget->intersect(mCursorPos, mCurrentHitWidgets);

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

    void Gui::onMenuModeChanged()
    {
    }

    void Gui::rebuild()
    {
        if(mDepthDirty)
        {
            mRootWidget->flattenDepth();
            mDepthDirty = false;
        }

        if(mMeasurementsDirty)
        {
            mRootWidget->measure(mRenderer.getFramebufferDimensions());
            mMeasurementsDirty = false;
        }

        if(mTransformsDirty)
        {
            mRootWidget->flattenTransform();
            mTransformsDirty = false;
        }
    }

    void Gui::onUpdate(float relTime)
    {
        rebuild();

        mRootWidget->update(relTime);
    }

    void Gui::onFramebufferResize(glm::vec2 dimensionsPx)
    {
        mRootWidget->measure(dimensionsPx);
    }

    void Gui::_setupGui()
    {
        mRootWidget = std::make_shared<Widget>(*this);
    }

}
