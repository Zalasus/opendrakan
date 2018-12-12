/*
 * Cursor.h
 *
 *  Created on: 30 Jun 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_CURSOR_H_
#define INCLUDE_GUI_CURSOR_H_

#include <odCore/gui/Widget.h>

namespace dragonRfl
{

    class DragonGui;

    class Cursor : public odGui::Widget
    {
    public:

        Cursor(DragonGui &gui);

    };

}

#endif /* INCLUDE_GUI_CURSOR_H_ */
