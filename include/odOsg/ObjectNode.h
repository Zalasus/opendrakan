/*
 * ObjectNode.h
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODOSG_OBJECTNODE_H_
#define INCLUDE_ODOSG_OBJECTNODE_H_

#include <odCore/render/ObjectNode.h>

#include <osg/PositionAttitudeTransform>

namespace odOsg
{

    class Renderer;
    class ModelNode;

    class ObjectNode : public odRender::ObjectNode
    {
    public:

        ObjectNode(Renderer *renderer, osg::Group *objectGroup);
        ~ObjectNode();

        virtual void setPosition(const glm::vec3 &pos) override;
        virtual void setOrientation(const glm::quat &orientation) override;
        virtual void setScale(const glm::vec3 &scale) override;

        virtual odRender::ModelNode *getModel() override;
        virtual void setModel(odRender::ModelNode *node) override;
        virtual void showModelPart(size_t index) override;
        virtual void hideModelPart(size_t index) override;


    private:

        Renderer *mRenderer;
        osg::ref_ptr<osg::Group> mObjectGroup;
        od::RefPtr<ModelNode> mModelNode;
        osg::ref_ptr<osg::PositionAttitudeTransform> mTransform;

    };

}


#endif /* INCLUDE_ODOSG_OBJECTNODE_H_ */
