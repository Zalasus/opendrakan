/*
 * ModelShapeImpl.cpp
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/ModelShapeImpl.h>

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

#include <odCore/Exception.h>

#include <odCore/db/Model.h>
#include <odCore/db/ModelBounds.h>

#include <odCore/physics/bullet/BulletAdapter.h>
#include <odCore/physics/bullet/ManagedCompoundShape.h>

namespace odBulletPhysics
{

    ModelShape::ModelShape(std::shared_ptr<odDb::Model> model)
    : mModel(model)
    {
        OD_CHECK_ARG_NONNULL(model);

        mSharedShape = _buildFromBounds(model->getModelBounds());  // TODO: do we have to consider LODs here?
    }

    btCollisionShape *ModelShape::getSharedShape()
    {
        return mSharedShape.get();
    }

    std::unique_ptr<btCollisionShape> ModelShape::createNewUniqueShape()
    {
        return _buildFromBounds(mModel->getModelBounds());
    }

    std::unique_ptr<ManagedCompoundShape> ModelShape::_buildFromBounds(const odDb::ModelBounds &bounds) const
    {
        auto shape = std::make_unique<ManagedCompoundShape>(bounds.getShapeCount(), true);

        auto hierarchy = bounds.getHierarchy();
        auto boxes = bounds.getBoxes();
        auto spheres = bounds.getSpheres();

        for(size_t index = 0; index < bounds.getShapeCount(); ++index)
        {
            size_t firstChild = hierarchy[index].first;
            glm::vec3 myTranslation;
            glm::quat myRotation;

            // Bullet does not seem to support a manual hierarchical bounds structure. Therefore, we only care about
            //  leafs in the bounding hierarchy here and ignore all shapes that have children
            if(firstChild == 0)
            {
                std::shared_ptr<btCollisionShape> newChildShape;

                if(bounds.getShapeType() == odDb::ModelBounds::SPHERES)
                {
                    myTranslation = spheres[index].center();
                    myRotation = glm::quat(1,0,0,0);

                    newChildShape = std::make_shared<btSphereShape>(spheres[index].radius());

                }else
                {
                    myTranslation = boxes[index].center();
                    myRotation = boxes[index].orientation();

                    btVector3 halfExtends = BulletAdapter::toBullet(boxes[index].extends() * 0.5f); // btBoxShape wants half extends, so we multiply by 0.5
                    newChildShape = std::make_shared<btBoxShape>(halfExtends);
                }

                btTransform t = BulletAdapter::makeBulletTransform(myTranslation, myRotation);
                shape->addManagedChildShape(t, std::move(newChildShape));
            }
        }

        return std::move(shape);
    }



}
