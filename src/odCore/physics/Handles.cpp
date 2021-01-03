/*
 * Handles.cpp
 *
 *  Created on: Feb 17, 2019
 *      Author: zal
 */

#include <odCore/physics/Handles.h>

#include <odCore/LightCallback.h>

namespace odPhysics
{

    Handle::Handle()
    : mLightCallback(nullptr)
    {
    }

    LayerHandle *Handle::asLayerHandle()
    {
        return nullptr;
    }

    ObjectHandle *Handle::asObjectHandle()
    {
        return nullptr;
    }

    LightHandle *Handle::asLightHandle()
    {
        return nullptr;
    }


    LayerHandle *LayerHandle::asLayerHandle()
    {
        return this;
    }

    Handle::Type LayerHandle::getHandleType()
    {
        return Type::Layer;
    }


    ObjectHandle *ObjectHandle::asObjectHandle()
    {
        return this;
    }

    Handle::Type ObjectHandle::getHandleType()
    {
        return Type::Object;
    }


    LightHandle *LightHandle::asLightHandle()
    {
        return this;
    }

    Handle::Type LightHandle::getHandleType()
    {
        return Type::Light;
    }

    void LightHandle::addAffectedHandle(std::shared_ptr<Handle> handle)
    {
        mAffectedHandles.push_back(handle);
    }

    void LightHandle::clearLightAffection()
    {
        for(auto &weakHandle : mAffectedHandles)
        {
            if(!weakHandle.expired())
            {
                auto handle = weakHandle.lock();
                if(handle != nullptr && handle->getLightCallback() != nullptr)
                {
                    handle->getLightCallback()->removeAffectingLight(this->getLight());
                }
            }
        }

        mAffectedHandles.clear();
    }
}
