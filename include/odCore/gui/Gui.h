/*
 * Gui.h
 *
 *  Created on: 21 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_GUI_GUI_H_
#define INCLUDE_ODCORE_GUI_GUI_H_

#include <string>
#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <odCore/gui/WidgetIntersector.h>

#include <odCore/render/GuiCallback.h>

namespace odRender
{
    class Renderer;
}

namespace odGui
{

    class Widget;

    class Gui : public odRender::GuiCallback
    {
    public:

        Gui(odRender::Renderer &renderer);
        virtual ~Gui();

        inline bool isMenuMode() const { return mMenuMode; }
        inline odRender::Renderer &getRenderer() { return mRenderer; }
        inline const glm::mat4 &getWidgetSpaceToNdcTransform() const { return mWidgetSpaceToNdcTransform; }
        inline const glm::mat4 &getNdcToWidgetSpaceTransform() const { return mNdcToWidgetSpaceTransform; }
        inline void markDepthDirty() { mDepthDirty = true; }
        inline void markTransformsDirty() { mTransformsDirty = true; }
        inline void markMeasurementsDirty() { mMeasurementsDirty = true; }

        void quit();

        void addWidget(std::shared_ptr<Widget> widget);
        void removeWidget(std::shared_ptr<Widget> widget);

        void setMenuMode(bool b);

        void mouseDown();

        /**
         * @brief Sets the widget to be used as a cursor.
         *
         * This is purely visual. The mouse picking functionality works without an assigned cursor widget.
         */
        void setCursorWidget(std::shared_ptr<Widget> cursor);

        /**
         * @brief Positions cursor in NDC.
         */
        void setCursorPosition(const glm::vec2 &pos);

        /**
         * @brief Hook for child classes to react when menu mode is enabled/disabled. Does nothing by default.
         */
        virtual void onMenuModeChanged();

        /**
         * There are three steps to rebuilding the GUI:
         *
         * *Measure:* Walks the GUI tree top-down to figure out the absolute dimensions of
         * each widget. This becomes necessary everytime a widget or the screen change dimensions. Only
         * that subtree (i.e. the widget itself and all of it's direct or indirect children) has to be
         * re-measured, but this process is relatively inexpensive so we could consider doing this on
         * the whole tree everytime so we have one less dirty-state to manage.
         *
         * *Flatten Transform:* Walks the GUI tree top-down to figure out where in the screen-space to
         * place the widgets. The renderer has no notion of a hierarchical GUI, so it needs each render
         * handle to know exactly where it should be placed on the screen in NDC. This steps becomes
         * necessary whenever a widget or a drawable moves, but only on that widget's subtree. The widgets
         * in question must have been measured before this step can be executed successfully.
         *
         * *Flatten Depth:* Walks the GUI tree top-down to figure out a global sorting of render handles
         * that produces the widget stacking as determined by the individual Z-indices. Since Z-indices
         * are nested but the render handles are not, we have to tell the renderer in what order to draw
         * the handles so overlap is handled correctly. Recalculating this becomes necessary everytime a
         * widget's Z-index changes or a widget or drawable is added to the hierarchy. This has to happen
         * only on the widgets that come after the invalidated one in the global ordering, but determining
         * those widgets generally requires more bookkeeping and the worst case is still O(n), so it really
         * only makes sense to flatten the whole tree here. That's why this step is managed by a single
         * dirty-flag in the Gui object.
         *
         * Note that due to me being lazy, all of these dirty states are currently centralized in the Gui object.
         * If they are invalidated on any widget in the tree, the whole tree has to be re-flattened or measured.
         */
        void rebuild();

        virtual void onUpdate(float relTime) override;
        virtual void onFramebufferResize(glm::vec2 dimensionsPx) override;


    private:

        void _setupGui();

        odRender::Renderer &mRenderer;

        bool mMenuMode;

        bool mDepthDirty;
        bool mMeasurementsDirty;
        bool mTransformsDirty;

        std::shared_ptr<Widget> mRootWidget;
        glm::vec2 mCursorPosInNdc;
        std::shared_ptr<Widget> mCursorWidget;

        glm::mat4 mWidgetSpaceToNdcTransform;
        glm::mat4 mNdcToWidgetSpaceTransform;

        std::vector<HitWidgetInfo> mCurrentHitWidgets;
        std::vector<HitWidgetInfo> mLastHitWidgets;
        std::vector<HitWidgetInfo> mJoinedHitWidgets; // vector for combining last and current hits

    };

}

#endif /* INCLUDE_ODCORE_GUI_GUI_H_ */
