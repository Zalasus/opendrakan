/*
 * Rfl.cpp
 *
 *  Created on: 27 Jan 2018
 *      Author: zal
 */

#include <odCore/rfl/Rfl.h>

namespace odRfl
{

    ClassFactory *Rfl::getFactoryForClassId(ClassId id)
    {
        auto it = mRegisteredClasses.find(id);
        if(it == mRegisteredClasses.end())
        {
            return nullptr;
        }

        return &it->second.get();
    }

}
