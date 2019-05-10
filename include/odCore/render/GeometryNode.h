/*
 * GeometryNode.h
 *
 *  Created on: May 10, 2019
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_RENDER_GEOMETRYNODE_H_
#define INCLUDE_ODCORE_RENDER_GEOMETRYNODE_H_

#include <odCore/render/RenderNode.h>

namespace odRender
{

    class Geometry;

    /**
     * A simple render node for directly rendering a set of geometries.
     */
    class GeometryNode : public odRender::RenderNode
    {
    public:

        virtual void addGeometry(Geometry *geometry) = 0;
        virtual void removeGeometry(Geometry *geometry) = 0;

    };

}


#endif /* INCLUDE_ODCORE_RENDER_GEOMETRYNODE_H_ */
