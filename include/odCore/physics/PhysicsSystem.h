/*
 * PhysicsSystem.h
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_
#define INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_

#include <vector>
#include <memory>

#include <glm/vec3.hpp>

#include <odCore/RefCounted.h>

namespace od
{
    class LevelObject;
    class Layer;
    class Light;
}

namespace odDb
{
    class Model;
}

namespace odPhysics
{
    class Handle;
    class ObjectHandle;
    class LayerHandle;
    class LightHandle;
    class ModelShape;

    struct PhysicsTypeMasks
    {
        PhysicsTypeMasks() = delete;

        typedef uint32_t Mask;

        static const Mask Ray = 1;
        static const Mask LevelObject = 2;
        static const Mask Layer = 4;
        static const Mask Light = 8;

        static const Mask All = -1;
    };

    class RayTestResult
    {
    public:

        RayTestResult();
        RayTestResult(float fraction, const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, ObjectHandle *hitObject);
        RayTestResult(float fraction, const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, LayerHandle *hitLayer);
        RayTestResult(float fraction, const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, LightHandle *hitLight);
        RayTestResult(const RayTestResult &r);
        ~RayTestResult();

        inline float getHitFraction() const { return mHitFraction; }
        inline glm::vec3 getHitPoint() const { return mHitPoint; }
        inline glm::vec3 getHitNormal() const { return mHitNormal; }
        inline PhysicsTypeMasks::Mask getType() const { return mTypeMask; }

        RayTestResult &operator=(const RayTestResult &r);

        // will throw if type does not match
        ObjectHandle *getObjectHandle();
        LayerHandle *getLayerHandle();


    private:

        float mHitFraction;
        glm::vec3 mHitPoint;
        glm::vec3 mHitNormal;

        PhysicsTypeMasks::Mask mTypeMask;

        od::RefPtr<Handle> mHandle;
    };

    typedef std::vector<RayTestResult> RayTestResultVector;


    struct ContactTestResult
    {
        od::RefPtr<Handle> handleA;
        od::RefPtr<Handle> handleB;
    };

    typedef std::vector<ContactTestResult> ContactTestResultVector;


    /**
     * @brief Abstract interface for a physics system implementation.
     */
    class PhysicsSystem
    {
    public:

        virtual ~PhysicsSystem() = default;

        virtual size_t rayTest(const glm::vec3 &from, const glm::vec3 &to, PhysicsTypeMasks::Mask typeMask, RayTestResultVector &resultsOut) = 0;
        virtual bool rayTestClosest(const glm::vec3 &from, const glm::vec3 &to, PhysicsTypeMasks::Mask typeMask, Handle *exclude, RayTestResult &resultOut) = 0;

        virtual size_t contactTest(Handle *handle, odPhysics::PhysicsTypeMasks::Mask typeMask, ContactTestResultVector &resultsOut) = 0;

        virtual od::RefPtr<ObjectHandle> createObjectHandle(od::LevelObject &obj) = 0;
        virtual od::RefPtr<LayerHandle>  createLayerHandle(od::Layer &layer) = 0;
        virtual od::RefPtr<LightHandle>  createLightHandle(od::Light &light) = 0;

        virtual od::RefPtr<ModelShape> createModelShape(odDb::Model &model) = 0;
    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_ */
