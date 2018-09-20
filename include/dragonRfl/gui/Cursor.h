/*
 * Cursor.h
 *
 *  Created on: 30 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_CURSOR_H_
#define INCLUDE_GUI_CURSOR_H_

#include <odCore/gui/TexturedQuad.h>
#include <odCore/gui/Widget.h>

namespace od
{

    class GuiManager;

    class Cursor : public Widget
    {
    public:

        Cursor(GuiManager &gm);

    };

}

#endif /* INCLUDE_GUI_CURSOR_H_ */
