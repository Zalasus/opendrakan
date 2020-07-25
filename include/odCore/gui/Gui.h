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

namespace odRender
{
    class Renderer;
}

namespace odGui
{

    class Widget;

    class Gui
    {
    public:

        Gui(odRender::Renderer &renderer);
        virtual ~Gui();

        inline bool isMenuMode() const { return mMenuMode; }
        inline odRender::Renderer &getRenderer() { return mRenderer; }
        inline const glm::mat4 &getWidgetSpaceToNdcTransform() const { return mWidgetSpaceToNdcTransform; }
        inline const glm::mat4 &getNdcToWidgetSpaceTransform() const { return mNdcToWidgetSpaceTransform; }

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


    private:

        void _setupGui();

        odRender::Renderer &mRenderer;

        bool mMenuMode;

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
