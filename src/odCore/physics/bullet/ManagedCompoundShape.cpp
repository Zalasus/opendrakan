/*
 * ManagedCompoundShape.cpp
 *
 *  Created on: Feb 12, 2019
 *      Author: zal
 */

#include <odCore/physics/bullet/ManagedCompoundShape.h>

namespace odBulletPhysics
{

    ManagedCompoundShape::ManagedCompoundShape(size_t initialChildShapeCapacity, bool useAabbTree)
    : btCompoundShape(useAabbTree, initialChildShapeCapacity)
    {
        mChildShapes.reserve(initialChildShapeCapacity);
    }

    ManagedCompoundShape::ManagedCompoundShape(ManagedCompoundShape &shape)
    : btCompoundShape(shape.getDynamicAabbTree() != nullptr, shape.getNumChildShapes())
    , mChildShapes(shape.mChildShapes)
    {
        for(size_t i = 0; i < shape.getNumChildShapes(); ++i)
        {
            this->addChildShape(shape.getChildTransform(i), shape.getChildShape(i));
        }
    }

    void ManagedCompoundShape::addManagedChildShape(btTransform xform, std::shared_ptr<btCollisionShape> shape)
    {
        btCollisionShape *shapePtr = shape.get();
        mChildShapes.push_back(std::move(shape));

        this->addChildShape(xform, shapePtr);
    }

}

