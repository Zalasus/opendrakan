/*
 * ObjectNode.h
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_OBJECTNODE_H_
#define INCLUDE_ODOSG_OBJECTNODE_H_

#include <memory>

#include <osg/PositionAttitudeTransform>
#include <osg/Depth>
#include <osg/NodeCallback>

#include <odCore/render/ObjectNode.h>

#include <odOsg/LightState.h>

namespace odOsg
{

    class Renderer;
    class ModelNode;
    class Rig;

    class ObjectNode : public odRender::ObjectNode
    {
    public:

        ObjectNode(Renderer *renderer, osg::Group *objectGroup);
        ~ObjectNode();

        virtual void setPosition(const glm::vec3 &pos) override;
        virtual void setOrientation(const glm::quat &orientation) override;
        virtual void setScale(const glm::vec3 &scale) override;
        virtual void setLightingLayer(od::Layer *layer) override;

        virtual odRender::ModelNode *getModel() override;
        virtual void setModel(odRender::ModelNode *node) override;

        virtual void setVisible(bool visible) override;
        virtual void setModelPartVisible(size_t partIndex, bool visible) override;

        virtual void setRenderMode(RenderMode rm) override;

        virtual void addFrameListener(odRender::FrameListener *listener) override;
        virtual void removeFrameListener(odRender::FrameListener *listener) override;

        virtual odRender::Rig *getRig() override;


    private:

        Renderer *mRenderer;
        osg::ref_ptr<osg::Group> mObjectGroup;
        od::RefPtr<ModelNode> mModelNode;
        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;
        osg::ref_ptr<LightStateCallback> mLightStateCallback;
        osg::ref_ptr<osg::Depth> mDepth;
        odRender::FrameListener *mFrameListener;
        osg::ref_ptr<osg::Callback> mUpdateCallback;
        std::unique_ptr<Rig> mRig;
    };

}


#endif /* INCLUDE_ODOSG_OBJECTNODE_H_ */
