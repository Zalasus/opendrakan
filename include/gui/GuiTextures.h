/*
 * GuiTextures.h
 *
 *  Created on: 1 Jul 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_GUITEXTURES_H_
#define INCLUDE_GUI_GUITEXTURES_H_

#include "SrscFile.h"

namespace od
{

    class GuiTextures
    {
    public:

        static const RecordId Cursor                = 0x0016;
        static const RecordId Map_Icon              = 0x0017;

        static const RecordId Inventory_Left        = 0x1016;
        static const RecordId Inventory_Right       = 0x1017;
        static const RecordId Inventory_Borders     = 0x1018;

        static const RecordId Numbers               = 0x1020;

        /// this + bitmap number = actual bitmap ID (bitmap numbers start at 1)
        static const RecordId InventoryIcons_Base   = 0x1021;

        static const RecordId Crosshair             = 0x1431;

        static const RecordId LoadingBar            = 0x1400;

        static const RecordId MainMenu_TopLeft      = 0x1410;
        static const RecordId MainMenu_TopRight     = 0x1411;
        static const RecordId MainMenu_BottomLeft   = 0x1412;
        static const RecordId MainMenu_BottomRight  = 0x1413;

        static const RecordId Font                  = 0x1430;

    };

}

#endif /* INCLUDE_GUI_GUITEXTURES_H_ */
