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

#include <odCore/WeakRefPtr.h>

#include <odCore/physics/PhysicsSystem.h>

namespace odBulletPhysics
{
    struct ObjectHandle;
    struct LayerHandle;

    struct BulletCollisionGroups
    {
        enum Masks
        {
            RAYCAST  = 0x0001, // bit 0 reserved by bullet for raytests
            LAYER    = 0x0002,
            OBJECT   = 0x0004,
            DETECTOR = 0x0008,
            LIGHT    = 0x0010,
            ALL      = 0xffff
        };
    };

    /**
     * @brief PhysicsSystem implementation using the Bullet physics engine.
     *
     * Since this is a non-optional component and I have no plans to provide alternatives to bullet yet,
     * this still is contained within the engine core, making Bullet a dependency.
     */
    class BulletPhysicsSystem : public odPhysics::PhysicsSystem
    {
    public:

        BulletPhysicsSystem();
        virtual ~BulletPhysicsSystem();

        virtual size_t rayTest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::RayTestResultVector &resultsOut) override;
        virtual bool rayTestClosest(const glm::vec3 &from, const glm::vec3 &to, odPhysics::PhysicsTypeMasks::Mask typeMask, odPhysics::Handle *exclude, odPhysics::RayTestResult &resultOut) override;

        virtual size_t contactTest(const glm::vec3 &v) override;

        virtual od::RefPtr<odPhysics::ObjectHandle> createObjectHandle(od::LevelObject &obj) override;
        virtual od::RefPtr<odPhysics::LayerHandle>  createLayerHandle(od::Layer &layer) override;
        virtual od::RefPtr<odPhysics::LightHandle>  createLightHandle(od::Light &light) override;

        virtual od::RefPtr<odPhysics::ModelShape> createModelShape(odDb::Model &model) override;


    private:

        BulletCollisionGroups::Masks _toBulletMask(odPhysics::PhysicsTypeMasks::Mask mask);
        void _objectToResult(float fraction, const btVector3 &hitPoint, const btVector3 &hitNormal, const btCollisionObject *object, odPhysics::RayTestResult &result);

        // order is important since bullet never takes ownership!
        //  mCollisionWorld needs to be initialized last and destroyed first
        std::unique_ptr<btBroadphaseInterface> mBroadphase;
        std::unique_ptr<btCollisionConfiguration> mCollisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> mDispatcher; // depends on mCollisionConfiguration. init after that
        std::unique_ptr<btGhostPairCallback> mGhostPairCallback;
        std::unique_ptr<btCollisionWorld> mCollisionWorld;
    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_BULLETPHYSICSSYSTEM_H_ */
