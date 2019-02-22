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
    : mHitFraction(0.0f)
    , mHitPoint(0.0f, 0.0f, 0.0f)
    , mHitNormal(0.0f, 0.0f, 0.0f)
    , mTypeMask(0.0f)
    , mHandle(nullptr)
    {
    }

    RayTestResult::RayTestResult(float fraction, const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, ObjectHandle *hitObject)
    : mHitFraction(fraction)
    , mHitPoint(hitPoint)
    , mHitNormal(hitNormal)
    , mTypeMask(PhysicsTypeMasks::LevelObject)
    , mHandle(hitObject)
    {
    }

    RayTestResult::RayTestResult(float fraction, const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, LayerHandle *hitLayer)
    : mHitFraction(fraction)
    , mHitPoint(hitPoint)
    , mHitNormal(hitNormal)
    , mTypeMask(PhysicsTypeMasks::Layer)
    , mHandle(hitLayer)
    {
    }

    RayTestResult::RayTestResult(float fraction, const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, LightHandle *hitLight)
    : mHitFraction(fraction)
    , mHitPoint(hitPoint)
    , mHitNormal(hitNormal)
    , mTypeMask(PhysicsTypeMasks::Light)
    , mHandle(hitLight)
    {
    }

    RayTestResult::RayTestResult(const RayTestResult &r)
    : mHitFraction(r.mHitFraction)
    , mHitPoint(r.mHitPoint)
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



