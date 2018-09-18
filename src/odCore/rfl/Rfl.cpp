/*
 * Rfl.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <odCore/rfl/Rfl.h>

namespace od
{

    Rfl::Rfl(Engine &engine)
    : mEngine(engine)
    {
    }


    std::vector<RflRegistrar*> &RflRegistrar::getRflRegistrarListSingleton()
    {
        static std::vector<RflRegistrar*> list;

        return list;
    }

}




