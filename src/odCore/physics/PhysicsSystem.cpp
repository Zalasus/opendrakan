/*
 * PhysicsSystem.cpp
 *
 *  Created on: Feb 17, 2019
 *      Author: zal
 */

#include <odCore/physics/PhysicsSystem.h>

#include <odCore/Exception.h>

#include <odCore/physics/Handles.h>

namespace odPhysics
{

    RayTestResult::RayTestResult()
    : mHitPoint(0, 0, 0)
    , mHitNormal(0, 0, 0)
    , mTypeMask(0)
    , mHandle(nullptr)
    {
    }

    RayTestResult::RayTestResult(const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, ObjectHandle *hitObject)
    : mHitPoint(hitPoint)
    , mHitNormal(hitNormal)
    , mTypeMask(PhysicsTypeMasks::LevelObject)
    , mHandle(hitObject)
    {
    }

    RayTestResult::RayTestResult(const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, LayerHandle *hitLayer)
    : mHitPoint(hitPoint)
    , mHitNormal(hitNormal)
    , mTypeMask(PhysicsTypeMasks::Layer)
    , mHandle(hitLayer)
    {
    }

    RayTestResult::RayTestResult(const RayTestResult &r)
    : mHitPoint(r.mHitPoint)
    , mHitNormal(r.mHitNormal)
    , mTypeMask(r.mTypeMask)
    , mHandle(r.mHandle)
    {
    }

    RayTestResult::~RayTestResult()
    {
    }

    RayTestResult &RayTestResult::operator=(const RayTestResult &r)
    {
        mHitPoint = r.mHitPoint;
        mHitNormal = r.mHitNormal;
        mTypeMask = r.mTypeMask;
        mHandle = r.mHandle;

        return *this;
    }

    ObjectHandle *RayTestResult::getObjectHandle()
    {
        if(mHandle == nullptr)
        {
            throw od::Exception("Invalid RayTestResult object");
        }

        ObjectHandle *oh = mHandle->asObjectHandle();
        if(oh == nullptr)
        {
            throw od::Exception("RayTestResult does not contain object handle");
        }

        return oh;
    }

    LayerHandle *RayTestResult::getLayerHandle()
    {
        if(mHandle == nullptr)
        {
            throw od::Exception("Invalid RayTestResult object");
        }

        LayerHandle *lh = mHandle->asLayerHandle();
        if(lh == nullptr)
        {
            throw od::Exception("RayTestResult does not contain layer handle");
        }

        return lh;
    }


}



