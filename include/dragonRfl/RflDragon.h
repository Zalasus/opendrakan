/*
 * RflDragon.h
 *
 *  Created on: 16 Sep 2018
 *      Author: zal
 */

#ifndef INCLUDE_DRAGONRFL_RFLDRAGON_H_
#define INCLUDE_DRAGONRFL_RFLDRAGON_H_

#include <odCore/rfl/Rfl.h>

namespace od
{

    class RflDragon : public RflImpl<RflDragon>
    {
    public:

        RflDragon(Engine &engine);

    };

}

#endif /* INCLUDE_DRAGONRFL_RFLDRAGON_H_ */
