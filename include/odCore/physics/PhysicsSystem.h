/*
 * PhysicsSystem.h
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_
#define INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_

#include <vector>

#include <glm/vec3.hpp>

#include <odCore/RefCounted.h>

namespace od
{
    class LevelObject;
    class Layer;
}

namespace odPhysics
{
    class Handle;
    class ObjectHandle;
    class LayerHandle;

    struct PhysicsTypeMasks
    {
        PhysicsTypeMasks() = delete;

        static const uint32_t LevelObject = 1;
        static const uint32_t Layer = 2;
        static const uint32_t Light = 4;
        static const uint32_t Detector = 8;

        static const uint32_t All = -1;
    };

    class RayTestResult
    {
    public:

        RayTestResult(const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, ObjectHandle *hitObject);
        RayTestResult(const glm::vec3 &hitPoint, const glm::vec3 &hitNormal, LayerHandle *hitLayer);

        inline glm::vec3 getHitPoint() const { return mHitPoint; }
        inline glm::vec3 getHitNormal() const { return mHitNormal; }

        ObjectHandle *getObjectHandle();
        LayerHandle *getLayerHandle();


    private:

        glm::vec3 mHitPoint;
        glm::vec3 mHitNormal;

        uint32_t mTypeMask;

        od::RefPtr<Handle> mHandle;
    };

    typedef std::vector<RayTestResult> RayTestResultVector;

    /**
     * @brief Abstract interface for a physics system implementation.
     */
    class PhysicsSystem
    {
    public:

        virtual ~PhysicsSystem() = default;

        virtual size_t raycast(const glm::vec3 &from, const glm::vec3 &to, uint32_t typeMask, RayTestResultVector &resultsOut) = 0;
        virtual bool raycastClosest(const glm::vec3 &from, const glm::vec3 &to, uint32_t typeMask, Handle *exclude, RayTestResult &resultOut) = 0;

        virtual od::RefPtr<ObjectHandle> createObjectHandle(od::LevelObject &obj) = 0;
        virtual od::RefPtr<LayerHandle>  createLayerHandle(od::Layer &layer) = 0;
    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_ */
