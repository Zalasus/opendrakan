/*
 * BulletPhysicsSystem.h
 *
 *  Created on: 8 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_BULLETPHYSICSSYSTEM_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_BULLETPHYSICSSYSTEM_H_

#include <memory>

#include <BulletCollision/BroadphaseCollision/btBroadphaseInterface.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionDispatch/btCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>

#include <odCore/physics/PhysicsSystem.h>

namespace odRender
{
    class Renderer;
}

namespace odBulletPhysics
{
    class ObjectHandle;
    class LayerHandle;
    class DebugDrawer;

    /**
     * @brief PhysicsSystem implementation using the Bullet physics engine.
     *
     * Since this is a non-optional component and I have no plans to provide alternatives to bullet yet,
     * this still is contained within the engine core, making Bullet a dependency.
     */
    class BulletPhysicsSystem final : public odPhysics::PhysicsSystem
    {
    public:

        /**
         * @param renderer  Optional renderer. If non-null, will be used for debug drawing
         */
        explicit BulletPhysicsSystem(odRender::Renderer *renderer);
        virtual ~BulletPhysicsSystem();

        virtual size_t rayTest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::RayTestResultVector &resultsOut) override;
        virtual bool rayTestClosest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, std::shared_ptr<odPhysics::Handle> exclude, odPhysics::RayTestResult &resultOut) override;

        virtual size_t contactTest(std::shared_ptr<odPhysics::Handle> handle, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::ContactTestResultVector &resultsOut) override;

        virtual std::shared_ptr<odPhysics::ObjectHandle> createObjectHandle(od::LevelObject &obj, bool isDetector) override;
        virtual std::shared_ptr<odPhysics::LayerHandle>  createLayerHandle(od::Layer &layer) override;
        virtual std::shared_ptr<odPhysics::LightHandle>  createLightHandle(const od::Light &light) override;

        virtual std::shared_ptr<odPhysics::ModelShape> createModelShape(std::shared_ptr<odDb::Model> model) override;

        virtual void setEnableDebugDrawing(bool enable) override;
        virtual bool isDebugDrawingEnabled() override;

        virtual void update(float relTime) override;


    private:

        // order is important since bullet never takes ownership!
        //  mCollisionWorld needs to be initialized last and destroyed first
        std::unique_ptr<btBroadphaseInterface> mBroadphase;
        std::unique_ptr<btCollisionConfiguration> mCollisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> mDispatcher; // depends on mCollisionConfiguration. init after that
        std::unique_ptr<btGhostPairCallback> mGhostPairCallback;
        std::unique_ptr<btCollisionWorld> mCollisionWorld;

        std::unique_ptr<DebugDrawer> mDebugDrawer;
    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_BULLETPHYSICSSYSTEM_H_ */
