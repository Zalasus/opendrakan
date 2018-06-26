/*
 * Widget.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_WIDGET_H_
#define INCLUDE_GUI_WIDGET_H_

#include <osg/Vec2>
#include <osg/Group>
#include <osg/Geode>

namespace od
{

    /**
     * Base class for UI element that live inside a Window. Widgets can group together other
     * Widgets (see Container) and/or define their own geometry.
     *
     * Widgets have their own coordinate system, called "widget space" here.
     * Widget space is similar to the window space as used by many windowing systems, where 0/0 is the widget's
     * upper left and 1/1 it's lower right corner).
     * The widgets actual placement and scale on screen depends on the container/window the widget is contained in.
     *
     * There is no "screen-relative" scale. A widget must be able to report it's size in pixels
     * (via Widget::getFullScaleDimensions() ). The window/container encapsulating the widget can define
     * a scale by which this pixel size is scaled. The default scale is 1.0, meaning widget pixels will map 1:1 to screen pixels.
     */
    class Widget : public osg::Referenced
    {
    public:

        friend class Window;

        Widget();
        virtual ~Widget() = default;

        virtual osg::Vec2 getFullScaleDimensions() const = 0;

        void setVisible(bool b);


    protected:

        void addGeometry(osg::Geometry *g);


    private:

        osg::ref_ptr<osg::Group> mWidgetGroup;
        osg::ref_ptr<osg::Geode> mWidgetGeode;

    };

}

#endif /* INCLUDE_GUI_WIDGET_H_ */
