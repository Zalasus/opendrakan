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

        inline float getZPosition() const { return mZPositionInParentSpace; }
        inline float getDepth() const { return mDepthInParentSpace; }
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
         * @brief Sets the depth of this Widget relative to it's parent.
         *
         * A depth of 1 means this is exactly as deep as it's parent. This is the default.
         *
         * You can ignore this for 2D-only widgets. Do not set it to 0, because that will mess up the transforms.
         */
        void setDepth(float d);

        /**
         * @brief Sets this widget's Z position inside it's parent.
         *
         * This is 0 by default.
         *
         * The Widget Space's Z axis points *into* the screen.
         */
        void setZPosition(float z);

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
         * @brief Sets this Widget's render mode to perspective.
         */
        void setPerspectiveMode(float aspect, float fov);

        /**
         * @brief Sets this Widget's render mode to orthogonal projection.
         *
         * This is the default for new widgets.
         */
        void setOrthogonalMode();

        /**
         * @brief Set to true if your subclass needs periodic calling of the onUpdate() hook.
         */
        void setEnableUpdate(bool b);

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
        void flatten();


    protected:

        inline Gui &getGui() { return mGui; }


    private:

        void _moveToNoneRenderSpaceRecursive();
        void _recalculateMatrix();
        void _flattenRecursive(const glm::mat4 &parentToRoot);
        void _intersectRecursive(const glm::vec2 &pointInParent, std::vector<HitWidgetInfo> &hitWidgets);
        glm::vec2 _getOriginVector();

        Gui &mGui;
        WidgetOrigin mOrigin;
        WidgetDimensionType mDimensionType;
        glm::vec2 mDimensions;
        glm::vec2 mPositionInParentSpace;
        float mDepthInParentSpace;
        float mZPositionInParentSpace;
        bool mVisible;
        Widget *mParentWidget; // yes, the bare pointer is intentional. this is supposed to be a weak ref. see explanation in doc of setParent()

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
