/*
 * ModelNode.h
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_MODELNODE_H_
#define INCLUDE_ODOSG_MODELNODE_H_

#include <odCore/render/ModelNode.h>

#include <vector>

#include <osg/Group>

namespace odOsg
{

    class Renderer;
    class Geometry;

    class ModelNode : public odRender::ModelNode
    {
    public:

        explicit ModelNode(Renderer *renderer);

        virtual size_t addLod(float minDistance, float maxDistance) override;
        virtual void addGeometry(odRender::Geometry *g) override;
        virtual void addGeometry(odRender::Geometry *g, size_t lodIndex) override;
        virtual void addGeometry(odRender::Geometry *g, size_t lodIndex, size_t partIndex) override;
        virtual void setLightingMode(LightingMode lm) override;

        osg::Node *getOsgNode();


    private:

        Renderer *mRenderer;
        osg::ref_ptr<osg::Group> mGeometryGroup;
        osg::ref_ptr<osg::LOD> mLodNode;

        std::vector<std::pair<float, float>> mLods;
        std::vector<od::RefPtr<Geometry>> mGeometries;

    };

}


#endif /* INCLUDE_ODOSG_MODELNODE_H_ */
