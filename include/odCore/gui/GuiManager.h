/*
 * GuiManager.h
 *
 *  Created on: 8 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_GUIMANAGER_H_
#define INCLUDE_GUI_GUIMANAGER_H_

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <odCore/RefCounted.h>

#include <odCore/gui/WidgetIntersector.h>

namespace od
{
    class Engine;
}

namespace odGui
{

    class Widget;

    /**
     * Class for managing the game's GUI.
     */
    class GuiManager
    {
    public:

        GuiManager();
        ~GuiManager();

        inline bool isMenuMode() const { return mMenuMode; }

        void quit();

        glm::vec2 getScreenResolution();

        void addWidget(Widget *widget);
        void removeWidget(Widget *widget);

        void setMenuMode(bool b);

        void mouseDown();

        /**
         * @brief Sets the widget to be used as a cursor.
         *
         * This is purely visual. The mouse picking functionality works without an assigned cursor widget.
         */
        void setCursorWidget(Widget *cursor);

        /**
         * @brief Positions cursor in NDC.
         */
        void setCursorPosition(const glm::vec2 &pos);


    private:

        void _setupGui();

        bool mMenuMode;

        od::RefPtr<Widget> mRootWidget;
        glm::vec2 mCursorPosInNdc;
        od::RefPtr<Widget> mCursorWidget;

        glm::mat4 mWidgetSpaceToNdcTransform;
        glm::mat4 mNdcToWidgetSpaceTransform;

        std::vector<HitWidgetInfo> mCurrentHitWidgets;
        std::vector<HitWidgetInfo> mLastHitWidgets;
        std::vector<HitWidgetInfo> mJoinedHitWidgets; // vector for combining last and current hits
    };

}

#endif /* INCLUDE_GUI_GUIMANAGER_H_ */
