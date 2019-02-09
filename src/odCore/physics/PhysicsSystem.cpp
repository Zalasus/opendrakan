/*
 * PhysicsSystem.cpp
 *
 *  Created on: 9 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/PhysicsSystem.h>

#include <odCore/physics/Handles.h>

namespace odPhysics
{

    ObjectHandle *TestResult::getObjectHandle()
    {
        if(mTypeMask != PhysicsTypeMasks::LevelObject)
        {
            return nullptr;
        }

        return static_cast<ObjectHandle*>(mHandle.get());
    }

    LayerHandle *TestResult::getLayerHandle()
    {
        if(mTypeMask != PhysicsTypeMasks::Layer)
        {
            return nullptr;
        }

        return static_cast<ObjectHandle*>(mHandle.get());
    }


}
