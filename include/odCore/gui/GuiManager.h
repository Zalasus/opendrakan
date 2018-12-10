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

        GuiManager(od::Engine &engine);

        inline od::Engine &getEngine() { return mEngine; }
        inline bool isMenuMode() const { return mMenuMode; }

        void quit();

        glm::vec2 getScreenResolution();

        void addWidget(Widget *widget);
        void removeWidget(Widget *widget);
        size_t getWidgetCount();
        std::pair<int32_t, int32_t> getWidgetZRange();

        void setMenuMode(bool b);

        void mouseDown();

        /**
         * @brief Sets the widget to be used as a cursor.
         *
         * This is purely visual. The mouse picking functionality works without an assigned cursor widget.
         */
        void setCursorWidget(Widget *cursor);

        /**
         * @brief Positions cursor in screen space.
         */
        void setCursorPosition(const glm::vec2 &pos);


    private:

        inline void _decryptString(char * const str, const size_t len);
        void _setupGui();

        od::Engine &mEngine;
        bool mMenuMode;

        std::vector<od::RefPtr<Widget>> mRootWidgets;
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
