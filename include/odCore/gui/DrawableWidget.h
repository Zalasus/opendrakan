/*
 * DrawableWidget.h
 *
 *  Created on: 14 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_GUI_DRAWABLEWIDGET_H_
#define INCLUDE_ODCORE_GUI_DRAWABLEWIDGET_H_

#include <vector>

#include <odCore/RefCounted.h>

#include <odCore/gui/Widget.h>

namespace odRender
{
    class GuiQuad;
}

namespace odGui
{

    class DrawableWidget : public Widget
    {
    public:

        virtual ~DrawableWidget();


    protected:

        void addDrawable(odRender::GuiQuad *quad);
        void removeDrawable(odRender::GuiQuad *quad);


    private:

        std::vector<od::RefPtr<odRender::GuiQuad>> mDrawables;

    };

}

#endif /* INCLUDE_ODCORE_GUI_DRAWABLEWIDGET_H_ */
