/*
 * RflManager.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: zal
 */

#include <odCore/rfl/RflManager.h>

namespace od
{

    RflManager::RflManager(Engine &e)
    : mEngine(e)
    {
        // first, instantiate all statically linked RFLs

    }

    static std::vector<RflRegistrar*> &RflManager::getRflRegistrarListSingleton()
    {
        static std::vector<RflRegistrar*> list(8);

        return list;
    }

}


