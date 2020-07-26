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

namespace odRender
{
    class PhysicsDebugDrawer;
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

        static const Mask All = Ray | LevelObject | Layer | Light | Detector;
    };

    struct RayTestResult
    {
        float hitFraction;
        glm::vec3 hitPoint;
        glm::vec3 hitNormal;
        std::shared_ptr<Handle> handle;
    };

    typedef std::vector<RayTestResult> RayTestResultVector;


    struct ContactTestResult
    {
        std::shared_ptr<Handle> handle;
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
        virtual bool rayTestClosest(const glm::vec3 &from, const glm::vec3 &to, PhysicsTypeMasks::Mask typeMask, std::shared_ptr<Handle> exclude, RayTestResult &resultOut) = 0;

        virtual size_t contactTest(Handle *handle, odPhysics::PhysicsTypeMasks::Mask typeMask, ContactTestResultVector &resultsOut) = 0;

        virtual std::shared_ptr<ObjectHandle> createObjectHandle(od::LevelObject &obj, bool isDetector) = 0;
        virtual std::shared_ptr<LayerHandle>  createLayerHandle(od::Layer &layer) = 0;
        virtual std::shared_ptr<LightHandle>  createLightHandle(std::shared_ptr<od::Light> light) = 0;

        virtual std::shared_ptr<ModelShape> createModelShape(std::shared_ptr<odDb::Model> model) = 0;

        /**
         * @brief Uses createModelShape() to create a ModelShape and caches it in model. Once the cache exists, no further calls to createModelShape() are needed.
         */
        std::shared_ptr<ModelShape> getOrCreateModelShape(std::shared_ptr<odDb::Model> model);

        virtual void setEnableDebugDrawing(bool enable) = 0;
        virtual bool isDebugDrawingEnabled() = 0;
        inline void toggleDebugDrawing() { setEnableDebugDrawing(!isDebugDrawingEnabled()); }

        virtual void update(float relTime) = 0;
    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_PHYSICSSYSTEM_H_ */
