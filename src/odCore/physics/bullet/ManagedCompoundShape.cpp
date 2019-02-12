/*
 * ManagedCompoundShape.cpp
 *
 *  Created on: Feb 12, 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/ManagedCompoundShape.h>

namespace odBulletPhysics
{

    ManagedCompoundShape::ManagedCompoundShape(size_t initialChildShapeCapacity)
    : btCompoundShape(true, initialChildShapeCapacity)
    {
        mChildShapes.reserve(initialChildShapeCapacity);
    }

    void ManagedCompoundShape::addManagedChildShape(btTransform xform, std::unique_ptr<btCollisionShape> &&shape)
    {
        btCollisionShape *shapePtr = shape.get();
        mChildShapes.emplace_back(shape);

        this->addChildShape(xform, shapePtr);
    }

}

