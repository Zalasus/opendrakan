/*
 * ContainerWidget.h
 *
 *  Created on: 15 Jul 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_CONTAINERWIDGET_H_
#define INCLUDE_GUI_CONTAINERWIDGET_H_

#include "gui/Widget.h"

namespace od
{

    class ContainerWidget : public Widget
    {
    public:

        ContainerWidget(GuiManager &gm);

        inline void setDimensionType(WidgetDimensionType dt) { mDimType = dt; this->updateMatrix(); }
        inline void setDimensions(const osg::Vec2 &dim) { mDimensions = dim; this->updateMatrix(); }

        virtual WidgetDimensionType getDimensionType() const override;
        virtual osg::Vec2 getDimensions() const override;

        void addWidget(Widget *w);


    private:

        WidgetDimensionType mDimType;
        osg::Vec2 mDimensions;
    };

}

#endif /* INCLUDE_GUI_CONTAINERWIDGET_H_ */
