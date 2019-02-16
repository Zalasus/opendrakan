/*
 * ModelShape.h
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_MODELSHAPE_H_
#define INCLUDE_ODCORE_PHYSICS_MODELSHAPE_H_

#include <odCore/RefCounted.h>

namespace odPhysics
{

    /**
     * Abstract interface for a model's bounding shape. Implementation is provided by the physics system.
     */
    class ModelShape : public od::RefCounted
    {
    public:

        virtual ~ModelShape() = default;

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_MODELSHAPE_H_ */
