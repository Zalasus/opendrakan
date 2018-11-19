/*
 * ModelNode.h
 *
 *  Created on: Nov 19, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_MODELNODE_H_
#define INCLUDE_ODCORE_RENDER_MODELNODE_H_

#include <stdlib.h>

#include <odCore/RefCounted.h>

namespace odRender
{

    class Geometry;

    class ModelNode : public od::RefCounted
    {
    public:

        virtual ~ModelNode() = default;

        virtual void addGeometry(Geometry *g) = 0;
        virtual void addGeometry(Geometry *g, size_t lodIndex) = 0;
        virtual void addGeometry(Geometry *g, size_t lodIndex, size_t partIndex) = 0;


    };

}


#endif /* INCLUDE_ODCORE_RENDER_MODELNODE_H_ */
