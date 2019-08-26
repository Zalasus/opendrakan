/*
 * GuiTextures.h
 *
 *  Created on: 1 Jul 2018
 *      Author: zal
 */

#ifndef INCLUDE_GUI_GUITEXTURES_H_
#define INCLUDE_GUI_GUITEXTURES_H_

#include <odCore/SrscFile.h>

namespace dragonRfl
{

    class GuiTextures
    {
    public:

        static constexpr od::RecordId Cursor                = 0x0016;
        static constexpr od::RecordId Map_Icon              = 0x0017;

        static constexpr od::RecordId HudOrbs_1             = 0x1015; // ice, lightning, health
        static constexpr od::RecordId HudOrbs_2             = 0x1019; // fire, poison, magma

        static constexpr od::RecordId Inventory_Left        = 0x1016;
        static constexpr od::RecordId Inventory_Right       = 0x1017;
        static constexpr od::RecordId Inventory_Borders     = 0x1018;

        static constexpr od::RecordId Numbers               = 0x1020;

        static constexpr od::RecordId HudElements           = 0x1021;

        /// this + bitmap number = actual bitmap ID (bitmap numbers start at 1)
        static constexpr od::RecordId InventoryIcons_Base   = 0x1021;

        static constexpr od::RecordId Crosshair             = 0x1431;

        static constexpr od::RecordId LoadingBar            = 0x1400;

        static constexpr od::RecordId MainMenu_TopLeft      = 0x1410;
        static constexpr od::RecordId MainMenu_TopRight     = 0x1411;
        static constexpr od::RecordId MainMenu_BottomLeft   = 0x1412;
        static constexpr od::RecordId MainMenu_BottomRight  = 0x1413;

        static constexpr od::RecordId Font                  = 0x1430;

    };

}

#endif /* INCLUDE_GUI_GUITEXTURES_H_ */
