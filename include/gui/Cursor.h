/*
 * Cursor.h
 *
 *  Created on: 30 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_CURSOR_H_
#define INCLUDE_GUI_CURSOR_H_

#include "gui/TexturedQuad.h"
#include "gui/Widget.h"

namespace od
{

    class GuiManager;

    class Cursor : public Widget
    {
    public:

        Cursor(GuiManager &gm);

        virtual WidgetDimensionType getDimensionType() const override;
        virtual osg::Vec2 getDimensions() const override;
    };

}

#endif /* INCLUDE_GUI_CURSOR_H_ */
