/*
 * Inventory.h
 *
 *  Created on: Apr 6, 2019
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_GUI_INVENTORY_H_
#define INCLUDE_DRAGONRFL_GUI_INVENTORY_H_

#include <odCore/gui/Widget.h>

namespace dragonRfl
{
    class DragonGui;
    class Item;

    class Inventory : public odGui::Widget
    {
    public:

        Inventory(DragonGui &gui);

        void addItem(Item *item, int32_t slotX, int32_t slotY);


    };

}


#endif /* INCLUDE_DRAGONRFL_GUI_INVENTORY_H_ */
