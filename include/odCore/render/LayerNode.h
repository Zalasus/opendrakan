/*
 * LayerNode.h
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_LAYERNODE_H_
#define INCLUDE_ODCORE_RENDER_LAYERNODE_H_

#include <glm/vec3.hpp>

namespace odRender
{

    class Geometry;

    class LayerNode
    {
    public:

        virtual ~LayerNode() = default;

        virtual void setPosition(const glm::vec3 &p) = 0;

        virtual void setGeometry(Geometry *geometry) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_LAYERNODE_H_ */
