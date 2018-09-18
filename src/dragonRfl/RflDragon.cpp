/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

namespace od
{

    DragonRfl::DragonRfl(Engine &engine)
    : RflImpl<DragonRfl>(engine)
    {
    }


    OD_REGISTER_RFL(DragonRfl);

}
