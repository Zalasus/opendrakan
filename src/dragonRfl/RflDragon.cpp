/*
 * RflDragon.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

namespace od
{

    RflDragon::RflDragon(Engine &engine)
    : Rfl(engine)
    {
    }


    OD_REGISTER_RFL(RflDragon);

}
