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

        explicit ManagedCompoundShape(size_t initialChildShapeCapacity, bool useAabbTree);

        /**
         * Creates a shallow copy of shape. Child shapes are shared between shape and the copy.
         */
        explicit ManagedCompoundShape(ManagedCompoundShape &shape);

        void addManagedChildShape(btTransform xform, std::shared_ptr<btCollisionShape> shape);


    private:

        using btCompoundShape::addChildShape;
        using btCompoundShape::removeChildShape;
        using btCompoundShape::removeChildShapeByIndex;

        std::vector<std::shared_ptr<btCollisionShape>> mChildShapes;
    };

}


#endif /* INCLUDE_ODCORE_PHYSICS_BULLET_MANAGEDCOMPOUNDSHAPE_H_ */
