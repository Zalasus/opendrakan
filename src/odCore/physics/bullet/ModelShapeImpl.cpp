/*
 * ModelShapeImpl.cpp
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/ModelShapeImpl.h>

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include <odCore/db/ModelBounds.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/ManagedCompoundShape.h>

namespace odBulletPhysics
{

    ModelShape::ModelShape(const odDb::ModelBounds &bounds)
    : mBounds(bounds)
    {
    }

    btCollisionShape *ModelShape::getSharedShape()
    {
        if(mSharedShape == nullptr)
        {
            mSharedShape = createNewUniqueShape();
        }

        return mSharedShape.get();
    }

    std::unique_ptr<btCollisionShape> ModelShape::createNewUniqueShape()
    {
        // TODO: possible optimization: multiple compound shapes can share their child shapes. we could thus save memory when creating scaled shapes

        auto shape = std::make_unique<ManagedCompoundShape>(mBounds.getShapeCount());

        auto hierarchy = mBounds.getHierarchy();
        auto boxes = mBounds.getBoxes();
        auto spheres = mBounds.getSpheres();

        for(size_t index = 0; index < mBounds.getShapeCount(); ++index)
        {
            size_t firstChild = hierarchy[index].first;
            glm::vec3 myTranslation;
            glm::quat myRotation;

            // Bullet does not seem to support a manual hierarchical bounds structure. Therefore, we only care about
            //  leafs in the bounding hierarchy here and ignore all shapes that have children
            if(firstChild == 0)
            {
                std::unique_ptr<btCollisionShape> newChildShape;

                if(mBounds.getShapeType() == odDb::ModelBounds::SPHERES)
                {
                    myTranslation = spheres[index].center();
                    myRotation = glm::quat(1,0,0,0);

                    newChildShape = std::make_unique<btSphereShape>(spheres[index].radius());

                }else
                {
                    myTranslation = boxes[index].center();
                    myRotation = boxes[index].orientation();

                    btVector3 halfExtends = BulletAdapter::toBullet(boxes[index].extends() * 0.5f); // btBoxShape wants half extends, so we multiply by 0.5
                    newChildShape = std::make_unique<btBoxShape>(halfExtends);
                }

                btTransform t = BulletAdapter::makeBulletTransform(myTranslation, myRotation);
                shape->addManagedChildShape(t, std::move(newChildShape));
            }
        }

        return std::move(shape);
    }

}
