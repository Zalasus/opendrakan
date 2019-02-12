/*
 * ManagedCompoundShape.h
 *
 *  Created on: Feb 12, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_BULLET_MANAGEDCOMPOUNDSHAPE_H_
#define INCLUDE_ODCORE_PHYSICS_BULLET_MANAGEDCOMPOUNDSHAPE_H_

#include <vector>
#include <memory>

#include <BulletCollision/CollisionShapes/btCompoundShape.h>

namespace odBulletPhysics
{

    /**
     * @brief A btCompoundShape with automatic memory management for it's children.
     */
    class ManagedCompoundShape : public btCompoundShape
    {
    public:

        ManagedCompoundShape(size_t initialChildShapeCapacity);

        void addManagedChildShape(btTransform xform, std::unique_ptr<btCollisionShape> shape);


    private:

        using btCompoundShape::addChildShape;

        std::vector<std::unique_ptr<btCollisionShape>> mChildShapes;
    };

}


#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_MANAGEDCOMPOUNDSHAPE_H_ */
