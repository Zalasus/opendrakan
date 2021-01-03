/*
 * ModelShape.h
 *
 *  Created on: 16 Feb 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_PHYSICS_MODELSHAPE_H_
#define INCLUDE_ODCORE_PHYSICS_MODELSHAPE_H_

namespace odPhysics
{

    /**
     * Abstract interface for a model's bounding shape. Implementation is provided by the physics system.
     */
    class ModelShape
    {
    public:

        virtual ~ModelShape() = default;

    };

}

#endif /* INCLUDE_ODCORE_PHYSICS_MODELSHAPE_H_ */
