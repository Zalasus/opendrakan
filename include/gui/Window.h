/*
 * Window.h
 *
 *  Created on: 24 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_WINDOW_H_
#define INCLUDE_GUI_WINDOW_H_

#include "gui/Widget.h"

#include <osg/MatrixTransform>

namespace od
{

    class GuiManager;

    enum class WindowOrigin
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center
    };

    /**
     * Top level container for a single widget. Is positioned in screen space and defines a widget space
     * for it's child widget to live in. It's size is the full scale resolution as reported by the child
     * widget multiplied by the window scale.
     */
    class Window : public osg::MatrixTransform
    {
    public:

        Window(GuiManager &guiManager);

        void setScale(const osg::Vec2 &scale);
        inline void setScale(float scale) { setScale(osg::Vec2(scale, scale)); }

        void setOrigin(WindowOrigin origin);

        /// @brief Sets position of window origin on screen (in screen space coordinates).
        void setPosition(const osg::Vec2 &pos);

        void setChildWidget(Widget *w);


    private:

        void _updateMatrix();

        GuiManager &mGuiManager;
        WindowOrigin mOrigin;
        osg::Vec2 mPositionInScreenSpace;
        osg::Vec2 mScale;
        osg::ref_ptr<Widget> mChildWidget;
    };

}

#endif /* INCLUDE_GUI_WINDOW_H_ */
