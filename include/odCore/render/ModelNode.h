/*
 * ModelNode.h
 *
 *  Created on: Nov 19, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_MODELNODE_H_
#define INCLUDE_ODCORE_RENDER_MODELNODE_H_

#include <stdlib.h>
#include <vector>

#include <odCore/RefCounted.h>

namespace odRender
{

    class Geometry;

    class ModelNode : public od::RefCounted
    {
    public:

        virtual ~ModelNode() = default;

        virtual const std::vector<std::pair<float, float>> &getLods() = 0;
        virtual Geometry *getGeometry(size_t lodIndex) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_MODELNODE_H_ */
