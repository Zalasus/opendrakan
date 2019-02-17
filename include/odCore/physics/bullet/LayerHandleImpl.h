/*
 * LayerHandleImpl.h
 *
 *  Created on: Feb 11, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_LAYERHANDLEIMPL_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_LAYERHANDLEIMPL_H_

#include <memory>

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include <odCore/physics/Handles.h>

namespace od
{
    class Layer;
}

namespace odBulletPhysics
{

    class LayerHandle : public odPhysics::LayerHandle
    {
    public:

        LayerHandle(od::Layer &layer, btCollisionWorld *collisionWorld);
        virtual ~LayerHandle();

        virtual od::Layer &getLayer() override;


    private:

        void _buildCollisionShape();

        od::Layer &mLayer;
        btCollisionWorld *mCollisionWorld;

        std::unique_ptr<btTriangleMesh> mTriMesh;
        std::unique_ptr<btCollisionShape> mShape;
        std::unique_ptr<btCollisionObject> mCollisionObject;
    };

}


#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_LAYERHANDLEIMPL_H_ */
