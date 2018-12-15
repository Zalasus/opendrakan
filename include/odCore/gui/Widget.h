/*
 * Widget.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_WIDGET_H_
#define INCLUDE_GUI_WIDGET_H_

#include <vector>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <odCore/RefCounted.h>

#include <odCore/gui/WidgetIntersector.h>

namespace odGui
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
     * Widgets also have a Z index, which is used to control stacking of widgets. A widget with a lower Z index is
     * always on top of other widgets with higher Z indices.
     *
     * Widgets have dimensions, which can be expressed either in pixels or relative to their parent.
     * When pixel dimensions are used, the widget's pixels will be mapped to the screen pixels 1:1. In parent-relative dimensions,
     * a dimension of (1/1) is exactly the parent's size, (0.5/0.5) is half the parent's size, etc.
     *
     * UI scaling is not yet implemented, but can be easily achieved by applying a scale factor to a widget's dimensions.
     */
    class Widget : public od::RefCounted
    {
    public:

        Widget();
        virtual ~Widget();

        inline int32_t getZIndex() const { return mZIndexInParentSpace; }
        inline glm::vec2 getPosition() const { return mPositionInParentSpace; }
        inline WidgetDimensionType getDimensionType() const { return mDimensionType; }
        inline glm::vec2 getDimensions() const { return mDimensions; }
        inline const glm::mat4 &getParentToWidgetSpaceMatrix() const { return mParentSpaceToWidgetSpace; }
        inline bool isMatrixDirty() const { return mMatrixDirty; }
        inline bool isMouseOver() const { return mMouseOver; }
        inline void setDimensions(const glm::vec2 &dim) { mDimensions = dim; mMatrixDirty = true; }
        inline void setDimensions(const glm::vec2 &dim, WidgetDimensionType type) { mDimensions = dim; mDimensionType = type; mMatrixDirty = true; }
        inline void setDimensions(float x, float y) { setDimensions(glm::vec2(x, y)); }
        inline void setDimensions(float x, float y, WidgetDimensionType type) { setDimensions(glm::vec2(x, y), type); }
        inline void setDimensionType(WidgetDimensionType type) { mDimensionType = type; mMatrixDirty = true; }
        inline void setOrigin(WidgetOrigin origin) { mOrigin = origin; mMatrixDirty = true; }
        inline void setPosition(const glm::vec2 &pos) { mPositionInParentSpace = pos; mMatrixDirty = true; }
        inline void setPosition(float x, float y) { setPosition(glm::vec2(x, y)); }
        inline void setZIndex(int32_t i) { mZIndexInParentSpace = i; mMatrixDirty = true; }
        inline void setParent(Widget *p) { mParentWidget = p; mMatrixDirty = true; }
        inline void setMouseOver(bool b) { mMouseOver = b; }

        /**
         * @brief Returns true if \c pos lies within the logical widget bounds.
         *
         * This gets called when a UI event (mouse click etc.) happens within the rectangular screen space occupied by the widget.
         * It can be used to limit the space inside which a widget responds to events. This is useful for round buttons, widgets with
         * borders etc. The default behaviour is to return true for all points within [0.0 0.0] and [1.0 1.0].
         *
         * @param  pos[in]   Point in this widget's coordinate space to be checked.
         */
        virtual bool liesWithinLogicalArea(const glm::vec2 &pos);

        /**
         * @brief Hook for when a mouse click event happens inside of logical widget area.
         * @param pos  Position in widget space where mouse was clicked
         */
        virtual void onMouseDown(const glm::vec2 &pos, int button);

        /**
         * @brief Update hook for widget. Gets called every frame if widget is active.
         * @param simTime   Absolute time, in seconds
         * @param relTime   Relative time since last update, in seconds
         */
        virtual void onUpdate(double simTime, double relTime);

        /**
         * @brief Hook for when a the mouse cursor enters the logical widget area.
         * @note Whether the mouse is over a widget or not can be checked via Widget::isMouseOver(), too.
         * @param pos  Position in widget space where mouse entered
         */
        virtual void onMouseEnter(const glm::vec2 &pos);

        /**
         * @brief Hook for when a the mouse cursor leaves the logical widget area.
         * @note Whether the mouse is over a widget or not can be checked via Widget::isMouseOver(), too.
         * @param pos  The last position in widget space where mouse was still over widget
         */
        virtual void onMouseLeave(const glm::vec2 &pos);

        void intersect(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, const glm::mat4 &parentInverseMatrix, std::vector<HitWidgetInfo> &hitWidgets);

        glm::vec2 getDimensionsInPixels();

        void setVisible(bool b);

        void updateMatrix();

        virtual void flattenDrawables(const glm::mat4 &parentMatrix);


    protected:

        virtual void intersectChildren(const glm::vec2 &pointNdc, const glm::mat4 &parentMatrix, const glm::mat4 &parentInverseMatrix, std::vector<HitWidgetInfo> &hitWidgets);


    private:

        glm::vec2 _getOriginVector();

        WidgetOrigin mOrigin;
        WidgetDimensionType mDimensionType;
        glm::vec2 mDimensions;
        glm::vec2 mPositionInParentSpace;
        int32_t mZIndexInParentSpace;
        Widget *mParentWidget;
        bool mMatrixDirty;
        glm::mat4 mParentSpaceToWidgetSpace;
        glm::mat4 mWidgetSpaceToParentSpace;
        bool mMouseOver;
    };

}

#endif /* INCLUDE_GUI_WIDGET_H_ */
