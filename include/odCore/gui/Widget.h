/*
 * Widget.h
 *
 *  Created on: 22 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_WIDGET_H_
#define INCLUDE_GUI_WIDGET_H_

#include <vector>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/gui/WidgetIntersector.h>

namespace odRender
{
    class Group;
    class Handle;
}

namespace odGui
{

    class Gui;

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
     * Base class for UI elements. Widgets can group together other Widgets and/or define their own geometry.
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
    class Widget
    {
    public:

        Widget(Gui &gui);
        virtual ~Widget();

        inline int32_t getZIndex() const { return mZIndex; }
        inline glm::vec2 getPosition() const { return mPositionInParentSpace; }
        inline WidgetDimensionType getDimensionType() const { return mDimensionType; }
        inline glm::vec2 getDimensions() const { return mDimensions; }
        inline bool isMouseOver() const { return mMouseOver; }
        inline void setMouseOver(bool b) { mMouseOver = b; }

        void setPosition(const glm::vec2 &pos);
        void setDimensions(const glm::vec2 &dim);
        void setDimensions(const glm::vec2 &dim, WidgetDimensionType type);
        void setDimensionType(WidgetDimensionType type);
        void setOrigin(WidgetOrigin origin);


        /**
         * @brief Returns true if \c pos lies within the logical widget bounds.
         *
         * This gets called to check whether a UI event (mouse click etc.) happened within the widget.
         * It can be used to limit the space inside which a widget responds to events. This is useful for
         * round buttons, widgets with orders etc.
         *
         * The default behaviour is to return true for all points within [0.0 0.0] and [1.0 1.0].
         *
         * Children of a widget will only be considered for events if this method returns true for the
         * event coordiantes.
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
         *
         * @param relTime   Relative time since last update, in seconds
         *
         * @note Renderers should call update() instead of calling this hook directly.
         */
        virtual void onUpdate(float relTime);

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

        void addChild(std::shared_ptr<Widget> w);
        void removeChild(std::shared_ptr<Widget> w);

        /**
         * @brief Adds a render handle to be drawn in this widget's render space.
         *
         * Handles that are added first are also drawn first, so appear below those that are added later.
         */
        void addRenderHandle(std::shared_ptr<odRender::Handle> r);
        void removeRenderHandle(std::shared_ptr<odRender::Handle> r);

        /**
         * @brief Performs recursive intersection check, collecting all found widgets in a vector.
         * @param pointInWidgetSpace  The point to check, using *this* Widget's space
         */
        void intersect(const glm::vec2 &point, std::vector<HitWidgetInfo> &hitWidgets);

        /**
         * @brief Returns the measured dimensions of this widget in pixels.
         *
         * This is only valid after the measure-pass of the GUI rebuild.
         */
        inline glm::vec2 getMeasuredDimensions() const { return mMeasuredDimensionsPx; }

        void setVisible(bool b);

        /**
         * This affects stacking inside the parent widget. A widget with a lower Z index is always on top of other widgets
         * with higher Z indices.
         *
         * A widget's render handles have an implicit Z index which is determined by the order in which they are added.
         * Handles that are added first are also drawn first, so appear below those that are added later.
         */
        void setZIndex(int32_t zIndex);

        void setNeedsUpdate(bool b);

        /**
         * @brief Periodic update. Calls the onUpdate() hook, then propagates to all children that explicitly request the periodic update.
         */
        void update(float relTime);

        /**
         * @brief Pushes measurements down the widget hierarchy recursively.
         *
         * This terminology is borrowed from Android, and except for the fact
         * that our system is far less complex, essentially works the same way.
         */
        void measure(glm::vec2 parentDimensionsPx);

        /**
         * @brief Updates the widget transforms if necessary and flattens this and it's children.
         *
         * It really only makes sense to call this on the root widget. Partially flattening the widget tree does not
         * yield correct results as of now.
         */
        void flattenTransform();

        /**
         * @brief Walks the GUI tree and turns the hierachical widget stack into a linear draw order for the renderer.
         *
         * It really only makes sense to call this on the root widget. Partially flattening the widget tree does not
         * yield correct results as of now.
         */
        void flattenDepth();


    protected:

        inline Gui &getGui() { return mGui; }


    private:

        void _moveToNoneRenderSpaceRecursive();
        void _recalculateMatrix();
        void _flattenDepthRecursive(size_t &nextGlobalRenderOrderIndex);
        void _flattenTransformRecursive(const glm::mat4 &parentToRoot);
        void _intersectRecursive(const glm::vec2 &point, const glm::mat4 &rootToParent, std::vector<HitWidgetInfo> &hitWidgets);
        glm::vec2 _getOriginVector();

        /**
         * @brief Propagates the transform-dirty state up the widget hierarchy.
         */
        void _dirtyTransform();

        Gui &mGui;
        WidgetOrigin mOrigin;
        WidgetDimensionType mDimensionType;
        glm::vec2 mDimensions;
        glm::vec2 mPositionInParentSpace;
        int32_t mZIndex;
        bool mVisible;
        Widget *mParentWidget; // yes, the bare pointer is intentional. this is supposed to be a weak ref. see explanation in doc of setParent()

        // number of child widgets (of any depth) that requested an periodic update. if this is 0, the update() method does not have to propagate down
        size_t mChildrenInNeedOfUpdate;
        bool mNeedsUpdate;

        glm::vec2 mMeasuredDimensionsPx;

        bool mMatrixDirty;
        glm::mat4 mParentSpaceToMySpace;
        glm::mat4 mMySpaceToParentSpace;

        bool mMouseOver;

        std::vector<std::shared_ptr<Widget>> mChildWidgets;

        std::shared_ptr<odRender::Group> mRenderGroup;
    };

}

#endif /* INCLUDE_GUI_WIDGET_H_ */
