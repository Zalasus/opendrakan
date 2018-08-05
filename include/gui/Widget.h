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
#include <osg/BoundingBox>

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

    enum class WidgetZPolicy
    {
        Normal,
        AlwaysOnTop,
        AlwaysOnBottom
    };

    /**
     * Base class for UI elements. Widgets can group together other Widgets (see Container)
     * and/or define their own geometry.
     *
     * Widgets have their own coordinate system, called "widget space" here.
     * Widget space is similar to the window space as used by many windowing systems, where 0/0 is the widget's
     * upper left and 1/1 it's lower right corner).
     *
     * Widgets have a XY position, which is always given in it's parent's widget space. If a widget has no parent, it's
     * position is specified in a simulated widget space, where 0/0 is the screen's top left and 1/1 it's bottom right corner.
     * The position of a widget is relative to it's origin. The default origin is the widget's top left corner.
     *
     * Widgets also have a Z position, which is used to control stacking of widgets. A widget with a lower Z coordinate is
     * always on top of other widgets with lower Z coordinates.
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
        virtual ~Widget();

        inline GuiManager &getGuiManager() { return mGuiManager; }
        inline int32_t getZIndex() const { return mZIndexInParentSpace; }
        inline WidgetDimensionType getDimensionType() const { return mDimensionType; }
        inline osg::Vec2 getDimensions() const { return mDimensions; }
        inline bool isMatrixDirty() const { return mMatrixDirty; }
        inline void setDimensions(const osg::Vec2 &dim) { mDimensions = dim; mMatrixDirty = true; }
        inline void setDimensions(const osg::Vec2 &dim, WidgetDimensionType type) { mDimensions = dim; mDimensionType = type; mMatrixDirty = true; }
        inline void setDimensions(float x, float y) { setDimensions(osg::Vec2(x, y)); }
        inline void setDimensions(float x, float y, WidgetDimensionType type) { setDimensions(osg::Vec2(x, y), type); }
        inline void setDimensionType(WidgetDimensionType type) { mDimensionType = type; mMatrixDirty = true; }
        inline void setOrigin(WidgetOrigin origin) { mOrigin = origin; mMatrixDirty = true; }
        inline void setPosition(const osg::Vec2 &pos) { mPositionInParentSpace = pos; mMatrixDirty = true; }
        inline void setZIndex(int32_t i) { mZIndexInParentSpace = i; mMatrixDirty = true; }
        inline void setParent(Widget *p) { mParentWidget = p; mMatrixDirty = true; }

        virtual std::pair<int32_t, int32_t> getZRange() const = 0;

        osg::Vec2 getDimensionsInPixels();
        osg::Vec2 getParentDimensionsInPixels();
        std::pair<int32_t, int32_t> getParentZRange();

        void setVisible(bool b);

        void updateMatrix();


    private:

        GuiManager &mGuiManager;
        WidgetOrigin mOrigin;
        WidgetDimensionType mDimensionType;
        osg::Vec2 mDimensions;
        osg::Vec2 mPositionInParentSpace;
        int32_t mZIndexInParentSpace;
        Widget *mParentWidget;
        bool mMatrixDirty;
        osg::ref_ptr<osg::NodeCallback> mUpdateCallback;
    };

}

#endif /* INCLUDE_GUI_WIDGET_H_ */
