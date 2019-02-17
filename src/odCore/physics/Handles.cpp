/*
 * Handles.cpp
 *
 *  Created on: Feb 17, 2019
 *      Author: zal
 */

#include <odCore/physics/Handles.h>

namespace odPhysics
{

    LayerHandle *Handle::asLayerHandle()
    {
        return nullptr;
    }

    ObjectHandle *Handle::asObjectHandle()
    {
        return nullptr;
    }


    LayerHandle *LayerHandle::asLayerHandle()
    {
        return this;
    }


    ObjectHandle *ObjectHandle::asObjectHandle()
    {
        return this;
    }

}

