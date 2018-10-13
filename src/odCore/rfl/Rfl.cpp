/*
 * Rfl.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <odCore/rfl/Rfl.h>

namespace odRfl
{

    Rfl::Rfl(od::Engine &engine)
    : mEngine(engine)
    {
    }

    void Rfl::onStartup()
    {
    }

    void Rfl::onMenuToggle(bool newMode)
    {
    }

    std::vector<RflRegistrar*> &RflRegistrar::getRflRegistrarListSingleton()
    {
        static std::vector<RflRegistrar*> list;

        return list;
    }

}




