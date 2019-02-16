/*
 * ModelShapeImpl.h
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_MODELSHAPEIMPL_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_MODELSHAPEIMPL_H_

#include <memory>

#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include <odCore/physics/ModelShape.h>

namespace odDb
{
    class ModelBounds;
}

namespace odBulletPhysics
{

    class ModelShape : public odPhysics::ModelShape
    {
    public:

        explicit ModelShape(const odDb::ModelBounds &bounds);

        btCollisionShape *getSharedShape();
        std::unique_ptr<btCollisionShape> createNewUniqueShape();


    private:

        const odDb::ModelBounds &mBounds;

        std::unique_ptr<btCollisionShape> mSharedShape;

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_MODELSHAPEIMPL_H_ */
