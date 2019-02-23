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

#include <odCore/physics/Handles.h>

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
    class ModelShape;

    struct PhysicsTypeMasks
    {
        PhysicsTypeMasks() = delete;

        typedef uint32_t Mask;

        static const Mask Ray = 1;
        static const Mask LevelObject = 2;
        static const Mask Layer = 4;
        static const Mask Light = 8;
        static const Mask Detector = 16;

        static const Mask All = -1;
    };

    struct RayTestResult
    {
        float hitFraction;
        glm::vec3 hitPoint;
        glm::vec3 hitNormal;
        od::RefPtr<Handle> handle;
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

        virtual od::RefPtr<ObjectHandle> createObjectHandle(od::LevelObject &obj, bool isDetector) = 0;
        virtual od::RefPtr<LayerHandle>  createLayerHandle(od::Layer &layer) = 0;
        virtual od::RefPtr<LightHandle>  createLightHandle(od::Light &light) = 0;

        virtual od::RefPtr<ModelShape> createModelShape(odDb::Model &model) = 0;
    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_ */
