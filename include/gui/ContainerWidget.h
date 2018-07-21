/*
 * ContainerWidget.h
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_CONTAINERWIDGET_H_
#define INCLUDE_GUI_CONTAINERWIDGET_H_

#include <deque>

#include "gui/Widget.h"

namespace od
{

    class ContainerWidget : public Widget
    {
    public:

        ContainerWidget(GuiManager &gm);

        virtual std::pair<int32_t, int32_t> getZRange() const override;

        void addWidget(Widget *w);


    private:

        osg::ref_ptr<osg::Group> mChildWidgetGroup;
        std::deque<osg::ref_ptr<Widget>> mChildWidgets;

    };

}

#endif /* INCLUDE_GUI_CONTAINERWIDGET_H_ */
