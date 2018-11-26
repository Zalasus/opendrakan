/*
 * LayerNode.h
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_LAYERNODE_H_
#define INCLUDE_ODCORE_RENDER_LAYERNODE_H_

#include <odCore/RefCounted.h>

namespace odRender
{

    class Geometry;

    class LayerNode : public od::RefCounted
    {
    public:

        virtual ~LayerNode() = default;

        virtual Geometry *getGeometry() = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_LAYERNODE_H_ */
