/*
 * LayerNode.h
 *
 *  Created on: 26 Nov 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_LAYERNODE_H_
#define INCLUDE_ODOSG_LAYERNODE_H_

#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include <odCore/render/LayerNode.h>

#include <odOsg/render/Geometry.h>
#include <odOsg/render/LightState.h>

namespace od
{
    class Layer;
}

namespace odOsg
{
    class Renderer;

    class LayerNode : public odRender::LayerNode
    {
    public:

        LayerNode(Renderer *renderer, od::Layer *layer, osg::Group *layerGroup);
        virtual ~LayerNode();

        virtual odRender::Geometry *getGeometry() override;

        virtual void addLight(od::Light *light) override;
        virtual void removeLight(od::Light *light) override;
        virtual void clearLightList() override;


    private:

        void _buildLayerGeometry(od::Layer *layer);

        Renderer *mRenderer;
        osg::ref_ptr<osg::Group> mLayerGroup;

        osg::ref_ptr<osg::PositionAttitudeTransform> mLayerTransform;
        osg::ref_ptr<LightStateAttribute> mLightStateAttribute;
        od::RefPtr<Geometry> mGeometry;
    };

}

#endif /* INCLUDE_ODOSG_LAYERNODE_H_ */
