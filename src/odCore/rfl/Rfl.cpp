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

    ClassRegistrar *Rfl::getRegistrarForClassId(ClassId id)
    {
        auto it = mRegisteredClasses.find(id);
        if(it == mRegisteredClasses.end())
        {
            return nullptr;
        }

        return &it->second.get();
    }

}




