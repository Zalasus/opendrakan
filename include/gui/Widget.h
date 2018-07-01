/*
 * Widget.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_WIDGET_H_
#define INCLUDE_GUI_WIDGET_H_

#include <osg/Vec2>
#include <osg/MatrixTransform>
#include <osg/Geode>

namespace od
{

    class GuiManager;

    enum class WidgetOrigin
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center
    };

    enum class WidgetDimensionType
    {
        Pixels,
        ParentRelative
    };

    /**
     * Base class for UI elements. Widgets can group together other Widgets (see Container)
     * and/or define their own geometry.
     *
     * Widgets have their own coordinate system, called "widget space" here.
     * Widget space is similar to the window space as used by many windowing systems, where 0/0 is the widget's
     * upper left and 1/1 it's lower right corner).
     *
     * Widgets have a position, which is always given in it's parent's widget space. If a widget has no parent, it's
     * position is specified in a simulated widget space, where 0/0 is the screen's top left and 1/1 it's bottom right corner.
     * The position of a widget is relative to it's origin. The default origin is the widget's top left corner.
     *
     * Widgets must be able to report their dimensions by implementing getDimensions(). These can be either
     * given in absolute pixels or relative to the widget's parent's size. The type of the dimensions reported by
     * getDimensions() must be returned by getDimensionType().
     *
     * In case pixel dimensions are returned, the widget's pixels will be mapped to the screen pixels 1:1.
     * In parent-relative dimensions, a dimension of (1/1) is exactly the parent's size, (0.5/0.5) is half the parent's size, etc.
     *
     * It is up to the widget implementation to apply an optional scale factor to these dimensions if UI scaling is desired.
     */
    class Widget : public osg::MatrixTransform
    {
    public:

        Widget(GuiManager &gm);
        virtual ~Widget() = default;

        inline GuiManager &getGuiManager() { return mGuiManager; }

        virtual WidgetDimensionType getDimensionType() const = 0;
        virtual osg::Vec2 getDimensions() const = 0;

        osg::Vec2 getDimensionsInPixels();
        osg::Vec2 getParentDimensionsInPixels();

        void setVisible(bool b);

        void setOrigin(WidgetOrigin origin);

        /// @brief Sets position of widget origin in parent widget space
        void setPosition(const osg::Vec2 &pos);

        void setParent(Widget *parent);


    protected:

        void addDrawable(osg::Drawable *drawable);


    private:

        void _updateMatrix();

        GuiManager &mGuiManager;
        WidgetOrigin mOrigin;
        osg::Vec2 mPositionInParentSpace;
        osg::ref_ptr<osg::Geode> mGeode;

        Widget *mParentWidget;
    };

}

#endif /* INCLUDE_GUI_WIDGET_H_ */
