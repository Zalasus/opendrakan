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

namespace odDb
{
    class Model;
}

namespace odOsg
{

    class Renderer;
    class Geometry;

    class ModelNode : public odRender::ModelNode
    {
    public:

        enum class LightingMode
        {
            Off,
            AmbientDiffuse,
            AmbientDiffuseSpecular
        };

        ModelNode(Renderer *renderer, odDb::Model *model);

        // implement odRender::ModelNode
        virtual const std::vector<std::pair<float, float>> &getLods() override;
        virtual odRender::Geometry *getGeometry(size_t lodIndex) override;

        size_t addLod(float minDistance, float maxDistance);
        void addGeometry(Geometry *g);
        void addGeometry(Geometry *g, size_t lodIndex);
        void setLightingMode(LightingMode lm);

        osg::Node *getOsgNode();


    private:

        void _buildSingleLodModelNode();
        void _buildMultiLodModelNode();

        Renderer *mRenderer;
        od::RefPtr<odDb::Model> mModel;

        osg::ref_ptr<osg::Group> mGeometryGroup;
        osg::ref_ptr<osg::LOD> mLodNode;

        std::vector<std::pair<float, float>> mLods;
        std::vector<od::RefPtr<Geometry>> mGeometries;

    };

}


#endif /* INCLUDE_ODOSG_MODELNODE_H_ */
