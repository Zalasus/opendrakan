/*
 * ClassRegistry.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: zal
 */

#include <dragonRfl/ClassRegistry.h>

#include <dragonRfl/classes/UserInterfaceProperties.h>

namespace dragonRfl
{

    void registerClasses(DragonRfl &rfl)
    {
        rfl.registerClass<UserInterfaceProperties>();
    }

}



