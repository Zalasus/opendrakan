/*
 * DrawableWidget.h
 *
 *  Created on: 9 Jul 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_DRAWABLEWIDGET_H_
#define INCLUDE_GUI_DRAWABLEWIDGET_H_

#include "gui/Widget.h"

namespace od
{

    class GuiManager;

    class DrawableWidget : public Widget
    {
    public:

        DrawableWidget(GuiManager &gm);

        virtual std::pair<int32_t, int32_t> getZRange() const override;


    protected:

        void addDrawable(osg::Drawable *drawable);


    private:

        osg::ref_ptr<osg::Geode> mGeode;

    };

}

#endif /* INCLUDE_GUI_DRAWABLEWIDGET_H_ */
