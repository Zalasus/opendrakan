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
#include <osg/Uniform>

#include <odCore/render/ObjectNode.h>

#include <odOsg/render/LightState.h>

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

        virtual glm::vec3 getPosition() override;
        virtual glm::quat getOrientation() override;
        virtual glm::vec3 getScale() override;

        virtual void setPosition(const glm::vec3 &pos) override;
        virtual void setOrientation(const glm::quat &orientation) override;
        virtual void setScale(const glm::vec3 &scale) override;

        /**
         * @brief Sets the global directional light that is always applied to the object if it is shaded.
         *
         * @param diffuse    Diffuse component of the light
         * @param ambient    Ambient component of the light
         * @param direction  Direction vector pointing towards the light source
         */
        virtual void setGlobalLight(const glm::vec3 &diffuse, const glm::vec3 &ambient, const glm::vec3 &direction) override;
        virtual void setLocalLightMask(uint32_t localLightMask) override;

        virtual odRender::ModelNode *getModel() override;
        virtual void setModel(odRender::ModelNode *node) override;

        virtual void setVisible(bool visible) override;
        virtual void setModelPartVisible(size_t partIndex, bool visible) override;

        virtual void setRenderMode(RenderMode rm) override;

        virtual void addFrameListener(odRender::FrameListener *listener) override;
        virtual void removeFrameListener(odRender::FrameListener *listener) override;

        virtual void setEnableColorModifier(bool b) override;
        virtual void setColorModifier(const glm::vec4 &cm) override;

        virtual odRender::Rig *getRig() override;


    private:

        Renderer *mRenderer;
        osg::ref_ptr<osg::Group> mObjectGroup;
        od::RefPtr<ModelNode> mModelNode;
        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;
        osg::ref_ptr<LightStateAttribute> mLightStateAttribute;
        osg::ref_ptr<osg::Depth> mDepth;
        odRender::FrameListener *mFrameListener;
        osg::ref_ptr<osg::Callback> mUpdateCallback;
        std::unique_ptr<Rig> mRig;
        osg::ref_ptr<osg::Uniform> mColorModifierUniform;
    };

}


#endif /* INCLUDE_ODOSG_OBJECTNODE_H_ */
